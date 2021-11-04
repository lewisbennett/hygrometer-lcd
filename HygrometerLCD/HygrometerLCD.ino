#pragma region Includes

// Application configuration.
#include "Configuration.h"

// DHT library for DHT11 sensor.
#include <DHT.h>

// LCD library for interacting with a screen.
#include <LiquidCrystal_I2C.h>

#pragma endregion

#pragma region Variables

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

#pragma endregion

#pragma region Core Functions

/*
 * @brief Ran once at startup.
 */
void setup() {

	// Start the DHT sensor.
	dht.begin();

	// Initialize the LCD.
	lcd.init();
	

	// Configure PIR input.
#ifdef USE_PI

	pinMode(PIR_PIN, INPUT);

#else

	lcd.backlight();

#endif


	// Set static text on LCD.
#ifdef DISPLAY_TEMPERATURE

	lcd.setCursor(0, TEMPERATURE_LCD_ROW);
	lcd.print("Temperature:");

#endif

#ifdef DISPLAY_RELATIVE_HUMIDITY

	lcd.setCursor(0, RELATIVE_HUMIDITY_LCD_ROW);
	lcd.print("Rel. humidity:");

#endif

#if defined(DISPLAY_ABSOLUTE_HUMIDITY) && defined(DISPLAY_TEMPERATURE) && defined(DISPLAY_RELATIVE_HUMIDITY)

	lcd.setCursor(0, ABSOLUTE_HUMIDITY_LCD_ROW);
	lcd.print("Abs. humidity:");

#endif
}

unsigned long nextHygroReading_Millis;

/*
 * @brief Runs continuously.
 */
void loop() {

#ifdef USE_PIR

	if (digitalRead(PIR_PIN) == HIGH)
		lcd.backlight();

	else
		lcd.noBacklight();

#endif

	unsigned long now = millis();

	if (now < nextHygroReading_Millis)
		return;

#ifdef DISPLAY_TEMPERATURE

	float temperature = displayTemperature(13, TEMPERATURE_LCD_ROW);

#endif

#ifdef DISPLAY_RELATIVE_HUMIDITY

	uint8_t relativeHumidity = displayRelativeHumidity(15, RELATIVE_HUMIDITY_LCD_ROW);

#endif

#if defined(DISPLAY_ABSOLUTE_HUMIDITY) && defined(DISPLAY_TEMPERATURE) && defined(DISPLAY_RELATIVE_HUMIDITY)

	displayAbsoluteHumidity(15, ABSOLUTE_HUMIDITY_LCD_ROW, relativeHumidity, temperature);

#endif

	nextHygroReading_Millis = now + HYGRO_READING_INTERVAL_MILLIS;
}

#pragma endregion

#pragma region Value Displaying Functions

/*
 * @brief Calculates and displays the latest absolute humidity reading on the LCD.
 * @param column The starting column for printing values.
 * @param row The row to print values on.
 * @param relativeHumidity The latest relative humidity reading.
 * @param temperature_X10 The latest temperature reading.
 */
void displayAbsoluteHumidity(uint8_t column, uint8_t row, uint8_t relativeHumidity, float temperature) {

	// Absolute humidity can't be calculated without both values.
	if (isnan(relativeHumidity) || isnan(temperature))
		return;

	// Formula for calculating absolute humidity sourced from:
	// https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
	
	// Absolute humidity calculated in grams/m^3.
	float absoluteHumidity = (6.112 * pow(2.71828, (17.67 * temperature) / (temperature + 243.5)) * (relativeHumidity / 100.0) * 2.1674) / (273.15 + temperature);

	// Separate the whole number from the decimals.
	uint8_t absoluteHumidity_Int = (uint8_t)absoluteHumidity;
	uint8_t absoluteHumidity_Decimals = (absoluteHumidity - absoluteHumidity_Int) * 100;

	lcd.setCursor(column, row);
	lcd.print(absoluteHumidity_Int);

	column += absoluteHumidity_Int >= 10 ? 2 : 1;

	lcd.setCursor(column, row);

	if (absoluteHumidity_Decimals != 0) {

		lcd.print(".");

		column++;
		lcd.setCursor(column, row);

		if (absoluteHumidity_Decimals == 10)
			lcd.print("1");

		else {

			if (absoluteHumidity_Decimals < 10) {

				lcd.print("0");

				column++;

				lcd.setCursor(column, row);
			}

			lcd.print(absoluteHumidity_Decimals);
		}

		// If the absolute humidity only has one decimal place, it will be a multiple of 10 when multiplied by 100.
		column += absoluteHumidity_Decimals % 10 == 0 ? 1 : 2;

		lcd.setCursor(column, row);
	}

	// Clear the remaining columns.
	while (column < LCD_COLUMNS) {

		lcd.setCursor(column, row);
		lcd.print(" ");

		column++;
	}
}

/*
 * @brief Displays the latest relative humidity reading on the LCD.
 * @param column The starting column for printing values.
 * @param row The row to print values on.
 * @returns Returns the relative humidity reading.
 */
uint8_t displayRelativeHumidity(uint8_t column, uint8_t row) {

	uint8_t relativeHumidity = dht.readHumidity();

	if (isnan(relativeHumidity))
		return NAN;

	lcd.setCursor(column, row);
	lcd.print(relativeHumidity);

	// Increment the column by the character count of the value.
	// Relative humidity is shown as a percentage, so we only need to be concered with values 0 - 100.
	if (relativeHumidity == 100)
		column += 3;

	else if (relativeHumidity > 10)
		column += 2;

	else
		column++;

	lcd.setCursor(column, row);
	lcd.print("%");

	column++;

	// Clear the remaining columns.
	while (column < LCD_COLUMNS) {

		lcd.setCursor(column, row);
		lcd.print(" ");

		column++;
	}

	return relativeHumidity;
}

/*
 * @brief Displays the latest temperature reading on the LCD.
 * @param column The starting column for printing values.
 * @param row The row to print values on.
 * @returns Returns the temperature reading.
 */
float displayTemperature(uint8_t column, uint8_t row) {

	float temperature = dht.readTemperature();

	if (isnan(temperature))
		return NAN;

	// Get the original temperature with no decimals. We'll use this to separate the decimal value from the whole value.
	int16_t temperature_Int = (int16_t)temperature;

	lcd.setCursor(column, row);
	lcd.print(temperature_Int);

	// The next column will differ depending on the character count of the temperature reading. For example:
	// 0 - 9: one character;
	// -1 - -9, 10 - 99: two characters;
	// -10 - -99: three characters.
	// The DHT11 and DHT22 can't read high or low enough for triple digits, so we only need to be concerned with the above.
	if (temperature_Int <= -10)
		column += 3;

	else if (temperature_Int >= 10 || temperature_Int < 0)
		column += 2;

	else
		column++;

	lcd.setCursor(column, row);

	// Isolate the decimal value. The value must also be absolute, as we don't care whether it's negative at this point.
	uint8_t temperature_Decimal = abs(temperature - temperature_Int) * 10;

	if (temperature_Decimal != 0) {

		lcd.print(".");

		column++;

		lcd.setCursor(column, row);
		lcd.print(temperature_Decimal);

		column++;

		lcd.setCursor(column, row);
	}

	lcd.print((char)223);		// Prints the degree symbol.

	column++;

	lcd.setCursor(column, row);
	lcd.print("C");

	column++;

	// Clear the remaining columns.
	while (column < LCD_COLUMNS) {

		lcd.setCursor(column, row);
		lcd.print(" ");

		column++;
	}

	return temperature;
}

#pragma endregion
