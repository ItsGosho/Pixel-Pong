#ifndef ITSGOSHO_BUTTON_ENHANCED_H
#define ITSGOSHO_BUTTON_ENHANCED_H

#include <Arduino.h>
#include <cstdint>

#define PRESSED_READING 0
#define INTERMEDIATE_READING 1
#define RELEASED_READING 2
#define UNKNOWN_READING 3

#define SHOT_ACTION 0
#define HOLD_ACTION 1
#define UNKNOWN_ACTION 2

#define DEFAULT_SHOT_THRESHOLD_MS 15
#define DEFAULT_HOLD_THRESHOLD_MS 150
#define DEFAULT_HOLD_NOTIFICATION_MS 500

class ButtonEnhanced {

public:

    struct Configuration {
        unsigned long shotThresholdMS;
        unsigned long holdThresholdMS;
        unsigned long holdNotificationMS;
    };

    ButtonEnhanced();

    ButtonEnhanced(const uint8_t& buttonPin, const Configuration& config = {DEFAULT_SHOT_THRESHOLD_MS, DEFAULT_HOLD_THRESHOLD_MS, DEFAULT_HOLD_NOTIFICATION_MS});

    void refreshReading();

    bool isHold();

    bool isShot();

    void setShotThresholdMs(const unsigned long& shotThresholdMs);

    void setHoldThresholdMs(const unsigned long& holdThresholdMs);

    void setHoldNotificationMs(const unsigned long& holdNotificationMs);

    void setOnShotCallback(void (* callback)());

    void setOnHoldCallback(void (* callback)());

    unsigned long getTotalShots() const;

    unsigned long getTotalHolds() const;

    void clearTotalShots();

    void clearTotalHolds();

    void clearTotalsCounting();

    void pauseTotalShotsCounting();

    void pauseTotalHoldsCounting();

    void pauseTotalsCounting();

    void resumeTotalShotsCounting();

    void resumeTotalHoldsCounting();

    void resumeTotalsCounting();

    bool getIsTotalShotsPaused() const;

    bool getIsTotalHoldsPaused() const;

    void pauseShotCallback();

    void pauseHoldCallback();

    void resumeShotCallback();

    void resumeHoldCallback();

    void pauseCallbacks();

    void resumeCallbacks();

    bool getIsShotCallbackPaused() const;

    bool getIsHoldCallbackPaused() const;

    void pause();

    void resume();

    bool getIsCorePaused() const;

    void resetConfiguration();

    void updateConfiguration(const Configuration& config);

    Configuration getDefaultConfiguration();

    void changePin(const uint8_t& buttonPin);

private:

    typedef void (* onShot)();        //When a button is pressed and released immediately
    //typedef void (* onPress)();       //When a button is pressed.
    typedef void (* onHold)();        //When a button is kept pressed.
    //typedef void (* onRelease)();     //When a button is released.

    uint8_t buttonPin;
    unsigned long startMS;
    unsigned long timeMS;
    unsigned long holdNotificationLastMS;

    Configuration configuration;

    unsigned long totalShots;
    unsigned long totalHolds;

    bool isCorePaused;

    bool isTotalShotsPaused;
    bool isTotalHoldsPaused;

    bool isShotCallbackPaused;
    bool isHoldCallbackPaused;

    onShot onShotCallback;
    onHold onHoldCallback;

    uint8_t actionState;

    void setStartMs(const unsigned long& startMs);

    void setTimeMs(const unsigned long& timeMs);

    void setHoldNotificationLastMs(const unsigned long& holdNotificationLastMs);

    void setActionState(const uint8_t& actionState);

    /**
     * When a button is kept hold we don't want to inform the user that fast.
     * Suppose the user decided that he want to receive the notification when a button is pressed each 500 ms.
     * He would want that if he had a game that requires moving a object while a button is pressed.
     * Each 500 ms he receives notification he will move the object with 1 pixel.
     * Changing the notification delay will increase/decrease the smoothness of the movement.
     */
    bool isHoldNotificationTimePassed();

    /**
     * When a button is activated we do not know if there was a single fast click or it is kept hold.
     * We must define separation between these two phases.
     * The @param holdThresholdMS defines after what activation/pressing time the button is in hold phase.
     */
    bool isEnteredHold();

    /**
     * A reading can be in 3 different states.
     * PRESSED_READING: We detected that there is a press of the provided button.
     * INTERMEDIATE_READING: There was no release of the button since the last reading. The button is still kept pressed.
     * RELEASED_READING: The button has been released.
     *
     * If the state can't be determined, then state UNKNOWN_READING: 3 is returned.
     */
    uint8_t getReadingState();

    void processPressedReading();

    void processIntermediateReading();

    void processReleasedReading();
};

#endif //ITSGOSHO_BUTTON_ENHANCED_H
