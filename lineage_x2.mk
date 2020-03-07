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


PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="sm6150-user 9 PKQ1.190630.001 eng.root.20200118.175901 release-keys"

BUILD_FINGERPRINT := "realme/RMX1992/RMX1992L1:9/PKQ1.190630.001/1578551002:user/release-keys"

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME="RMX1992" \
    PRODUCT_DEVICE="RMX1992" \
    PRODUCT_MODEL="RMX1992"

PRODUCT_GMS_CLIENTID_BASE := android-realme