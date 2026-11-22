#include "settings.h"
#include <Preferences.h>

Settings cfg;

void settingsLoad() {
  Preferences p;
  p.begin("cfg", true);
  if (p.getBytesLength("s") == sizeof(Settings)) p.getBytes("s", &cfg, sizeof(Settings));
  p.end();
}

void settingsSave() {
  Preferences p;
  p.begin("cfg", false);
  p.putBytes("s", &cfg, sizeof(Settings));
  p.end();
}
