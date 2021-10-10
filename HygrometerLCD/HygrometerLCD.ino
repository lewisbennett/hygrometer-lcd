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
	float temperature = dht.readTemperature();

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

	column = 0;

	if (!isnan(temperature)) {

		int temperatureInt = (uint16_t)temperature;

		column = 13;

		lcd.setCursor(column, 0);
		lcd.print(temperatureInt);

		if (temperatureInt >= 0)
			column = temperatureInt < 10 ? 14 : 15;

		else
			column = temperatureInt > -10 ? 15 : 16;

		lcd.setCursor(column, 0);

		float temperatureDecimal = temperature - temperatureInt;

		if (temperatureDecimal != 0) {

			lcd.print(".");

			lcd.setCursor(column += 1, 0);
			lcd.print((uint8_t)(temperatureDecimal * 10));

			lcd.setCursor(column += 1, 0);
		}

		lcd.print((char)223);		// Prints the degree symbol.
		lcd.setCursor(column += 1, 0);
		lcd.print("C");

		column++;

		// Clear the remaining columns.
		while (column < LCD_COLUMNS) {

			lcd.setCursor(column, 0);
			lcd.print(" ");

			column++;
		}
	}

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
