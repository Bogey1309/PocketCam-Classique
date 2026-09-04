**Hardware Architecture**

PocketCam Classique V1 is built from an ESP32CAM with an OV5640 camera module.

Contents:
- ESP32-CAM (AI Thinker)
- OV5640 5MP camera (attached to the ESP32-CAM)
- 1.3" 240x240 ST7789 SPI TFT display, (doesn't have a CS pin)
- microsd card, used in SD_MMC 1-bit mode to save GPIO pins
- 3.7V 500mAh LiPo battery
- IP5310 boost converter (also charging module)
- master power switch (SPST toggle, sits between the IP5310 output and the ESP32-CAM's 5V input)
- shutter button (tactile switch)

The ESP32-CAM only exposes only a few GPIO pins once the camera and onboard PSRAM are accounted for, so component choice and wiring were planned around that  rather than picked first and fitted in later.

Development:
breadboard and jumper cables will be used for the prototype before soldering anything into the final enclosure.
