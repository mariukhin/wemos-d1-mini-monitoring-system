extern "C" {
  #include <user_interface.h>
}
#include "LightSleepModule.h"

static const uint32_t SLEEP_CHUNK = 268000;

static uint32_t remains;

static void wakeUp() {
  if (remains <= SLEEP_CHUNK) {
    wifi_fpm_close();
  }
}

void sleep(uint32_t ms) {
  uint8_t optmode;

  wifi_station_disconnect();
  optmode = wifi_get_opmode();
  if (optmode != NULL_MODE)
    wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(wakeUp);
  remains = ms;
  while (remains > 0) {
    if (remains > SLEEP_CHUNK)
      ms = SLEEP_CHUNK;
    else
      ms = remains;
    wifi_fpm_do_sleep(ms * 1000);
    delay(ms);
    remains -= ms;
  }
  if (optmode != NULL_MODE)
    wifi_set_opmode_current(optmode);
}