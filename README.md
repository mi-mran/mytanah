# MyTanah

Life, Lands, Livelihood Enriched.

## Todo

- [x] Added example project to check flashing and building
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
	
## Build and Flash Project

Ensure that you have already setup the environment properly before building the project.

Build the project. "project/build" directory will be created/updated containing required build files:

	cd project
	idf.py set-target esp32
	idf.py build
	
Flash the project into ESP32-WROOM-32D. On some boards you may need to press boot button during "connecting ..."

	idf.py flash
	
To view logs:

	idf.py monitor
