#!/sbin/sh
RIRU_MODULE_ID="%%%RIRU_MODULE_ID%%%"

# Variables for customize.sh
RIRU_API=0
RIRU_MIN_COMPATIBLE_API=0
RIRU_VERSION_CODE=0
RIRU_VERSION_NAME=""

# Used by /data/adb/riru/util_functions.sh
RIRU_MODULE_API_VERSION=%%%RIRU_MODULE_API_VERSION%%%
RIRU_MODULE_MIN_API_VERSION=%%%RIRU_MODULE_MIN_API_VERSION%%%
RIRU_MODULE_MIN_RIRU_VERSION_NAME="%%%RIRU_MODULE_MIN_RIRU_VERSION_NAME%%%"

check_riru_version() {
  RIRU_CORE_MODULES_PATH=/data/adb/modules/riru-core
  RIRU_CORE_MODULES_UPDATE_PATH=/data/adb/modules_update/riru-core
  if [ ! -f "$RIRU_CORE_MODULES_UPDATE_PATH/api_version" ] && [ ! -f "$RIRU_CORE_MODULES_PATH/api_version" ] && [ ! -f "/data/adb/riru/api_version" ] && [ ! -f "/data/adb/riru/api_version.new" ]; then
    ui_print "*********************************************************"
    ui_print "! Riru $RIRU_MODULE_MIN_RIRU_VERSION_NAME or above is required"
    ui_print "! Please install Riru from Magisk Manager or https://github.com/RikkaApps/Riru/releases"
    abort "*********************************************************"
  fi
  RIRU_API=$(cat "$RIRU_CORE_MODULES_UPDATE_PATH/api_version") || RIRU_API=$(cat "$RIRU_CORE_MODULES_PATH/api_version") || RIRU_API=$(cat "/data/adb/riru/api_version.new") || RIRU_API=$(cat "/data/adb/riru/api_version") || RIRU_API=0
  [ "$RIRU_API" -eq "$RIRU_API" ] || RIRU_API=0
  ui_print "- Riru API version: $RIRU_API"
  if [ "$RIRU_API" -lt $RIRU_MODULE_MIN_API_VERSION ]; then
    ui_print "*********************************************************"
    ui_print "! Riru $RIRU_MODULE_MIN_RIRU_VERSION_NAME or above is required"
    ui_print "! Please upgrade Riru from Magisk Manager or https://github.com/RikkaApps/Riru/releases"
    abort "*********************************************************"
  fi
}

if [ -f /data/adb/modules_update/riru-core/util_functions.sh ]; then
  ui_print "- Load /data/adb/modules_update/riru-core/util_functions.sh"
  . /data/adb/modules_update/riru-core/util_functions.sh
else
  if [ -f /data/adb/modules/riru-core/util_functions.sh ]; then
    ui_print "- Load /data/adb/modules/riru-core/util_functions.sh"
    . /data/adb/modules/riru-core/util_functions.sh
  else
    if [ -f /data/adb/riru/util_functions.sh ]; then
      ui_print "- Load /data/adb/riru/util_functions.sh"
      . /data/adb/riru/util_functions.sh
    else
      ui_print "- Can't find /data/adb/riru/util_functions.sh"
    fi
  fi
fi

check_riru_version