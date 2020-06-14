# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    ro.config.media_vol_steps=25 \
    ro.config.vc_call_vol_steps=7

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.bluetooth.soc=cherokee \
    ro.bluetooth.wipower=false \
    ro.bluetooth.emb_wp_mode=false

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.camera.light_ae.enable=1 \
    vendor.camera.aux.packagelist=org.codeaurora.snapcam,com.android.camera,org.lineageos.snap

# CNE
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.cne.feature=0 \
    persist.vendor.dpm.feature=0

# Data modules
PRODUCT_PROPERTY_OVERRIDES += \
    persist.data.netmgrd.qos.enable=true \
    persist.vendor.data.profile_update=true \
    persist.vendor.data.mode=concurrent \
    ro.vendor.use_data_netmgrd=true

# Display post-processing
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.display.ad=1 \
    ro.vendor.display.ad.sdr_calib_data=/vendor/etc/OPPO_OLED_AD_calib.cfg

# Fingerprint
PRODUCT_PROPERTY_OVERRIDES += \
    ro.boot.realme.lockstate=0

# Graphics
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sf.enable_hwc_vds=1 \
    dev.pm.dyn_samplingrate=1 \
    persist.front.skipdisplay.enable=1

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    media.settings.xml=/system/etc/media_profiles_vendor.xml \
    media.stagefright.thumbnail.prefer_hw_codecs=true \
    persist.mm.enable.prefetch=true \
    vendor.media.vpp.enable=true

# Memory optimizations
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.bservice_enable=true

# Netflix custom property
PRODUCT_PROPERTY_OVERRIDES += \
    ro.netflix.bsp_rev=Q6150-17263-1

# Perf
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.core_ctl_min_cpu=4 \
    ro.vendor.qti.core_ctl_max_cpu=6

# RCS and IMS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.dbg.ims_volte_enable=1 \
    persist.dbg.volte_avail_ovr=1 \
    persist.dbg.vt_avail_ovr=1 \
    persist.dbg.wfc_avail_ovr=1 \
    persist.rcs.supported=0 \
    persist.vendor.ims.disableQXDMLogs=0 \
    persist.vendor.ims.disableADBLogs=0 \
    persist.vendor.ims.disableDebugLogs=0 \
    persist.vendor.ims.disableIMSLogs=0 \
    persist.vendor.ims.rtp.enableqxdm=3 \
    persist.vendor.ims.vt.enableadb=3

# RIL
PRODUCT_PROPERTY_OVERRIDES += \
    DEVICE_PROVISIONED=1 \
    persist.sys.fflag.override.settings_network_and_internet_v2=true \
    persist.radio.hvolte.enable=1 \
    persist.vendor.radio.atfwd.start=true \
    persist.vendor.radio.lte_vrte_ltd=1 \
    persist.vendor.radio.cs_srv_type=1 \
    persist.vendor.radio.rat_on=combine \
    ril.subscription.types=NV,RUIM \
    persist.radio.add_power_save=1 \
    ro.telephony.default_network=22,20 \
    telephony.lteOnCdmaDevice=1

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.sensors.enable.mag_filter=true \
    ro.qti.sensors.wu=false

# SSR
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.ssr.enable_ramdumps=0 \
    persist.vendor.ssr.restart_level=ALL_ENABLE
