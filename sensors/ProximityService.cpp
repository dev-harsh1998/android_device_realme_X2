/* 
 * Copyright (c) Harshit Jain <god@hyper-labs.tech>
 * Copyright (c) Art_chen (Base Idea and refrence)
 * Dual Proximity Handler
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <android/sensor.h>
#include "ProximityService.h"

#define SENSOR_TYPE     33171005
#define RETRY_LIMIT     25
#define RETRY_PERIOD    5          // 5 seconds
#define WARN_PERIOD     (time_t)10 // 5 minutes

bool is_proximity_far();
bool get_fake_prox_event()
{
    return is_proximity_far();
}

bool is_proximity_far()
{
    ASensorManager *sensorManager = nullptr;
    ASensorRef ProxSensor;
    ALooper *looper;
    ASensorEventQueue *eventQueue = nullptr;
    int32_t ProxMinDelay = 0;
    time_t lastWarn = 0;
    int attemptCount = 0;
    bool is_far = false;
    int err = 0;
    int tries = 10;
    int evcnt = 0;

    /* Prepare stuffs */
    sensorManager = ASensorManager_getInstanceForPackage(nullptr);
    looper = ALooper_forThread();
    if (looper == nullptr)
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    eventQueue = ASensorManager_createEventQueue(sensorManager, looper, 0, NULL, NULL);

    /* Get sensor event */
    while (true) {
        time_t now = time(NULL);
        ProxSensor = ASensorManager_getDefaultSensor(sensorManager,
                                                     SENSOR_TYPE);
        if (ProxSensor != nullptr) {
            ProxMinDelay = ASensor_getMinDelay(ProxSensor);
            break;
        }
        if (++attemptCount >= RETRY_LIMIT) {
            goto out;
        } else if (now > lastWarn + WARN_PERIOD) {
            lastWarn = now;
            LOG(INFO) << "QTIPROX  delay still = " << ProxMinDelay;
        }
        sleep(RETRY_PERIOD);
    }

    /* Real deal starts here */
    ProxMinDelay = (ProxMinDelay > 0) ? ProxMinDelay : 200000; //Sensor.DELAY_NORMAL
    LOG(INFO) << "HOLY SHIT WE HAVE QTIPROX SENSOR" << ProxMinDelay;
    err = ASensorEventQueue_registerSensor(eventQueue, ProxSensor,
                                      ProxMinDelay, 10000);
    if (err < 0) {
        LOG(INFO) << "QTIPROX: RIP";
        goto out;
    }

    while (ALooper_pollAll(-1, NULL, NULL, NULL) == 0) {
        ASensorEvent sensorEvent;
        LOG(INFO) << "QTIPROX : PISIDETHE POLLER";
        while (ASensorEventQueue_getEvents(eventQueue, &sensorEvent, 1) > 0) {
            LOG(INFO) << "QTIPROX : GETTING EVENTS";
            is_far = sensorEvent.data[0] > 0.0f ? true : false;
            if(is_far) goto out;
            evcnt++;
            --tries;
            if(!tries) goto out;
        }
        if(!evcnt) goto out;
    }

out:
    if (sensorManager != nullptr && eventQueue != nullptr) {
        LOG(INFO) << "QTIPROX : CLEANUP";
        ASensorManager_destroyEventQueue(sensorManager, eventQueue);
    }
    LOG(INFO) << "QTIPROX : FAR 1 NEAR 0: ?" << is_far;
    return is_far;
}