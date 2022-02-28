# MyTanah

Life, Lands, Livelihood Enriched.

## Todo

- [x] Iterface with RFM95 module using lib_ttn
- [x] Develop API to interface with sensor
- [x] Port existing LMIC library to ESP-IDF environment
- [x] Setup RPi Chirpstack Gateway Bridge, Network Server and Application Server
- [x] Connect RAK2245 gateway to the Gateway Bridge
- [x] Join ChirpStack Network via OTAA/APB
- [x] Subscribe to MQTT broker and store data into local DB
- [ ] Add sleep wake feature
- [ ] Code quality improvement

## Setup Environment

To checkout this repository:

	git clone --recurse-submodules https://github.com/mi-mran/mytanah.git
	git submodule update --init
	
Install dependencies (only need to do once after git clone):

	cd mytanah/esp-idf
	./install.sh
	
Export IDF path (required every terminal session):

	. ./export.sh
	
On Linux systems, you need to add your user to "dialout" permission group to access /dev/ttyUSB* port (only need to do once):

	sudo usermod -a -G dialout <username>
	
## Build and Flash Project

To set the device configurations, go to menuconfig >> Component config >> LoRa module configuration.
You can also check/set pinout from there.
	
	cd project
	idf.py menuconfig

Ensure that you have already setup the environment properly before building the project.

Build the project. "project/build" directory will be created/updated containing required build files:

	idf.py set-target esp32
	idf.py build
	
Flash the project into ESP32-WROOM-32D. On some boards you may need to press boot button during "connecting ...". Check your port number, may not be /dev/ttyUSB0 if you have more than 1 usb device connected.

	idf.py -p /dev/ttyUSB0 flash
	
To view logs:

	idf.py -p /dev/ttyUSB0 monitor
