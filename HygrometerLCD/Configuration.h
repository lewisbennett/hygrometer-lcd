/*
 * @brief Defines the row on the LCD to display temperature. Requires DISPLAY_ABSOLUTE_HUMIDITY.
 */
#define ABSOLUTE_HUMIDITY_LCD_ROW 2

/*
 * @brief Defines the pin that the DHT sensor is connected to.
 */
#define DHT_PIN 2

/*
 * @brief Defines the type of DHT sensor being used.
 */
#define DHT_TYPE DHT22

/*
 * @brief Defines whether to show absolute humidity readings. Comment/uncomment this line to toggle display.
 * Requires DISPLAY_RELATIVE_HUMIDITY and DISPLAY_TEMPERATURE.
 */
#define DISPLAY_ABSOLUTE_HUMIDITY

/*
 * @brief Defines whether to show relative humidity readings. Comment/uncomment this line to toggle display.
 */
#define DISPLAY_RELATIVE_HUMIDITY

/*
 * @brief Defines whether to display temperature readings. Comment/uncomment this line to toggle display.
 */
#define DISPLAY_TEMPERATURE

/*
 * @brief Defines how long to wait between making hygro readings, in milliseconds.
 */
#define HYGRO_READING_INTERVAL_MILLIS 2000

/*
 * @brief Defines the I2C address for the LCD screen.
 */
#define LCD_I2C_ADDRESS 0x27

/*
 * @brief Defines the number of columns on the LCD.
 */
#define LCD_COLUMNS 20

/*
 * @brief Defines the number of rows on the LCD.
 */
#define LCD_ROWS 4

/*
 * @brief Defines the pin that the PIR sensor is connected to. Requires USE_PIR.
 */
#define PIR_PIN 3

/*
 * @brief Defines the row on the LCD to display temperature.  Requires DISPLAY_RELATIVE_HUMIDITY.
 */
#define RELATIVE_HUMIDITY_LCD_ROW 1

/*
 * @brief Defines the row on the LCD to display temperature.  Requires DISPLAY_TEMPERATURE.
 */
#define TEMPERATURE_LCD_ROW 0

/*
 * @brief Defines whether a PIR sensor is in use to control the LCD's backlight.
 */
#define USE_PIR