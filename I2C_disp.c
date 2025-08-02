#include "I2C_disp.h"

/* Quick helper function for single byte transfers */
void i2c_write_byte(struct i2c_client* client, uint8_t val) {
    i2c_smbus_write_byte(client, val);
}

void lcd_toggle_enable(struct i2c_client* client, uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
#define DELAY_US 600
    usleep_range(DELAY_US, DELAY_US+50);
    i2c_write_byte(client, val | LCD_ENABLE_BIT);
    usleep_range(DELAY_US, DELAY_US+50);
    i2c_write_byte(client, val & ~LCD_ENABLE_BIT);
    usleep_range(DELAY_US, DELAY_US+50);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(struct i2c_client* client, uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(client, high);
    lcd_toggle_enable(client, high);
    i2c_write_byte(client, low);
    lcd_toggle_enable(client, low);
}

void lcd_clear(struct i2c_client* client) {
    lcd_send_byte(client, LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(struct i2c_client* client, int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(client, val, LCD_COMMAND);
}

void lcd_char(struct i2c_client* client, char val) {
    lcd_send_byte(client, val, LCD_CHARACTER);
}

void lcd_string(struct i2c_client* client, const char *s) {
    while (*s) {
        lcd_char(client, *s++);
    }
}