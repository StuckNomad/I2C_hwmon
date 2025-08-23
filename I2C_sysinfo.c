#include "I2C_sysinfo.h"

void get_thermal_info(int* temp){
    struct thermal_zone_device *tz;
    tz = thermal_zone_get_zone_by_name("cpu-thermal");

    if(tz == NULL){
        printk("I2C_LCD: Failed to get thermal zone\n");
    }

    thermal_zone_get_temp(tz, temp);
    *temp  /= 1000;
    return;
}

void get_mem_info(int* usage){
    struct sysinfo i;
    si_meminfo(&i);
    long available = si_mem_available();
    *usage = ((i.totalram-available)*100)/i.totalram;
    // printk("Total: %ld, Free: %ld, TH: %ld, FH: %ld, MU: %d, AV: %ld\n", i.totalram, i.freeram, i.totalhigh, i.freehigh, i.mem_unit, si_mem_available());
    return;
}