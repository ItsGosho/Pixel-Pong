#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "ButtonEnhanced.h"
#include "TwoDRObject.h"

using namespace itsgosho;

#define OLED_SDA_PIN 27
#define OLED_SCL_PIN 26

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define BTN_LEFT_TILE_PIN 21
#define BTN_RIGHT_TILE_PIN 22

//10MS
#define BALL_SPEED_US 10000


struct Tile {
    TwoDRObject* object;
    Direction direction;

    Tile(TwoDRObject* object, Direction direction) {
        this->object = object;
        this->direction = direction;
    }
};

struct PixelBall {
    TwoDRObject* object;
    Direction direction;

    PixelBall(TwoDRObject* object, Direction direction) {
        this->object = object;
        this->direction = direction;
    }
};

Adafruit_SSD1306* oledDisplay;

ButtonEnhanced leftTileBtn;
ButtonEnhanced rightTileBtn;

TwoDRObject* tileLeftObject;
TwoDRObject* tileRightObject;
TwoDRObject* topBorderObject;
TwoDRObject* bottomBorderObject;
TwoDRObject* leftBorderObject;
TwoDRObject* rightBorderObject;

TwoDRObject* pixelBallObject;

Tile* tileLeft;
Tile* tileRight;
PixelBall* pixelBall;

unsigned long ballSpeedUS;
unsigned long lastTimeCheckUS;

void initialize() {
    oledDisplay->clearDisplay();

    tileLeftObject = new TwoDRObject(3, 11, oledDisplay);
    tileRightObject = new TwoDRObject(3, 11, oledDisplay);

    tileLeftObject->draw({15, OLED_HEIGHT / 2 - 1}, InnerPosition::C);
    tileRightObject->draw({OLED_WIDTH - 15 - 1, OLED_HEIGHT / 2 - 1}, InnerPosition::C);

    tileLeft = new Tile(tileLeftObject, UP);
    tileRight = new Tile(tileRightObject, UP);

    topBorderObject = new TwoDRObject(OLED_WIDTH - 1, 1, oledDisplay);
    bottomBorderObject = new TwoDRObject(OLED_WIDTH - 1, 1, oledDisplay);
    leftBorderObject = new TwoDRObject(1, OLED_HEIGHT - 2 - 1, oledDisplay);
    rightBorderObject = new TwoDRObject(1, OLED_HEIGHT - 2 - 1, oledDisplay);

    topBorderObject->draw({0, 0}, InnerPosition::TL);
    bottomBorderObject->draw({0, OLED_HEIGHT - 1}, InnerPosition::TL);
    leftBorderObject->draw({0, 1}, InnerPosition::TL);
    rightBorderObject->draw({OLED_WIDTH - 1, 1}, InnerPosition::TL);

    pixelBallObject = new TwoDRObject(5, 5, oledDisplay);
    pixelBallObject->draw({OLED_WIDTH / 2, OLED_HEIGHT / 2}, InnerPosition::C);


    pixelBall = new PixelBall(pixelBallObject, static_cast<Direction>(random(2, 4))/*Random: LEFT/RIGHT*/);

    oledDisplay->display();

    ballSpeedUS = BALL_SPEED_US;
    lastTimeCheckUS = micros();
}

void setup() {
    Serial.begin(9600);

    leftTileBtn = ButtonEnhanced{BTN_LEFT_TILE_PIN};
    leftTileBtn.setHoldNotificationMs(50);

    rightTileBtn = ButtonEnhanced{BTN_RIGHT_TILE_PIN};
    rightTileBtn.setHoldNotificationMs(50);

    Wire.setPins(OLED_SDA_PIN, OLED_SCL_PIN);

    oledDisplay = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire);
    oledDisplay->begin(SSD1306_SWITCHCAPVCC, 0x3C);

    initialize();
}

void moveTile(Tile* tile) {
    if (tile->object->isUpMoveCollision(*topBorderObject) || tile->object->isDownMoveCollision(*bottomBorderObject)) {
        tile->direction = tile->direction == UP ? DOWN : UP;
    }

    tile->object->move(tile->direction);
}

void loop() {

    if (leftTileBtn.isShot() || leftTileBtn.isHold()) {
        moveTile(tileLeft);
        oledDisplay->display();
    }

    if (rightTileBtn.isShot() || rightTileBtn.isHold()) {
        moveTile(tileRight);
        oledDisplay->display();
    }

    switch (pixelBall->direction) {

        case Direction::LEFT:

            if (pixelBall->object->isLeftMoveCollision(*tileLeftObject)) {
                pixelBall->direction = static_cast<Direction>(random(6, 8)/*Random: RIGHT_UP/RIGHT_DOWN*/);
            }

            break;

        case Direction::RIGHT:

            if (pixelBall->object->isRightMoveCollision(*tileRightObject)) {
                pixelBall->direction = static_cast<Direction>(random(4, 6)/*Random: LEFT_UP/LEFT_DOWN*/);
            }

            break;

        case Direction::RIGHT_UP:

            if (pixelBall->object->isRightUpMoveCollision(*topBorderObject)) {
                pixelBall->direction = Direction::RIGHT_DOWN;
                break;
            }

            if (pixelBall->object->isRightUpMoveCollision(*tileRightObject)) {
                Direction directions[2] = {LEFT_UP, LEFT};
                pixelBall->direction = directions[random(0, 2)];
                break;
            }

        case Direction::LEFT_UP:

            if (pixelBall->object->isLeftUpMoveCollision(*topBorderObject)) {
                pixelBall->direction = Direction::LEFT_DOWN;
                break;
            }

            if (pixelBall->object->isLeftUpMoveCollision(*tileLeftObject)) {
                Direction directions[2] = {RIGHT_UP, RIGHT};
                pixelBall->direction = directions[random(0, 2)];
                break;
            }

        case Direction::RIGHT_DOWN:

            if (pixelBall->object->isRightDownMoveCollision(*bottomBorderObject)) {
                pixelBall->direction = Direction::RIGHT_UP;
                break;
            }

            if (pixelBall->object->isRightDownMoveCollision(*tileRightObject)) {
                Direction directions[2] = {LEFT_DOWN, LEFT};
                pixelBall->direction = directions[random(0, 2)];
                break;
            }

        case Direction::LEFT_DOWN:

            if (pixelBall->object->isRightDownMoveCollision(*bottomBorderObject)) {
                pixelBall->direction = Direction::LEFT_UP;
                break;
            }

            if (pixelBall->object->isRightDownMoveCollision(*tileLeftObject)) {
                Direction directions[2] = {RIGHT_DOWN, RIGHT};
                pixelBall->direction = directions[random(0, 2)];
                break;
            }
    }

    if (!pixelBall->object->isFront(*tileLeftObject) && !pixelBall->object->isBehind(*tileLeftObject)) {
        Serial.println("Left lose!");
        initialize();
    }

    if (!tileRightObject->isFront(*pixelBall->object) && !tileRightObject->isBehind(*pixelBall->object)) {
        Serial.println("Right lose!");
        initialize();
    }

    if (micros() - lastTimeCheckUS >= ballSpeedUS) {
        pixelBall->object->move(pixelBall->direction);
        oledDisplay->display();
        lastTimeCheckUS = micros();
    }
}