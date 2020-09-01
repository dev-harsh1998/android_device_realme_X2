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
PRODUCT_BRAND := google
PRODUCT_MODEL := Pixel 4 XL
PRODUCT_MANUFACTURER := Google

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="coral-user 10 QQ3A.200805.001 6578210 release-keys"

BUILD_FINGERPRINT := "google/coral/coral:10/QQ3A.200805.001/6578210:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-oppo
