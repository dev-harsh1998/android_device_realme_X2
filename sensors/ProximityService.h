/* 
 * Copyright (c) Harshit Jain <god@hyper-labs.tech>
 * Copyright (c) Art_chen (Base Idea and refrence)
 * Dual Proximity Handler
 */
#ifndef PROXIMITYSERVICE_H_
#define PROXIMITYSERVICE_H_
#include <fstream>
#include <android-base/logging.h>
#define PANEL_INCALL "/proc/touchpanel/INCALL_SUSPEND"
bool get_fake_prox_event();
#endif //PROXIMITYSERVICE_H_