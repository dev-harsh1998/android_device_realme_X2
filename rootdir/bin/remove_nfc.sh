#!/sbin/sh
#
# Copyright (c) 2020 Harshit Jain
#
# NFC script, partially based on X00TD
#

#Find get device variant
whichRmx=$(getprop "ro.product.name")

# Indian Variant does not support NFC
if [ "$whichRmx" = "RMX1992" ]; then
    echo "$whichRmx doesn't support NFC , removing ..."
    rm -rf /system_root/system/app/*Nfc*
    rm -rf /system_root/system/app/*nfc*
    rm -rf /system_root/system/etc/permissions/*nfc*
    rm -rf /system_root/system/framework/*nfc*
    rm -rf /system_root/system/lib/*nfc*
    rm -rf /system_root/system/lib64/*nfc*
    rm -rf /system_root/system/priv-app/Tag
    rm -rf /vendor/bin/*nfc*
    rm -rf /vendor/bin/hw/*nfc*
    rm -rf /vendor/etc/*nfc*
    rm -rf /vendor/etc/init/*nfc*
    rm -rf /vendor/etc/permissions/*nfc*
    rm -rf /vendor/lib/*nfc*
    rm -rf /vendor/lib/hw/*nfc*
    rm -rf /vendor/lib64/*nfc*
    rm -rf /vendor/lib64/hw/*nfc*
    rm -rf /vendor/bin/hw/*nfc*
    echo "Done removing nfc stuff from $whichRmx"
else
    echo "Cool!, $whichRmx supports NFC, Nothing changed"
fi