#include "nuvotonmod.h"

NuvotonMod::NuvotonMod(QObject *parent )
{
  this->setParent(parent);
}

NuvotonMod::~NuvotonMod()
{
  //
}

bool NuvotonMod::devConnect()
{
  hid_device_info *devs, *cur_dev;

  d.portName.clear();

  hid_init();
  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs;
  while (cur_dev)
    {
      if (cur_dev->vendor_id == NUVOTON_VID &&
          cur_dev->product_id == NUVOTON_PID)
        {
          d.description = QString().fromStdWString(cur_dev->product_string);
          d.serialNumber = QString().fromStdWString(cur_dev->serial_number);
          d.portName = "HID";
          d.manufacturer = QString().fromStdWString(cur_dev->manufacturer_string);
          break;
        }
      cur_dev = cur_dev->next;
    }
  hid_free_enumeration(devs);

  return d.portName == "HID";


}

deviceInfo NuvotonMod::getDeviceInfo()
{
  return d;
}

