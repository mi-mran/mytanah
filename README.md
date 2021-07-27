# MyTanah

Life, Lands, Livelihood Enriched.

## Todo

- [x] Added example project to test building and flashing
- [ ] Add example code for LoRa E32 transmitter and receiver (when adding code, please upload documentation of specific pinouts used)
- [ ] Discuss network topology of transceiver system

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

Ensure that you have already setup the environment properly before building the project.

Build the project. "project/build" directory will be created/updated containing required build files:

	cd project
	idf.py set-target esp32
	idf.py build
	
Flash the project into ESP32-WROOM-32D. On some boards you may need to press boot button during "connecting ...". Check your port number, may not be /dev/ttyUSB0 if you have more than 1 usb device connected.

	idf.py -p /dev/ttyUSB0 flash
	
To view logs:

	idf.py monitor
