#include "dnamod.h"

DnaMod::DnaMod(QObject *parent)
{
  this->setParent(parent);
}

DnaMod::~DnaMod()
{
}

bool DnaMod::devConnect()
{
  if (port.isOpen())
      return true;

  QSerialPortInfo i;
  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
      if (info.description().contains(FIND_STRING) && !info.isBusy())
        {
          i = info;
        };
    }

  if (i.isNull())
    return false;

  port.setPort(i);

  // Настраиваем порт
  port.setBaudRate(QSerialPort::Baud115200);
  port.setDataBits(QSerialPort::Data8);
  port.setParity(QSerialPort::NoParity);
  port.setStopBits(QSerialPort::OneStop);
  port.setFlowControl(QSerialPort::NoFlowControl);

  d.description = i.description();
  d.manufacturer = i.manufacturer();
  d.portName = i.portName();
  d.serialNumber = i.serialNumber();

  if (!port.open(QIODevice::ReadWrite))
    {
      qDebug() << "Port open error:" << port.errorString();
      return false;
    };

  QString tmp = commandRead(CMD_BAT_CNT);
  tmp = formatting(tmp);
  int cnt = tmp.toInt();
  if (cnt < 1)
    cnt = 1;
  batCnt = cnt;

  return true;
}

bool DnaMod::devDisconnect()
{
  port.close();

  if (fireTimer->isActive())
    stopAnalyze();
  generalTimer->stop();
  return true;
}

bool DnaMod::devIsConnected()
{
  return port.isOpen();
}

deviceInfo DnaMod::getDeviceInfo()
{
  return d;
}


QString DnaMod::formatting(QString in)
{
  // Форматирует выходные данные
  QString tmp = in.trimmed();
  tmp.replace("U=","",Qt::CaseInsensitive);
  tmp.replace("A","",Qt::CaseInsensitive);
  tmp.replace("V","",Qt::CaseInsensitive);
  tmp.replace("P=","",Qt::CaseInsensitive);
  tmp.replace("W","",Qt::CaseInsensitive);
  tmp.replace("B=","",Qt::CaseInsensitive);
  tmp.replace("T=","",Qt::CaseInsensitive);
  tmp.replace("F","",Qt::CaseInsensitive);

  return tmp;
}

QString DnaMod::commandRead(QString text)
{
  if (!port.isOpen())
    return "";

  QString str = text + "\n";
  port.write(str.toLocal8Bit());
  if (!port.waitForBytesWritten(COMMAND_TIMEOUT))
    {
      devDisconnect();
      return "";
    }

  if (!port.waitForReadyRead(COMMAND_TIMEOUT))
    {
      return "";
    }

  QByteArray ba;
  ba = port.readAll();
  str = QString().fromLocal8Bit(ba.data());
  return str.trimmed();
}

bool DnaMod::commandSet(QString text)
{
  if (!port.isOpen())
    return false;
  QString str = text + "\n";
  port.write(str.toLocal8Bit());
  if (!port.waitForBytesWritten(COMMAND_TIMEOUT))
    {
      devDisconnect();
      return false;
    }
  return true;
}

//-------------------------------
double DnaMod::getBat1()
{
  QString tmp = commandRead(CMD_GET_CELL_1);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getBat2()
{
  QString tmp = commandRead(CMD_GET_CELL_2);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getBat3()
{
  QString tmp = commandRead(CMD_GET_CELL_3);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getBatAll()
{
  QString tmp = commandRead(CMD_GET_BATT);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getPower()
{
  QString tmp = commandRead(CMD_CURRENT_POWER);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getUsbI()
{
  QString tmp = commandRead(CMD_USB_CURRENT);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getUsbU()
{
  QString tmp = commandRead(CMD_USB_VOLTAGE);
  tmp = formatting(tmp);
  return tmp.toDouble();
}

double DnaMod::getBoardTemp()
{
  QString tmp = commandRead(CMD_GET_BORD);
  tmp = formatting(tmp);

  return (tmp.toDouble() - 32) * (double(5) / double(9));
}

int DnaMod::batCount()
{
  return batCnt;
}

void DnaMod::sendFire(int ms)
{
  QString cmd = QString("F=%1S").arg(ms/1000);
  commandSet(cmd);
}

void DnaMod::sendStopFire()
{
  sendFire(50);
}

void DnaMod::sendUsbCharge(int mA)
{
  QString cmd = QString("U=%1").arg(mA/1000);
  commandSet(cmd);
}

void DnaMod::sendPower(double power)
{
  QString cmd = QString("P=%1W").arg(power);
  commandSet(cmd);
}











