#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "ButtonEnhanced.h"

#define OLED_SDA_PIN 27
#define OLED_SCL_PIN 26

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define TILE_SIZE 14
#define TILE_MARGIN 10

#define UP 0
#define DOWN 1

#define BTN_LEFT_TILE_PIN 21
#define BTN_RIGHT_TILE_PIN 22

struct Point {
    uint8_t x;
    uint8_t y;
};

struct Tile {
    Point head;
    Point tail;
    uint8_t direction;
};

Tile leftTile;
Tile rightTile;

Adafruit_SSD1306 oledDisplay;

ButtonEnhanced leftTileBtn;
ButtonEnhanced rightTileBtn;

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

    for (uint8_t i = 1; i < tileSize / 2; ++i) {
        display.drawPixel(tileStartingPosition, tileCenter - i, WHITE);
        display.drawPixel(tileStartingPosition, tileCenter + i, WHITE);
    }

    display.display();

    Point tileHead{tileStartingPosition, (uint8_t) (tileCenter - ((tileSize / 2) - 1))};
    Point tileTail{tileStartingPosition, (uint8_t) (tileCenter + ((tileSize / 2) - 1))};
    leftTile = Tile{tileHead, tileTail, UP};
}

void drawInitialRightTile(Adafruit_SSD1306& display, uint8_t tileSize, uint8_t tileMargin) {


    uint8_t tileCenter = OLED_HEIGHT / 2;
    uint8_t tileStartingPosition = OLED_WIDTH - tileMargin;

    display.drawPixel(tileStartingPosition, tileCenter, WHITE);

    for (uint8_t i = 1; i < tileSize / 2; ++i) {
        display.drawPixel(tileStartingPosition, tileCenter - i, WHITE);
        display.drawPixel(tileStartingPosition, tileCenter + i, WHITE);
    }

    display.display();

    Point tileHead{tileStartingPosition, (uint8_t) (tileCenter - ((tileSize / 2) - 1))};
    Point tileTail{tileStartingPosition, (uint8_t) (tileCenter + ((tileSize / 2) - 1))};
    rightTile = Tile{tileHead, tileTail, UP};
}

//starting logic
void drawInitialTiles(Adafruit_SSD1306& display, uint8_t tileSize, uint8_t tileMargin) {
    drawInitialLeftTile(display, tileSize, tileMargin);
    drawInitialRightTile(display, tileSize, tileMargin);
}

void testClearTile(Adafruit_SSD1306& display, Tile& tile) {

    for (uint8_t i = tile.head.y; i <= tile.tail.y; ++i) {
        display.drawPixel(tile.head.x, i, BLACK);
        display.display();
        delay(500);
    }

    tile.head.y = tile.head.x;
    tile.tail.y = tile.head.x;
}

void moveTileUp(Adafruit_SSD1306& display, Tile& tile) {
    display.drawPixel(tile.head.x, tile.head.y - 1, WHITE);
    display.drawPixel(tile.head.x, tile.tail.y, BLACK);

    display.display();

    tile.head.y = tile.head.y - 1;
    tile.tail.y = tile.tail.y - 1;
}

void moveTileDown(Adafruit_SSD1306& display, Tile& tile) {
    display.drawPixel(tile.head.x, tile.head.y, BLACK);
    display.drawPixel(tile.head.x, tile.tail.y + 1, WHITE);

    display.display();

    tile.head.y = tile.head.y + 1;
    tile.tail.y = tile.tail.y + 1;
}

void moveTile(Adafruit_SSD1306& display, Tile& tile, uint8_t direction) {

    switch (direction) {
        case 0:
            moveTileUp(display, tile);
            break;
        case 1:
            moveTileDown(display, tile);
            break;
        default:
            Serial.println("Warning. Direction not implemented!");
            break;
    }
}

bool hasReachedBorder(Tile& tile) {
    if (tile.head.y <= 1) //1 Because we must take the border also
        return true;
    else if (tile.tail.y >= (OLED_HEIGHT - 2)) // -2 Because we must take the border also
        return true;
    else
        return false;
}

void moveTile(Adafruit_SSD1306& display, Tile& tile) {
    if (hasReachedBorder(tile)) {
        tile.direction = tile.direction == UP ? DOWN : UP;
    }

    moveTile(display, tile, tile.direction);
}

/*TODO: Mark the head and tail for the left/right tails. (Try to calculate them after the cycle or before not update every cycle)
 * Make a mocked button logic.
 * On each button press implement the logic from the excali
 * */
void setup() {
    Serial.begin(9600);

    leftTileBtn = ButtonEnhanced{BTN_LEFT_TILE_PIN};
    leftTileBtn.setHoldNotificationMs(50);

    rightTileBtn = ButtonEnhanced{BTN_RIGHT_TILE_PIN};
    rightTileBtn.setHoldNotificationMs(50);

    Wire.setPins(OLED_SDA_PIN, OLED_SCL_PIN);

    oledDisplay = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire);
    oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    delay(2000);

    oledDisplay.clearDisplay();

    drawBorders(oledDisplay);
    drawInitialTiles(oledDisplay, TILE_SIZE, TILE_MARGIN);

    /*testClearTile(display, leftTile);
    testClearTile(display, rightTile);*/

    /*for (int i = 0; i < 5; ++i) {
        moveTile(display, leftTile, DOWN);
        delay(1000);
    }*/

    /* while (true) {
         moveTile(display, leftTile);
         delay(1000);
     }*/
}

/*TODO: If the button is hold move the tile each 100 ms. Find the sweat spot.*/
void loop() {


    if(leftTileBtn.isShot() || leftTileBtn.isHold()) {
        Serial.println("Left move!");
        moveTile(oledDisplay, leftTile);
    }

    if(rightTileBtn.isShot() || rightTileBtn.isHold()) {
        Serial.println("Right move!");
        moveTile(oledDisplay, rightTile);
    }

}