/*
 *  Базовый класс, общий для всех типов модов.
 *
 */

#ifndef CUSTOMMOD_H
#define CUSTOMMOD_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "qcustomplot.h"


#define GR_USB_CURRENT_MAX   2.1    // Максимальный ток USB на графике
#define GR_USB_VOLTAGE_MAX   5.5    // Максимальное напряжение USB на графике
#define GR_USB_VOLTAGE_MIN   4.0    // Минимальное напряжение USB на графике
#define GR_BAT_VOLTAGE_MAX   4.3    // Максимальное напряжение на батарее
#define GR_BAT_VOLTAGE_MIN   2.7    // Минимальное напряжение на батарее на графике
#define RELAX_TIMER_INTERVAL 5000   // Время после остановки анализа до сохранения результата
#define DEFAULT_USB_CHARGE   2000   // Ток заряда USB по умолчанию

#define BOARD_TEMP_LIMIT_1   70     // Лимит по температуре платы, утраиваем интервал
#define BOARD_TEMP_LIMIT_2   75     // Лимит по температуре платы, 10 интервалов




struct curInfo
{
  // Текущие значения
  double usb_i;
  double usb_u;
  double power;
  double bat1;
  double bat2;
  double bat3;
  double bat_min;
  double bat_all;
  double energy;
  double last_energy;
  double dev_temp;
};

struct outCurve
{
  double voltage;
  double energy;
  double percent;
};

struct deviceInfo
{
  QString description;
  QString manufacturer;
  QString portName;
  QString serialNumber;
};

bool lessThan(const outCurve &d1, const outCurve &d2);

class CustomMod : public QObject
{
  Q_OBJECT
public:
  explicit CustomMod(QObject *parent = 0);
  ~CustomMod();

  void setUSBPlotI(QCustomPlot *p);
  void setUSBPlotU(QCustomPlot *p);
  void setBatPlot(QCustomPlot *p);
  void setPowerPlot(QCustomPlot *p);

  void setPlotTimeline(int tl);
  void setPlotPowerMax(double powerMax);

  void plotInit();
  void stopTimers();
  void stopAnalyze();
  void startGenTimer(int interval);
  void startAnalyze(int fireTime = 5000,
                    int interval = 10000,
                    double power = 50,
                    double startVoltage = -1,
                    double stopVoltage = 2.8);
  curInfo *getCurInfo();
  QVector<outCurve> *getOutCurve();

  virtual bool devConnect();
  virtual bool devDisconnect();
  virtual bool devIsConnected();
  virtual deviceInfo getDeviceInfo();

protected:
  QTimer *generalTimer;
  QTimer *fireTimer;

private:
  void plotUpdate();
  void curUpdate();

  QTimer *relaxTimer;

  // Графики
  QCustomPlot *USBPlotI;
  QCustomPlot *USBPlotU;
  QCustomPlot *batPlot;
  QCustomPlot *powerPlot;

  // Массивы для графиков
  QVector<double> x_axis;
  QVector<double> y_usb_i;
  QVector<double> y_usb_u;
  QVector<double> y_power;
  QVector<double> y_bat_1;
  QVector<double> y_bat_2;
  QVector<double> y_bat_3;

  // Настройки графиков
  int timeLine;
  double plotPowerMax;

  // Настройки анализа
  double startVolt;
  double stopVolt;
  int fireTime;
  int period;


  // Текущие значения
  curInfo info;
  // Выходной график
  QVector<outCurve> curve;


  //---------------------------------
  // Количество батарей
  virtual int batCount();

  // Текущие значения
  virtual double getUsbI();
  virtual double getUsbU();
  virtual double getPower();
  virtual double getBat1();
  virtual double getBat2();
  virtual double getBat3();
  virtual double getBatAll();
  virtual double getBoardTemp();

  // Управляющие команды
  virtual void sendFire(int ms);
  virtual void sendStopFire();
  virtual void sendUsbCharge(int mA);
  virtual void sendPower(double p);


signals:
  void sigGeneralTimer();
  void sigStopAnalyze(bool resultOk);
  void sigBoardHot(int interval);
  void sigBoardOk();

public slots:

private slots:
  void slotGeneralTimer();
  void slotRelaxTimer();
  void slotFireTimer();

};


// Форматы сохранения графиков
class saveFormatDNA
{
public:
  saveFormatDNA(QVector<outCurve> *c, curInfo *i);
  ~saveFormatDNA();

  bool saveToFile(QString fileName);
private:
  QVector<outCurve> curve;
  curInfo info;


};


#endif // CUSTOMMOD_H
