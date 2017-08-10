#ifndef DNAMOD_H
#define DNAMOD_H

#include "custommod.h"
#include <QSerialPort>
#include <QSerialPortInfo>


#define FIND_STRING       "Evolv DNA"    // Текст поиска девайса по строке описания
#define COMMAND_TIMEOUT   100            // Таймаут ожидания команд
// Команды
#define CMD_BAT_CNT       "B=GET CELLS"        // Кол-во батарей
#define CMD_USB_VOLTAGE   "U=GETV"             // Напряжение USB
#define CMD_USB_CURRENT   "U=GETI"             // Ток USB
#define CMD_CURRENT_POWER "P=GET"              // Текущая мощность
#define CMD_GET_CELL_1    "B=GET CELL 1"       // Напряжение 1 батареи
#define CMD_GET_CELL_2    "B=GET CELL 2"       // Напряжение 2 батареи
#define CMD_GET_CELL_3    "B=GET CELL 3"       // Напряжение 3 батареи
#define CMD_GET_BATT      "B=GET"              // Общее напряжение на батареях
#define CMD_GET_BORD      "T=GET BOARD"        // Температура платы


class DnaMod : public CustomMod
{
public:
  DnaMod(QObject *parent = 0);
  ~DnaMod();

  bool devConnect();
  bool devDisconnect();
  bool devIsConnected();
  deviceInfo getDeviceInfo();

public slots:

private:
  QSerialPort port;
  deviceInfo d;
  QString formatting(QString in);

  QString commandRead(QString text);
  bool commandSet(QString text);

  double getBat1();
  double getBat2();
  double getBat3();
  double getBatAll();
  double getPower();
  double getUsbI();
  double getUsbU();
  double getBoardTemp();
  int batCount();

  void sendFire(int ms);
  void sendStopFire();
  void sendUsbCharge(int mA);
  void sendPower(double power);
};

#endif // DNAMOD_H
