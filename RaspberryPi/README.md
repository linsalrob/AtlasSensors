#Raspberry PI Code For the Atlas Sensors (and others)

##Getting started

Before you use the Raspberry PI serial port you need to configure the PI so that you can work with the serial port. Specifically, you need to do the following:

###Disable Serial Port Login

To enable the serial port for your own use you need to disable login on the port. There are two files that need to be edited

The first and main one is **/etc/inittab**

This file has the command to enable the login prompt and this needs to be disabled. Edit the file and move to the end of the file. You will see a line similar to

```
T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
```

Disable it by adding a # character to the beginning. Save the file.

```
#T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
```

###Disable Bootup Info

When the Raspberry Pi boots up, all the bootup information is sent to the serial port. Disabling this bootup information is optional and you may want to leave this enabled as it is sometimes useful to see what is happening at bootup. If you have a device connected (i.e. Arduino) at bootup, it will receive this information over the serial port, so it is up to you to decide whether this is a problem or not. We usually disable this so that it is out of the way.

You can disable it by editing the file `/boot/cmdline.txt`

The contents of the file look like this

```
dwc_otg.lpm_enable=0 console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
```

Remove all references to *ttyAMA0* (which is the name of the serial port). The file will now look like this

```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
```

###Reboot

Once you have made these two changes you need to reboot

```
sudo shutdown -r now
```

For more information, see [hobbytronics](http://www.hobbytronics.co.uk/raspberry-pi-serial-port)


#Code

Most of this code originated at [Atlas Scientific](https://www.atlas-scientific.com/) and we have edited and changed it for our own purposes. You are free to use this code as you wish.



