#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA_PIN 27
#define OLED_SCL_PIN 26

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define TILE_SIZE 10

void drawBorders(Adafruit_SSD1306& display) {
    for (int16_t column = 0; column < OLED_WIDTH; ++column) {
        display.drawPixel(column, 0, WHITE);
        display.drawPixel(column, OLED_HEIGHT - 1, WHITE);
    }

    for (int16_t row = 0; row < OLED_HEIGHT; ++row) {
        display.drawPixel(0, row, WHITE);
        display.drawPixel(OLED_WIDTH - 1, row, WHITE);
    }

    display.display();
}


void setup() {

    Wire.setPins(OLED_SDA_PIN, OLED_SCL_PIN);

    Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    delay(2000);

    display.clearDisplay();

    drawBorders(display);


}

void loop() {
}