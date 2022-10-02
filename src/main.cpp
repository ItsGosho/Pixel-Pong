#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA_PIN 27
#define OLED_SCL_PIN 26

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define TILE_SIZE 7
#define TILE_MARGIN 10

uint8_t leftTileHead;
uint8_t leftTileTail;
uint8_t rightTileHead;
uint8_t rightTileTail;

//starting logic
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

void drawInitialLeftTile(Adafruit_SSD1306& display, uint8_t tileSize, uint8_t tileMargin) {


    uint8_t tileCenter = OLED_HEIGHT / 2;
    uint8_t tileStartingPosition = 0 + tileMargin;

    display.drawPixel(tileStartingPosition, tileCenter, WHITE);

    for (uint8_t i = 1; i < tileSize; ++i) {
        display.drawPixel(tileStartingPosition, tileCenter - i, WHITE);
        display.drawPixel(tileStartingPosition, tileCenter + i, WHITE);
    }

    display.display();
}

void drawInitialRightTile(Adafruit_SSD1306& display, uint8_t tileSize, uint8_t tileMargin) {


    uint8_t tileCenter = OLED_HEIGHT / 2;
    uint8_t tileStartingPosition = OLED_WIDTH - tileMargin;

    display.drawPixel(tileStartingPosition, tileCenter, WHITE);

    for (uint8_t i = 1; i < tileSize; ++i) {
        display.drawPixel(tileStartingPosition, tileCenter - i, WHITE);
        display.drawPixel(tileStartingPosition, tileCenter + i, WHITE);
    }

    display.display();
}

//starting logic
void drawInitialTiles(Adafruit_SSD1306& display, uint8_t tileSize, uint8_t tileMargin) {
    drawInitialLeftTile(display, tileSize, tileMargin);
    drawInitialRightTile(display, tileSize, tileMargin);
}

void setup() {

    Wire.setPins(OLED_SDA_PIN, OLED_SCL_PIN);

    Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    delay(2000);

    display.clearDisplay();

    drawBorders(display);
    drawInitialTiles(display, TILE_SIZE, TILE_MARGIN);

}

void loop() {
}