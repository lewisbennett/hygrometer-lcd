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
	lcd.backlight();

	// Set static text on LCD.
	lcd.setCursor(0, 0);
	lcd.print("Temperature:");

	lcd.setCursor(0, 1);
	lcd.print("Rel. humidity:");

	lcd.setCursor(0, 2);
	lcd.print("Abs. humidity:");
}

/*
 * @brief Runs continuously.
 */
void loop() {

	uint8_t relativeHumidity = dht.readHumidity();

	uint8_t column = 0;

	if (!isnan(relativeHumidity)) {

		column = 15;

		lcd.setCursor(column, 1);
		lcd.print(relativeHumidity);

		column = relativeHumidity < 10 ? 16 : 17;

		lcd.setCursor(column, 1);
		lcd.print("%");

		column++;

		// Clear the remaining columns.
		while (column < LCD_COLUMNS) {

			lcd.setCursor(column, 1);
			lcd.print(" ");

			column++;
		}
	}

	float temperature = displayTemperature(13, 0) / 10.0;
	
	// Absolute humidity can't be calculated without both values.
	if (isnan(relativeHumidity) || isnan(temperature))
		return;

	// Formula from: https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
	float absoluteHumidity = (6.112 * pow(2.71828, (17.67 * temperature) / (temperature + 243.5)) * (relativeHumidity / 100.0) * 2.1674) / (273.15 + temperature);

	column = 15;

	lcd.setCursor(column, 2);
	lcd.print(absoluteHumidity);
}

#pragma endregion

#pragma region Value Displaying Functions

/*
 * @brief Displays the latest temperature reading on the LCD.
 * @param column The starting column for printing values.
 * @param row The row to print values on.
 * @returns Returns the temperature reading, multiplied by 10 (to avoid floating point math).
 */
int displayTemperature(uint8_t column, uint8_t row) {

	// Get the temperature, but multiply by 10 to avoid floating point math.
	int temperature_X10 = dht.readTemperature() * 10;

	if (isnan(temperature_X10))
		return NAN;

	// Get the original temperature with no decimals. We'll use this to separate the decimal value from the whole value.
	int temperature_Int = temperature_X10 / 10;

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
		column += 1;

	lcd.setCursor(column, row);

	// Isolate the decimal value. The value must also be absolute, as we don't care whether it's negative at this point.
	uint8_t temperature_Decimal = abs(temperature_X10 - (temperature_Int * 10));

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

	return temperature_X10;
}

#pragma endregion
