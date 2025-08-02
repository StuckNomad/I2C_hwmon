#include<linux/init.h>
#include<linux/module.h>
#include<linux/mod_devicetable.h>
#include<linux/of_device.h>
#include<linux/i2c.h>
#include<linux/delay.h>

#include "I2C_disp.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nithish");
MODULE_DESCRIPTION("Driver for I2C LCD display");

static int I2C_LCD_probe(struct i2c_client *client);
static void I2C_LCD_remove(struct i2c_client *client);

static struct i2c_client *LCD_client;

static int major;

static struct of_device_id i2c_driver_ids[] = {
    {
        .compatible = "brightlight,MY_LCD",
    }, {}
};
MODULE_DEVICE_TABLE(of, i2c_driver_ids);

static struct i2c_device_id I2C_LCD[] = {
    {"I2C_LCD", 0},
    {},
};

MODULE_DEVICE_TABLE(i2c, I2C_LCD);

static struct i2c_driver my_driver = {
    .probe = I2C_LCD_probe,
    .remove = I2C_LCD_remove,
    .driver = {
        .name = "I2C_LCD",
        .of_match_table = i2c_driver_ids,
    },
};

static ssize_t LCD_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs){
    int content_len = count > 16 ? 16 : count;
    char content[16];
    int status;

    status = copy_from_user(&content, user_buffer, content_len);
    // printk("LCD_write triggered. Count: %ld, Offset: %lld, Buffer: %s\n", count, *offs, content);

    if(status){
        printk("I2C_LCD: Error copying user buffer.\n");
        return status;
    }

    if(*offs == 0)   lcd_clear(LCD_client);
    else    lcd_set_cursor(LCD_client, 2, 0);

    *offs += content_len;

    lcd_string(LCD_client, content);
    return content_len;
}

static struct file_operations fops = {
    .write = LCD_write,
};

static int I2C_LCD_probe(struct i2c_client *client){
    major = register_chrdev(0, "I2C_LCD", &fops);
    if(major < 0){
        printk("I2C_LCD: Error registering character device.\n");
        return major;
    }

    LCD_client = client;

    printk("I2C_LCD - Major device number: %d, I2C_client_addr: %x\n", major, client->addr);

    lcd_send_byte(LCD_client, 0x03, LCD_COMMAND);
    lcd_send_byte(LCD_client, 0x03, LCD_COMMAND);
    lcd_send_byte(LCD_client, 0x03, LCD_COMMAND);
    lcd_send_byte(LCD_client, 0x02, LCD_COMMAND);

    lcd_send_byte(LCD_client, LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_client, LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_client, LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear(LCD_client);

    lcd_string(LCD_client, "Hello World!");

    return 0;

}

static void I2C_LCD_remove(struct i2c_client *client){
    lcd_clear(client);
    unregister_chrdev(major, "I2C_LCD");
    return;
}

static int __init LCD_init(void){
    printk("I2C_LCD: initializing the driver.\n");
    if(i2c_add_driver(&my_driver)){
        printk("I2C_LCD: Failed to add driver.\n");
    }

    return 0;
}

static void __exit LCD_exit(void){
    printk("I2C_LCD: exiting from the driver\n");
    i2c_del_driver(&my_driver);
}

module_init(LCD_init);
module_exit(LCD_exit);
