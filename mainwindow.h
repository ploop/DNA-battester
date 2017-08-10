#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>

#include "mods/nuvotonmod.h"
#include "mods/dnamod.h"


#define FIND_TIMER           1000   // Таймер обнаружения девайса

#define GR_USB_CURRENT_MAX   2.1    // Максимальный ток USB на графике
#define GR_USB_VOLTAGE_MAX   5.5    // Максимальное напряжение USB на графике
#define GR_BAT_VOLTAGE_MAX   4.3    // Максимальное напряжение на батарее
#define GR_BAT_VOLTAGE_MIN   2.7    // Минимальное напряжение на батарее на графике



namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  CustomMod *mod;
  CustomMod *mod1;
  CustomMod *mod2;

  QTimer *findTimer;

  // Количество батарей
  int batCnt;
  // Старт/пауза графиков
  bool graphRun;

  // Массивы для графика USB
  QVector<double> grUSB_x;
  QVector<double> grUSB_I_y;
  QVector<double> grUSB_U_y;

  // Массивы для напряжения и мощности
  QVector<double> grPower;
  QVector<double> grBat1;
  QVector<double> grBat2;
  QVector<double> grBat3;

  void modInit();

  void updateInfo();
  void clearInfo();

private slots:
  void slotFindTimer();
  void slotBtnApplyGraphSettings();
  void slotGraphRun();
  void slotBtnStart();
  void slotBtnStop();
  void slotBtnSave();

  void slotStopAnalyze();
  void slotGeneralTimer();

  void slotHot(int period);
  void slotHotOk();

  // TODO delete this
  void slotPb();



};


#endif // MAINWINDOW_H
