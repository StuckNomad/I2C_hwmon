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
    return;
}

void get_cpu_info(int* usage, cpu_time_info* time_info){
    struct timespec64 uptime;
    ktime_get_boottime_ts64(&uptime);
	timens_add_boottime(&uptime);
    int i=0, total_usage=0;
    for_each_possible_cpu(i) {
        u64 cpu_idle_time_us = get_cpu_idle_time_us(i, NULL);
        u64 up_time_us = uptime.tv_sec*1000000 + uptime.tv_nsec/1000;

        u64 up_time_diff = up_time_us - time_info[i].prev_uptime;
        u64 cpu_idle_time_diff = cpu_idle_time_us - time_info[i].prev_idletime;

        time_info[i].prev_uptime = up_time_us;
        time_info[i].prev_idletime = cpu_idle_time_us;

        total_usage += ((up_time_diff-cpu_idle_time_diff)*1000)/up_time_diff;
	}

    *usage = total_usage/4;

    // printk("uptime: %llu, cpu-idle: %llu\n", up_time_us, cpu_idle_time_us);
    // printk("uptime-diff: %llu, cpu-idle-diff: %llu\n", up_time_diff, cpu_idle_time_diff);
    return;
}