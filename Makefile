obj-m += I2Chwmon.o 

I2Chwmon-objs := I2C_hwmon.o  I2C_disp.o I2C_sysinfo.o

all: module dt
		echo builded device tree overlay and kernel module

module: 
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dt: testoverlay.dts
		dtc -@ -I dts -O dtb -o testoverlay.dtbo testoverlay.dts

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
		rm -rf testoverlay.dtbo