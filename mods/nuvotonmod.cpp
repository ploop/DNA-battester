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

  if (hid_init())
    return false;

  hid_device_info *devs, *cur_dev;
  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs;
  d.portName.clear();

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

  if (d.portName != "HID")
    return false;

  handle = hid_open(NUVOTON_VID, NUVOTON_PID, NULL);
  if (!handle)
    {
      qDebug() << "unable to open device";
    return false;
    }

  hid_set_nonblocking(handle, 1);

  //hid_exit();
  return true;

}

deviceInfo NuvotonMod::getDeviceInfo()
{
  return d;
}

MonitoringData NuvotonMod::getMonitorData()
{
  QByteArray command;
  QDataStream commandStream(&command, QIODevice::WriteOnly);
  commandStream.setByteOrder(QDataStream::LittleEndian);
  commandStream
         << quint8(0x66) // Команда мониторинга
         << quint8(14)   // хз что
         << quint32(0)   // arg1
         << quint32(BLOCK_SIZE)  // arg2
         << quint8('H')
         << quint8('I')
         << quint8('D')
         << quint8('C');
  // Контрольная сумма
  quint32 cs = 0;
  for (int i = 0; i < command.size(); i++)
    {
      cs += command.at(i);
    }
  // Записываем сумму, остальное нулями
  commandStream << quint32(cs)
         << quint64(0) << quint64(0) << quint64(0)
         << quint64(0) << quint64(0) << quint32(0)
         << quint8(0)  << quint8(0) ;

  // Пишем в девайс
  hid_write(handle, (quint8*)command.data(), command.size());

  quint8 buf[BLOCK_SIZE];
  memset(buf, 0, sizeof(buf));

  // Читаем ответ
  while (hid_read(handle, buf, BLOCK_SIZE) > 0);

  QByteArray result;
  QDataStream outStream(&result, QIODevice::ReadWrite);

  for (int i = 0; i < BLOCK_SIZE; i++)
    outStream << buf[i];

  MonitoringData *d;
  d = (MonitoringData *)&buf;

  qDebug() << "Bat1_voltage" << d->bat1Voltage + 275;
  qDebug() << "Bat2_voltage" << d->bat2Voltage + 275;
  qDebug() << "Bat3_voltage" << d->bat3Voltage + 275;
  qDebug() << "Bat4_voltage" << d->bat4Voltage + 275;

  return *d;
}

double NuvotonMod::getBat1()
{
  MonitoringData d = getMonitorData();
  return double((d.bat1Voltage + 275)) / double(100);
}

double NuvotonMod::getBat2()
{
  MonitoringData d = getMonitorData();
  return double((d.bat2Voltage + 275)) / double(100);
}

double NuvotonMod::getBat3()
{
  MonitoringData d = getMonitorData();
  return double((d.bat3Voltage + 275)) / double(100);
}

int NuvotonMod::batCount()
{
  // TODO Сделать
  return 2;
}

bool NuvotonMod::devIsConnected()
{
  // TODO Сделать
  return true;
}

