# SensESPStatusDisplay

SensESPStatusDisplay is a sample status display for the 
[SensESP Signal K sensor device library](https://github.com/SignalK/SensESP).
The hardware is at the moment hard-coded to assume SSD1306 based OLED
displays with a 128x32 pixel resolution, such as this: 
https://www.aliexpress.com/item/32672229793.html

The display currently shows, on four separate rows:
- Wifi status or the access point SSID
- Signal K server address
- Current local IP address
- A mini-graph of the number of deltas sent

To use the status display library in your own projects, you have to include
it in your `platformio.ini` lib_deps section:

    lib_deps =
        SignalK/SensESP
        https://github.com/mairas/SensESPStatusDisplay.git#latest
