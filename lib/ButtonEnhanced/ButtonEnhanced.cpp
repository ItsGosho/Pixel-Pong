#include "ButtonEnhanced.h"

using namespace itsgosho;

ButtonEnhanced::ButtonEnhanced() {
}

ButtonEnhanced::ButtonEnhanced(const uint8_t& buttonPin, const Configuration& config) {
    this->buttonPin = buttonPin;
    this->configuration = config;
    pinMode(this->buttonPin, INPUT);
    this->setStartMs(0);
    this->setTimeMs(0);
    this->setActionState(UNKNOWN_ACTION);
    this->setHoldNotificationLastMs(0);
}

void ButtonEnhanced::setStartMs(const unsigned long& startMs) {
    this->startMS = startMs;
}

void ButtonEnhanced::setTimeMs(const unsigned long& timeMs) {
    this->timeMS = timeMs;
}

void ButtonEnhanced::setHoldNotificationLastMs(const unsigned long& holdNotificationLastMs) {
    this->holdNotificationLastMS = holdNotificationLastMs;
}

void ButtonEnhanced::setActionState(const uint8_t& actionState) {
    this->actionState = actionState;
}

bool ButtonEnhanced::isHoldNotificationTimePassed() {
    return (millis() - this->holdNotificationLastMS) >= this->configuration.holdNotificationMS;
}

bool ButtonEnhanced::isEnteredHold() {
    return this->timeMS >= this->configuration.holdThresholdMS;
}

uint8_t ButtonEnhanced::getReadingState() {
    bool buttonRead = digitalRead(this->buttonPin);

    if (buttonRead && this->startMS == 0)
        return PRESSED_READING;
    else if (buttonRead && this->startMS > 0)
        return INTERMEDIATE_READING;
    else if (!buttonRead && this->startMS != 0)
        return RELEASED_READING;
    else
        return UNKNOWN_READING;
}

void ButtonEnhanced::processPressedReading() {
    this->startMS = millis();
}

void ButtonEnhanced::processIntermediateReading() {
    this->timeMS = millis() - this->startMS;

    if (this->isEnteredHold() && this->isHoldNotificationTimePassed()) {
        this->holdNotificationLastMS = millis();

        if (this->onHoldCallback && !this->isHoldCallbackPaused)
            this->onHoldCallback();

        this->setActionState(HOLD_ACTION);
    }
}

void ButtonEnhanced::processReleasedReading() {
    this->timeMS = millis() - this->startMS;

    if (this->timeMS >= this->configuration.shotThresholdMS && this->timeMS < this->configuration.holdThresholdMS) {

        if (!this->isTotalShotsPaused) {
            this->totalShots++;
        }

        if (this->onShotCallback && !this->isShotCallbackPaused)
            this->onShotCallback();

        this->setActionState(SHOT_ACTION);
    }

    if (this->timeMS >= this->configuration.holdThresholdMS && !this->isTotalHoldsPaused) {
        this->totalHolds++;
    }

    this->startMS = 0;
    this->timeMS = 0;
}

void ButtonEnhanced::refreshReading() {

    if (this->isCorePaused)
        return;

    switch (getReadingState()) {

        case PRESSED_READING:
            this->processPressedReading();
            break;

        case INTERMEDIATE_READING:
            this->processIntermediateReading();
            break;

        case RELEASED_READING:
            this->processReleasedReading();
            break;

        case UNKNOWN_READING:
            break;
    }
}

bool ButtonEnhanced::isShot() {
    this->refreshReading();
    uint8_t isShot = this->actionState == SHOT_ACTION;

    if (isShot) {
        this->actionState = UNKNOWN_ACTION;
        return true;
    }

    return false;
}

bool ButtonEnhanced::isHold() {
    this->refreshReading();
    uint8_t isHold = this->actionState == HOLD_ACTION;

    if (isHold) {
        this->actionState = UNKNOWN_ACTION;
        return true;
    }

    return false;
}

void ButtonEnhanced::setShotThresholdMs(const unsigned long& shotThresholdMs) {
    this->configuration.shotThresholdMS = shotThresholdMs;
}

void ButtonEnhanced::setHoldThresholdMs(const unsigned long& holdThresholdMs) {
    this->configuration.holdThresholdMS = holdThresholdMs;
}

void ButtonEnhanced::setHoldNotificationMs(const unsigned long& holdNotificationMs) {
    this->configuration.holdNotificationMS = holdNotificationMs;
}

void ButtonEnhanced::setOnShotCallback(void (* callback)()) {
    this->onShotCallback = callback;
}

void ButtonEnhanced::setOnHoldCallback(void (* callback)()) {
    this->onHoldCallback = callback;
}

unsigned long ButtonEnhanced::getTotalShots() const {
    return this->totalShots;
}

unsigned long ButtonEnhanced::getTotalHolds() const {
    return this->totalHolds;
}

void ButtonEnhanced::clearTotalShots() {
    this->totalShots = 0;
}

void ButtonEnhanced::clearTotalHolds() {
    this->totalHolds = 0;
}

void ButtonEnhanced::clearTotalsCounting() {
    this->clearTotalShots();
    this->clearTotalHolds();
}

void ButtonEnhanced::pauseTotalShotsCounting() {
    this->isTotalShotsPaused = true;
}

void ButtonEnhanced::pauseTotalHoldsCounting() {
    this->isTotalHoldsPaused = true;
}

void ButtonEnhanced::pauseTotalsCounting() {
    this->pauseTotalShotsCounting();
    this->pauseTotalHoldsCounting();
}

void ButtonEnhanced::resumeTotalShotsCounting() {
    this->isTotalShotsPaused = false;
}

void ButtonEnhanced::resumeTotalHoldsCounting() {
    this->isTotalHoldsPaused = false;
}

void ButtonEnhanced::resumeTotalsCounting() {
    this->resumeTotalShotsCounting();
    this->resumeTotalHoldsCounting();
}

bool ButtonEnhanced::getIsTotalShotsPaused() const {
    return this->isTotalShotsPaused;
}

bool ButtonEnhanced::getIsTotalHoldsPaused() const {
    return this->isTotalHoldsPaused;
}

void ButtonEnhanced::pauseShotCallback() {
    this->isShotCallbackPaused = true;
}

void ButtonEnhanced::pauseHoldCallback() {
    this->isHoldCallbackPaused = true;
}

void ButtonEnhanced::resumeShotCallback() {
    this->isShotCallbackPaused = false;
}

void ButtonEnhanced::resumeHoldCallback() {
    this->isHoldCallbackPaused = false;
}

void ButtonEnhanced::pauseCallbacks() {
    this->pauseShotCallback();
    this->pauseHoldCallback();
}

void ButtonEnhanced::resumeCallbacks() {
    this->resumeShotCallback();
    this->resumeHoldCallback();
}

bool ButtonEnhanced::getIsShotCallbackPaused() const {
    return this->isShotCallbackPaused;
}

bool ButtonEnhanced::getIsHoldCallbackPaused() const {
    return this->isHoldCallbackPaused;
}

void ButtonEnhanced::pause() {
    this->isCorePaused = true;
}

void ButtonEnhanced::resume() {
    this->isCorePaused = false;
}

bool ButtonEnhanced::getIsCorePaused() const {
    return this->isCorePaused;
}

void ButtonEnhanced::resetConfiguration() {
    this->configuration = this->getDefaultConfiguration();
}

void ButtonEnhanced::updateConfiguration(const Configuration& config) {
    this->configuration = config;
}

ButtonEnhanced::Configuration ButtonEnhanced::getDefaultConfiguration() {
    return Configuration{DEFAULT_SHOT_THRESHOLD_MS, DEFAULT_HOLD_THRESHOLD_MS, DEFAULT_HOLD_NOTIFICATION_MS};
}

void ButtonEnhanced::changePin(const uint8_t& buttonPin) {
    pinMode(buttonPin, INPUT);
    this->buttonPin = buttonPin;
}
