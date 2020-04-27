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

namespace android
{
namespace init
{

bool isCN()
{
  // Get region
  ifstream infile("/proc/oppoVersion/region");
  string region;
  bool ret = false;
  getline(infile, region);
  if (!region.compare("China"))
    ret = true;
  return ret;
}

bool hasNFC()
{
  // Check NFC
  ifstream infile("/proc/touchpanel/NFC_CHECK");
  string check;
  bool ret = false;
  getline(infile, check);
  if (!check.compare("SUPPORTED"))
    ret = true;
  return ret;
}

string setPrjverson()
{
  /*
   * TODO, Dynamically set this via "/proc/oppoVersion/prjVersion"
   * ifstream infile("/proc/oppoVersion/prjVersion");
   * string prjversion;
   * getline(infile, prjversion);
   * return prjversion;
   * The soc dts needs to be fixed for this to work properly so let's hold for now
   */
  string prjversion = "19771";
  if (!isCN() && hasNFC())
    prjversion = "19672";
  if (!isCN())
    prjversion = "19671";
  return prjversion;
}

void vendor_load_properties()
{
  property_set("ro.product.device", isCN() ? "RMX1991CN" : hasNFC() ? "RMX1993" : "RMX1992");
  property_set("ro.product.name",   isCN() ? "RMX1991"   : hasNFC() ? "RMX1993" : "RMX1992");
  property_set("ro.build.product",  isCN() ? "RMX1991"   : hasNFC() ? "RMX1993" : "RMX1992");
  property_set("ro.separate.soft", setPrjverson());
}
} // namespace init
} // namespace android
