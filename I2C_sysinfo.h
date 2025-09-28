#include <linux/thermal.h>
#include <linux/mm.h>
#include <linux/time64.h>
#include <linux/timekeeping.h>
#include <linux/time_namespace.h>
#include <linux/tick.h>
#include <linux/cpumask.h>

typedef struct cpu_time_info{
    u64 prev_uptime;
    u64 prev_idletime;
}cpu_time_info;

void get_thermal_info(int* temp);
void get_mem_info(int* usage);
void get_cpu_info(int* usage, cpu_time_info* time_info);