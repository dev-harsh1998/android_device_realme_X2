#
# Copyright (C) 2020 Android Open Source Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, device/realme/realme_trinket/device.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := aosp_realme_trinket
PRODUCT_DEVICE := realme_trinket
PRODUCT_BRAND := realme
PRODUCT_MODEL := realme_trinket
PRODUCT_MANUFACTURER := realme

BUILD_FINGERPRINT := "trinket-user-10-QKQ1.200209.002--release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-realme
