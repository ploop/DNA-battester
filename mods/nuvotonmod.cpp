#include "nuvotonmod.h"

NuvotonMod::NuvotonMod(QObject *parent )
{
  this->setParent(parent);
  memset(&data, 0, sizeof(data));
  batCnt = 1;
}

NuvotonMod::~NuvotonMod()
{
  hid_exit();
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

  getMonitorData();
  getMonitorData();

  getBatCnt();
  return true;

}

deviceInfo NuvotonMod::getDeviceInfo()
{
  return d;
}

void NuvotonMod::getMonitorData()
{
  QByteArray command = createCommand(CMD_MONITORING, 0, BLOCK_SIZE);

  // Пишем в девайс
  hid_write(handle, (quint8*)command.data(), command.size());

  quint8 buf[BLOCK_SIZE];
  memset(buf, 0, sizeof(buf));

  // Читаем в цикле, вычитываем всё
  while (hid_read(handle, buf, BLOCK_SIZE) > 0);

  // Если ничего не прочитано, выходим не обновляя прежнюю структуру
  int sum = 0;
  for (quint16 i = 0; i < sizeof(buf); i++)
    sum += buf[i];
  if (sum == 0)
    return;

  // Накладываем пакованную структуру на массив байт
  MonitoringData *d;
  d = (MonitoringData *)&buf;
  // И забираем содержимое
  data = *d;
}

void NuvotonMod::getBatCnt()
{
  if (data.bat2Voltage == 0)
    batCnt = 1;
  else if (data.bat3Voltage == 0)
    batCnt = 2;
  else batCnt = 3;
}

double NuvotonMod::getBat1()
{
  getMonitorData();
  return voltageConvert(data.bat1Voltage);
}

double NuvotonMod::getBat2()
{
  getMonitorData();
  return voltageConvert(data.bat2Voltage);
}

double NuvotonMod::getBat3()
{
  getMonitorData();
  return voltageConvert(data.bat3Voltage);
}

double NuvotonMod::getBatAll()
{
  getMonitorData();

  return voltageConvert(data.bat3Voltage) +
      voltageConvert(data.bat2Voltage) +
      voltageConvert(data.bat1Voltage);
}

double NuvotonMod::getPower()
{
  getMonitorData();
  return (double(data.outVoltage) / double(100)) *
      (double(data.outCurrent) / double(100));
}

double NuvotonMod::getBoardTemp()
{
  return data.boardTemp;
}

int NuvotonMod::batCount()
{
  return batCnt;
}

void NuvotonMod::sendFire(int ms)
{
  // Команда на жарку N секунд
  QByteArray command = createCommand(CMD_FIRE, ms / 1000, 0);
  hid_write(handle, (quint8*)command.data(), command.size());
}

void NuvotonMod::sendStopFire()
{
  sendFire(0);
}

double NuvotonMod::voltageConvert(int val)
{
  if (val == 0)
    return 0;
  return double(val + 275) / double(100);
}

QByteArray NuvotonMod::createCommand(quint8 command, quint32 arg1, quint32 arg2)
{
  // Создание блока с командой для отправки в девайс
  QByteArray commandArray;
  QDataStream commandStream(&commandArray, QIODevice::WriteOnly);
  commandStream.setByteOrder(QDataStream::LittleEndian);
  commandStream
                 << quint8(command) // Команда
                 << quint8(14)      // хз для чего, число 14
                 << quint32(arg1)   // Два аргумента
                 << quint32(arg2)
                 << quint8('H')     // Сигнатура "HIDC"
                 << quint8('I')
                 << quint8('D')
                 << quint8('C');

  // Контрольная сумма, просто сумма всех байт
  quint32 cs = 0;
  for (int i = 0; i < commandArray.size(); i++)
      cs += commandArray.at(i);
  commandStream << quint32(cs);

  // Перебросим структуру в блок, заполненный нулями
  QByteArray outArray;
  outArray.fill(0, BLOCK_SIZE);
  outArray.replace(0, commandArray.size(), commandArray);

  return outArray;
}

bool NuvotonMod::devIsConnected()
{
  // Для проверки соединения будем читать serial number
  wchar_t buf[16];
  int cnt = hid_get_serial_number_string(handle, buf, 16);
  return cnt == 0;
}

