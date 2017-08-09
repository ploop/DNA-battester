#ifndef NUVOTONMOD_H
#define NUVOTONMOD_H

#include "custommod.h"
#include "lib/hidapi.h"

#define NUVOTON_VID  0x0416
#define NUVOTON_PID  0x5020

#define BLOCK_SIZE   64

struct MonitoringData
{
  quint32 timeStamp;
  bool isFiring;
  bool isCharging;
  bool isCelcius;

  // Offsetted by 275, 420 - 275 = value
  quint8 bat1Voltage;
  quint8 bat2Voltage;
  quint8 bat3Voltage;
  quint8 bat4Voltage;

  // X * 10
  quint16 powerSet;
  quint16 tempSet;
  quint16 temp;

  // X * 100
  quint16 outVoltage;
  quint16 outCurrent;

  // X * 1000
  quint16 resSet;
  quint16 resREal;

  quint8 boardTemp;
} __attribute__ ((__packed__));

class NuvotonMod : public CustomMod
{
public:

  explicit NuvotonMod(QObject *parent = 0);
  ~NuvotonMod();

  bool devConnect();

  deviceInfo getDeviceInfo();

private:

  MonitoringData getMonitorData();


  deviceInfo d;
  hid_device *handle;
  MonitoringData data;

  double getBat1();
  double getBat2();
  double getBat3();

  int batCount();

  bool devIsConnected();


};

#endif // NUVOTONMOD_H
