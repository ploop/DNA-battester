#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>


#define FIND_TIMER           1000   // Таймер обнаружения девайса

#define GR_USB_CURRENT_MAX   2.1    // Максимальный ток USB на графике
#define GR_USB_VOLTAGE_MAX   5.5    // Максимальное напряжение USB на графике
#define GR_BAT_VOLTAGE_MAX   4.3    // Максимальное напряжение на батарее
#define GR_BAT_VOLTAGE_MIN   2.7    // Минимальное напряжение на батарее на графике

#define BAT_VOLTAGE_STOP     2.8    // Напряжение безусловной остановки анализа

// Текст поиска девайса по строке описания
#define FIND_STRING       "Evolv DNA"
// Команды
#define CMD_BAT_CNT       "B=GET CELLS"  // Кол-во батарей
#define CMD_USB_VOLTAGE   "U=GETV"       // Напряжение USB
#define CMD_USB_CURRENT   "U=GETI"       // Ток USB
#define CMD_CURRENT_POWER "P=GET"        // Текущая мощность
#define CMD_GET_CELL_1    "B=GET CELL 1" // Напряжение 1 батареи
#define CMD_GET_CELL_2    "B=GET CELL 2" // Напряжение 2 батареи
#define CMD_GET_CELL_3    "B=GET CELL 3" // Напряжение 3 батареи
#define CMD_GET_BATT      "B=GET"        // Общее напряжение на батареях


struct volPoint
{
  double voltage;
  double energy;
  double reverse_energy;
  double percrnt;
};


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

  QSerialPortInfo dnaSerialInfo;
  QSerialPort port;
  QTimer *timer;
  QTimer *findTimer;
  QTimer *fireTimer;
  QTimer *relaxTimer;

  // Количество батарей
  int batCnt;
  // Старт/пауза графиков
  bool graphRun;

  // Массивы для графика USB
  QVector<double> grUSB_x;
  QVector<double> grUSB_I_y;
  QVector<double> grUSB_U_y;
  QVector<volPoint> volPoints;

  // Массивы для напряжения и мощности
  QVector<double> grPower;
  QVector<double> grBat1;
  QVector<double> grBat2;
  QVector<double> grBat3;

  // Текущие значения
  double curUsbI;
  double curUsbU;
  double curPower;
  double curBat1;
  double curBat2;
  double curBat3;
  double curBatMin;
  double curBatAll;
  double curEnergy;
  double lastEnergy;



  bool serialInit();
  void graphInit();
  void graphUpdate();
  int getBatCnt();
  void updateInfo();
  void clearInfo();
  bool deviceConnect();
  bool deviceDisconnect();
  void getCur();
  void stopAnalyze();

  float m_getVoltage();
  QString sendConnand(QString text);


private slots:
  void slotTimer();
  void slotFindTimer();
  void slotBtnApplyGraphSettings();
  void slotGraphRun();
  void slotBtnStart();
  void slotBtnStop();
  void slotFireTimer();
  void slotRelaxTimer();

  void slotPb();



};


#endif // MAINWINDOW_H
