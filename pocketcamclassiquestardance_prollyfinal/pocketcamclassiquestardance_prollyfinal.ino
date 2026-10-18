// PocketCam Classique - V1, 5mp photo version

#include "esp_camera.h"
#include "SD_MMC.h"
#include "FS.h"
#include <TFT_eSPI.h>

// ai thinker esp32-cam pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define SHUTTER_PIN 13

#define FULL_PHOTO_SIZE     FRAMESIZE_QSXGA
#define FULL_PHOTO_QUALITY  8

TFT_eSPI tft = TFT_eSPI();

bool showingPhoto = false;
camera_fb_t *lastPhoto = NULL;
int photoNumber = 1;

bool lastButtonState = HIGH;
unsigned long lastPressTime = 0;

// trying to get that old kodak look
void applyKodakCcdLook() {
  sensor_t *sensor = esp_camera_sensor_get();
  if (!sensor) return;

  // bit warmer, kinda like kodak gold
  sensor->set_whitebal(sensor, 1);
  sensor->set_wb_mode(sensor, 1);     // sunny mode seems to work best
  sensor->set_brightness(sensor, 0);
  sensor->set_contrast(sensor, 1);
  sensor->set_saturation(sensor, 1);
  sensor->set_sharpness(sensor, 1);

  // less noise reduction so the pics keep some grain
  sensor->set_denoise(sensor, 0);
}

bool startCamera(pixformat_t pixelFormat, framesize_t frameSize,
                 int jpegQuality, int frameBufferCount) {
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href  = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn  = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = pixelFormat;
  config.frame_size = frameSize;
  config.jpeg_quality = jpegQuality;
  config.fb_count = frameBufferCount;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_err_t result = esp_camera_init(&config);
  if (result != ESP_OK) {
    Serial.printf("camera init failed: 0x%lx\n", (unsigned long)result);
    return false;
  }

  applyKodakCcdLook();
  return true;
}

bool startPreviewCamera() {
  return startCamera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 12, 2);
}

void showStatus(const char *message, uint16_t color) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(color, TFT_BLACK);
  tft.setCursor(10, 110);
  tft.print(message);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("starting up...");

  pinMode(SHUTTER_PIN, INPUT_PULLUP);

  // screen settings are in TFT_eSPI/User_Setup.h
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  showStatus("starting...", TFT_WHITE);

  if (!psramFound()) {
    Serial.println("no PSRAM found - go check Tools > PSRAM > enabled");
    showStatus("no PSRAM found", TFT_RED);
    while (true) delay(1000);
  }

  if (!startPreviewCamera()) {
    showStatus("camera failed", TFT_RED);
    while (true) delay(1000);
  }

  // 1 bit mode is IMPORTANT here, otherwise the camera runs out of pins
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD card didn't mount");
    showStatus("sd card failed", TFT_RED);
    while (true) delay(1000);
  }

  // find the next free photo number so we don't overwrite anything
  while (SD_MMC.exists("/photo_" + String(photoNumber) + ".jpg")) {
    photoNumber++;
  }

  Serial.println("all good, starting live view");
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  bool buttonWasPressed = checkButton();

  if (!showingPhoto) {
    camera_fb_t *frame = esp_camera_fb_get();
    if (!frame) return;

    if (!buttonWasPressed) {
      drawFrameOnScreen(frame);
      esp_camera_fb_return(frame);
      return;
    }

    // we done with the preview frame, time to switch to photo mode
    esp_camera_fb_return(frame);

    if (!saveFullResolutionPhoto()) {
      return;
    }

    // grab a new preview frame so we can see the pic we just took
    camera_fb_t *frozenFrame = esp_camera_fb_get();
    if (frozenFrame) {
      drawFrameOnScreen(frozenFrame);
      lastPhoto = frozenFrame;
      showingPhoto = true;
    }
  } else if (buttonWasPressed) {
    // press again to go back to live view
    esp_camera_fb_return(lastPhoto);
    lastPhoto = NULL;
    showingPhoto = false;
  }
}

// switch to full res, save the pic, andd then go back to live view
bool saveFullResolutionPhoto() {
  showStatus("taking photo...", TFT_WHITE);
  Serial.println("switching to full-resolution photo mode");

  esp_camera_deinit();

  if (!startCamera(PIXFORMAT_JPEG, FULL_PHOTO_SIZE,
                   FULL_PHOTO_QUALITY, 1)) {
    Serial.println("full-resolution camera mode failed");
    startPreviewCamera();
    return false;
  }

  // throw away the first frame so we get a fresh shot
  camera_fb_t *discardedFrame = esp_camera_fb_get();
  if (discardedFrame) esp_camera_fb_return(discardedFrame);

  camera_fb_t *fullPhoto = esp_camera_fb_get();
  bool saved = false;

  if (fullPhoto) {
    String filename = "/photo_" + String(photoNumber) + ".jpg";
    File photoFile = SD_MMC.open(filename, FILE_WRITE);

    if (photoFile) {
      size_t written = photoFile.write(fullPhoto->buf, fullPhoto->len);
      photoFile.close();
      saved = (written == fullPhoto->len);

      if (saved) {
        Serial.printf("saved %s (%u bytes, %ux%u)\n", filename.c_str(),
                      (unsigned)fullPhoto->len, fullPhoto->width, fullPhoto->height);
        photoNumber++;
      } else {
        Serial.println("SD card write was incomplete");
      }
    } else {
      Serial.println("couldn't open the file to save the photo");
    }
    esp_camera_fb_return(fullPhoto);
  } else {
    Serial.println("full-resolution capture failed");
  }

  // switch back to the small preview mode
  esp_camera_deinit();
  delay(100);

  if (!startPreviewCamera()) {
    showStatus("preview failed", TFT_RED);
    Serial.println("couldn't restore preview camera mode");
    while (true) delay(1000);
  }

  return saved;
}

bool checkButton() {
  bool currentReading = digitalRead(SHUTTER_PIN);
  bool result = false;

  // wait a bit so one press doesn't register multiple times
  if (currentReading != lastButtonState) lastPressTime = millis();

  if ((millis() - lastPressTime) > 50) {
    static bool stableState = HIGH;
    if (currentReading != stableState) {
      stableState = currentReading;
      if (stableState == LOW) result = true;
    }
  }

  lastButtonState = currentReading;
  return result;
}

void drawFrameOnScreen(camera_fb_t *frame) {
  // swap the bytes around or the colours look wrong
  uint16_t *pixelData = (uint16_t *)frame->buf;
  int numPixels = frame->len / 2;
  for (int i = 0; i < numPixels; i++) {
    pixelData[i] = (pixelData[i] >> 8) | (pixelData[i] << 8);
  }
  tft.pushImage(0, 0, 240, 240, pixelData);
}