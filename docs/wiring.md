TFT          ESP32-CAM
GND     -    GND
VCC     -    3.3V
SCL     -    GPIO3
SDA     -    GPIO33
RES     -    3.3V
DC      -    GPIO12
BLK     -    3.3V

Shutter
GPIO13 -- [Tactile Button] -- GND

Power
3.7V LiPo --> IP5310 --> [Toggle Switch] --> ESP32-CAM 5V

SD card
Uses the onboard microSD slot, wired internally to GPIO2/14/15 (SD_MMC 1-bit mode). Therefore no external wiring needed for it.
