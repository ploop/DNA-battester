#ifndef NUVOTONMOD_H
#define NUVOTONMOD_H

#include "custommod.h"
#include "lib/hidapi.h"

#define NUVOTON_VID  0x0416
#define NUVOTON_PID  0x5020

class NuvotonMod : public CustomMod
{
public:

  explicit NuvotonMod(QObject *parent = 0);
  ~NuvotonMod();

  bool devConnect();

  deviceInfo getDeviceInfo();

private:
  deviceInfo d;

};

#endif // NUVOTONMOD_H
