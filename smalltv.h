#pragma once
#include "esphome.h"
#include <SPI.h>

namespace esphome {
namespace smalltv {

#define TFT_DC      0
#define TFT_RST     2
#define TFT_BL      5
#define TFT_WIDTH   240
#define TFT_HEIGHT  240
#define BAND_HEIGHT 16

class SmallTVDisplay : public display::DisplayBuffer {
 public:
  void set_writer(std::function<void(display::DisplayBuffer &)> writer) {
    writer_ = writer;
  }

  void setup() override {
    pinMode(TFT_BL, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_BL, LOW);

    // Essential SPI prime — do not remove, required for SPI Mode 3 init
    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
    digitalWrite(TFT_DC, LOW);
    SPI.transfer(0x09);
    digitalWrite(TFT_DC, HIGH);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.endTransaction();

    // Reset sequence
    digitalWrite(TFT_RST, HIGH); delay(120);
    digitalWrite(TFT_RST, LOW);  delay(120);
    digitalWrite(TFT_RST, HIGH); delay(120);

    // SW reset
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE3));
    digitalWrite(TFT_DC, LOW); SPI.transfer(0x01);
    SPI.endTransaction(); delay(150);

    // Sleep out
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE3));
    digitalWrite(TFT_DC, LOW); SPI.transfer(0x11);
    SPI.endTransaction(); delay(255);

    // Remaining init in one transaction
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE3));
    // Color mode 16bit
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x3A);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x55);
    // MADCTL
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x36);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x00);
    // Inversion on
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x21);
    // Normal display mode
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x13);
    // Display on
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x29);
    // Full window
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x2A);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x00); SPI.transfer(0x00);
                                 SPI.transfer(0x00); SPI.transfer(0xEF);
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x2B);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x00); SPI.transfer(0x00);
                                 SPI.transfer(0x00); SPI.transfer(0xEF);
    SPI.endTransaction();
  }

  void update() override {
    // Set window and start memory write
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE3));
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x2A);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x00); SPI.transfer(0x00);
                                 SPI.transfer(0x00); SPI.transfer(0xEF);
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x2B);
    digitalWrite(TFT_DC, HIGH); SPI.transfer(0x00); SPI.transfer(0x00);
                                 SPI.transfer(0x00); SPI.transfer(0xEF);
    digitalWrite(TFT_DC, LOW);  SPI.transfer(0x2C);
    SPI.endTransaction();

    static uint16_t band[TFT_WIDTH * BAND_HEIGHT];

    for (int band_y = 0; band_y < TFT_HEIGHT; band_y += BAND_HEIGHT) {
      memset(band, 0, sizeof(band));
      band_start_ = band_y;
      band_buf_ = band;

      if (writer_) writer_(*this);

      SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE3));
      digitalWrite(TFT_DC, HIGH);
      for (int i = 0; i < TFT_WIDTH * BAND_HEIGHT; i++) {
        SPI.transfer16(band[i]);
      }
      SPI.endTransaction();

      yield();
      ESP.wdtFeed();
    }

    band_buf_ = nullptr;
  }

  void draw_absolute_pixel_internal(int x, int y, Color color) override {
    if (x < 0 || x >= TFT_WIDTH || y < 0 || y >= TFT_HEIGHT) return;
    if (band_buf_ == nullptr) return;
    x = (TFT_WIDTH - 1) - x;
    int local_y = y - band_start_;
    if (local_y < 0 || local_y >= BAND_HEIGHT) return;
    uint16_t c = ((color.r >> 3) << 11) | ((color.g >> 2) << 5) | (color.b >> 3);
    band_buf_[local_y * TFT_WIDTH + x] = c;
}

  int get_height_internal() override { return TFT_HEIGHT; }
  int get_width_internal()  override { return TFT_WIDTH; }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

 protected:
  int band_start_ = 0;
  uint16_t *band_buf_ = nullptr;
  std::function<void(display::DisplayBuffer &)> writer_;
};

}  // namespace smalltv
}  // namespace esphome
