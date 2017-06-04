<img src="https://github.com/perdidor/GrowBox-Automation-Project/blob/master/etc/Iphone.PNG" width="300">



**GrowBox Automation project** 

# Introduction


GrowBox Automation Project (hereandafter GAP) is the complete hardware\software DIY solution for indoor (stealth) plant growing automation.
It has three main components:

1. A "master" PC driven by Debian8. See recommendations below
2. A controller board in growbox, which has own small "brain" (AtMEGA328P aka "Arduino") to control light, temperature and collect telemetry data and send to master. Includes peripheral sensors.
3. Executive compartments driven by relays on controller board (light, fans etc).

# Pre-Requisites

To get started with GAP, first make sure you have all required parts, components and tools.

# Master PC requirements

Recommended Intel-based Asus P8H61 motherboard and similar, which have full kernel drivers support for chipset and are relatively cheap one-board sulotion.
Other way, you can use any linux-compatible host as master PC, but it must have video chip supported by I2C kernel driver. GAP using video socket (VGA\DVI\HDMI)
as connection point to [I2C bus](https://en.wikipedia.org/wiki/I%c2%b2C) to communicate with controller board. No other special requirements.


# Controller board content
<img src="https://github.com/perdidor/GrowBox-Automation-Project/blob/master/etc/ControllerBoard.JPG" width="450">

powered by 5VDC 5A PSU
1. Copper-foiled fiberglass or laminate 100x150mm single-side (no inches and other f****ng english-metrics in this thread, please).
PCB layout desigh included in repository. Sprint Layout 5.0 software required.
2. Miniature PCB Relay NRP05-A-05D - 7pcs
3. Contact group Xinya 306-021-12 - 8pcs
3. 1N4841 diode - 7pcs
4. Resistor 0.25W 25Ohm - 7pcs
5. Resistor 0.25W 470Ohm - 25pcs
6. Resistor 0.25W 10KOhm - 25pcs
7. Connfly HU-3 (DS1070-3 F) + Connfly WF-3 (DS1070-3 M) set - 7pcs
8. Connfly HU-4 (DS1070-4 F) + Connfly WF-4 (DS1070-4 M) set - 1pc
9. Transistor bipolar NPN 2N2222A or any other TO-92 form-factor NPN transistor with EBC layout
10. LED (Red) 5v 10mA - 7pcs
11. LED (Amber) 5v 10mA - 1pc
12. LED (Green) 5v 10mA - 1pc
13. LED (Red\Green) 5v 10\20mA - 1pc
14. Resistor 0.25W 4.7kOhm - 3pcs
15. AtMEGA 328P-PU chip,  DIP-28 - 1pc
16. P82B715N chip, DIP-8 - 1pc
17. Ceramic capacitor 100nF - 3pcs
18. Electrolyte capacitor 47uF - 1pc
19. electrolyte capacitor 100uF - 1pc
20. Power (or reset) button from old PC case - 1pc
21. Connfly MJC (DS1027-2 LR) Jumper - 1pc
22. Connfly PLS-20 (DS1021-1x20) - 1pc (it has 20 parts so you can take as much as you need). You will need 2 pins to make jumper pins that "breaks" ground power to relays during programming, as ISP uses same MCU pins as relays!!!
23. DS18B21 temperature sensor (light temperature) - 1pc
24. DHT22 temperature\humidity sensor 


# VGA to I2C board content
<img src="https://github.com/perdidor/GrowBox-Automation-Project/blob/master/etc/VGA2IIC.JPG" width="450">

powered from VGA socket, no external power required
1. Connfly DHPBS (DS1077-15 M) - 1pc
2. Connfly HU-3 (DS1070-3 F) + Connfly WF-3 (DS1070-3 M) set - 1pc
3. Connfly HU-4 (DS1070-4 F) + Connfly WF-4 (DS1070-4 M) set - 1pc
4. Resistor 0.25W 4.7kOhm - 2pcs
5. Resistor 0.25W 470Ohm - 2pcs
6. P82B715N chip, DIP-8 - 1pc
7. Copper-foiled fiberglass or laminate 100x50mm single-side. PCB layout desigh included in repository. Sprint Layout 5.0 software required.

# Executive compartments and other growbox hardware

1. PSU 12VDC 10A - 1pc
2. PSU 5VDC 5A - 1pc
3. PC case fan 120mm 12VDC 1A(max, 0.4-0.6A will be enough), 3-wire - 2pc
4. Channel exhaust fan 100mm 110\220VAC ( + hall sensor if not have on-board. i recommend unipolar like Honeywell SS441A, 135G TO92. And a little cylinder magnet 10x3mm to activate it)
5. Aluminium heatsink 300x180x40mm (LxWxH) - 1pc
6. Full-spectrum LED 110(220) VAC 50W - 8 pcs
7. "Wind" fan 12VDC - 1pc
8. Water pump 12VDC - 1pc
9. Soil humidity sensor. I recommend DIY sensor with graphite probes. Sensors from aliexpress sucks but cheap. - 1pc
10. Soldering tools, flux etc... OR, you can order ready-to-go pre-assembled PCB with attached layout file.
11. Arduino Nano V3 for (re)programming for AtMEGA
8. Connectors, wires etc.


# So, let's start

It will not be very "verbose" instruction, and only general steps are included. It's not cheap hobby, be prepare to provide about 500USD as initial charge to get working device.

1. Etching, Assembling and soldering controller board
2. Programming MCU via on-board [PROG socket]()




3. Testing controller board (you have to write your own firmware to test relays if you want to check them before place to "production")
4. Etching, Assembling and soldering VGA to I2C board
5. Install Debian 8.3 and above to master PC
6. Connect small board to VGA socket on master PC
7. Install i2c-dev package and make auto-boot i2c kernel module, connect controller board and small board using 3-pin i2c connectors on them. See pinout on PCB layout. Use Connfly HU-3 (DS1070-3 F).
8. Check to ensure that master PC and controller board can communicate via i2c bus (see i2cdetect linux command)
9. Assemble LEDs on heatsink with thermoglue. Attach 12VDC fans and DS18B21 temp sensor on it. See pinout on PCB layout. Use Connfly HU-3 (DS1070-3 F) for temp sensor 1-Wire interface.
10. Assemble Hall sensor on exhaust fan if needed, attach connectors to cable. See pinout on PCB layout. Use Connfly HU-3 (DS1070-3 F).
11. Make your own graphite-based soil sensor or buy it on aliexpress, attach wire and connectors. Use Connfly HU-3 (DS1070-3 F).
12. Assemble watering system with pump, water tank and silicone tubes with holes. Firmware provides 10sec activity on watering relay by default, you can change it as you wish.
13. Assemble all the stuff in growbox. Connect PSUs. Connect output ground sockets between 12VDC and 5VDC PSUs!!! Check connections triple times, as dangerous high voltage presents on controller board and relays controls it.
14. Test it without load (lights etc), just "click" relays remotely with i2cset command to check all is OK.
15. Install MySQL and libmysql. Create db and a table:

```
$ mysql -u root -p
$ mysql> CREATE DATABASE GrowBox;
$ mysql> USE GrowBox;
$ mysql> CREATE TABLE GrowBoxTelemetry (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, datetime VARCHAR(20) NOT NULL DEFAULT 0, lightfanon BOOL NOT NULL DEFAULT 0, lightfanrpm INT NOT NULL DEFAULT 0, mainfanon BOOL NOT NULL DEFAULT 0, mainfanrpm INT NOT NULL DEFAULT 0, lighttemp FLOAT(4,2) NOT NULL DEFAULT '0.00', lighton BOOL NOT NULL DEFAULT 0, ambienttemp FLOAT(4,2) NOT NULL DEFAULT 0.00, ambienthum FLOAT(4,2) NOT NULL DEFAULT 0.00, soilsensorvalue FLOAT(4,2) NOT NULL DEFAULT 0.00, sprinkleron BOOL NOT NULL DEFAULT 0, wateringon BOOL NOT NULL DEFAULT 0, reservedon BOOL NOT NULL DEFAULT 0, windon BOOL NOT NULL DEFAULT 0, heartbeats INT NOT NULL DEFAULT 0, timestamp int(11) NOT NULL DEFAULT 0);
```

16. Compile included GrowBox2File and GrowBox_CLI sources. I recommend Visual Studio 2017 Enterprise. Yeah, torrents will help) Make GrowBox2File auto-start.
17. Install [Homebridge](https://github.com/nfarina/homebridge)
18. Install [npm](https://github.com/npm/npm)
19. Place included homebridge plugins folders to /usr/lib/node-modules/ directory. Install them via npm locally, each folder. Make homebridge autostart.
20. Edit homebridge config.json, example incliuded.
21. Start GrowBox2File and check status LED on controller board becomes green with red blinks once per 2sec, it's really great as indicates successful communication with master PC.
22. Start homebridge, pair with your IOS device via HomeKit App. Check Sensors data. Edit Crontab (see etc folder for example). It will switch relays on/off.
23. Connect power load devices to controller board.
24. Check it again.
25. That's it. Now you're ready to grow in automated mode.
26. Rewrite software as you wish to make your own control algorithm.

# That's it!

Yep, it's really not easy. It took about 3 months from idea to device and about 600USD. BUT, it will take many times more to buy device like this.

# Acknowledgements

GAP is absolutely free to copy, distribute and rewrite. It's for community.

# Thanks

Special thanks to [Chip And Dip Shop](http://www.chipdip.ru)
Thanks for reading this completely. Sorry for my poor English.
