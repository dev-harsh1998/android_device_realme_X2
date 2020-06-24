#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, device/realme/x2/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_x2
PRODUCT_DEVICE := x2
PRODUCT_BRAND := realme
PRODUCT_MODEL := realme x2
PRODUCT_MANUFACTURER := realme

BUILD_FINGERPRINT := "realme/RMX1992/RMX1992L1:10/QKQ1.191201.002/1589968374:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-oppo
