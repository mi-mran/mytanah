# How to connect the components

There are a total of 6 components
1. ESP32
2. NPK Sensor
3. TTL-RS485 Converter
4. RFM95 Module
5. Battery
6. Battery Clip Connector

## ESP32 to TTL-RS485 to NPK Sensor

    ESP32       TTL-RS485           NPK         9V Battery
    RX2 ==========  R0 
    TX2 ==========  DI
    D5  ==========  DE
    D18 ==========  RE
                    B   ========== Blue
                    A   ========== Yellow
    VIN ==========  VCC ========== Brown ========== +
    GND ==========  GND ========== Black ========== -
    
## TODO
- Add in RFM95 module
