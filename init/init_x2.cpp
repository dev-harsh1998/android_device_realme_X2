/*
 * Copyright (C) 2020 AOSP Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "log.h"
#include <string>
#include <fstream>

using namespace std;

namespace android {
namespace init {

bool isCN(){
  // Get region
  std::ifstream infile("/proc/oppoVersion/region"); std::string region;
  bool ret = false;
  getline(infile, region);
  if(!region.compare("China"))
      ret = true; 
  return ret;
}

void vendor_load_properties() {
  property_set("ro.product.device", isCN()   ?  "RMX1991CN"   :   "RMX1992");
  property_set("ro.product.name",   isCN()   ?  "RMX1991"     :   "RMX1992");
  property_set("ro.build.product",  isCN()   ?  "RMX1991"     :   "RMX1992"); 
}
}  // namespace init
}  // namespace android
