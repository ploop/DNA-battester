#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  timer = new QTimer(this);
  findTimer = new QTimer(this);
  fireTimer = new QTimer(this);
  relaxTimer = new QTimer(this);

  connect(this->timer,SIGNAL(timeout()),this,SLOT(slotTimer()));
  connect(this->findTimer,SIGNAL(timeout()),this,SLOT(slotFindTimer()));
  connect(ui->btnApplyGraphSettings,SIGNAL(clicked(bool)),this,SLOT(slotBtnApplyGraphSettings()));
  connect(ui->btnGraphRun,SIGNAL(clicked(bool)),this,SLOT(slotGraphRun()));
  connect(ui->btnStart,SIGNAL(clicked(bool)),this,SLOT(slotBtnStart()));
  connect(ui->btnStop,SIGNAL(clicked(bool)),this,SLOT(slotBtnStop()));
  connect(this->fireTimer,SIGNAL(timeout()),this,SLOT(slotFireTimer()));
  connect(this->relaxTimer,SIGNAL(timeout()),this,SLOT(slotRelaxTimer()));
  connect(ui->btnSave,SIGNAL(clicked(bool)),this,SLOT(slotBtnSave()));

  // TODO delete this
  //connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(slotPb()));

  // При старте запуск поиска устройства
  findTimer->start(FIND_TIMER);

  batCnt = 1;
  curEnergy = 0;
  graphRun = true;

  graphInit();

}

MainWindow::~MainWindow()
{
  port.close();
  delete ui;
}

bool MainWindow::serialInit()
{
  // Находим DNA на портах
  port.close();

  QSerialPortInfo i;
  dnaSerialInfo = i;

  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
      if (info.description().contains(FIND_STRING) && !info.isBusy())
        {
          this->dnaSerialInfo = info;
        };
    }

  if (dnaSerialInfo.isNull())
      return false;

  port.setPort(dnaSerialInfo);

  // Настраиваем порт
  port.setBaudRate(QSerialPort::Baud115200);
  port.setDataBits(QSerialPort::Data8);
  port.setParity(QSerialPort::NoParity);
  port.setStopBits(QSerialPort::OneStop);
  port.setFlowControl(QSerialPort::NoFlowControl);

  return true;
}

void MainWindow::graphInit()
{
  // Очищаем настройки
  grUSB_x.clear();
  grUSB_I_y.clear();
  grUSB_U_y.clear();
  grPower.clear();
  grBat1.clear();
  grBat2.clear();
  grBat3.clear();

  ui->grBatVoltage->clearItems();
  ui->grUsbU->clearItems();
  ui->grUsbI->clearItems();
  ui->grPower->clearItems();



  qDebug() << "Graph init!!!";

  // Настройки с интерфейса
  int time_points = ui->spinTimeLine->value();
  int def_power = ui->spinDefPower->value();

  // ----------------------------------------
  // Настраиваем график USB тнапряжения
  ui->grUsbU->addGraph();

  // Цвета
  ui->grUsbU->graph(0)->setPen(QPen(Qt::blue)); // Напряжение синим

  ui->grUsbU->xAxis->setLabel(tr("Time"));
  ui->grUsbU->yAxis->setLabel(tr("USB Voltage"));
  ui->grUsbU->yAxis->setRange(0,GR_USB_VOLTAGE_MAX);
  ui->grUsbU->xAxis->setRange(0,time_points);
  ui->grUsbU->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // ----------------------------------------
  // Настраиваем график USB тока
  ui->grUsbI->addGraph();

  // Цвета
  ui->grUsbI->graph(0)->setPen(QPen(Qt::red)); // Ток красным

  ui->grUsbI->xAxis->setLabel(tr("Time"));
  ui->grUsbI->yAxis->setLabel(tr("USB Current"));
  ui->grUsbI->yAxis->setRange(0,GR_USB_CURRENT_MAX);
  ui->grUsbI->xAxis->setRange(0,time_points);
  ui->grUsbI->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // ----------------------------------------
  // Настраиваем график мощности
  ui->grPower->addGraph();

  // Цвета
  ui->grPower->graph(0)->setPen(QPen(Qt::darkYellow));

  ui->grPower->xAxis->setLabel(tr("Time"));
  ui->grPower->yAxis->setLabel(tr("Power"));
  ui->grPower->yAxis->setRange(0,def_power);
  ui->grPower->xAxis->setRange(0,time_points);
  ui->grPower->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // ----------------------------------------
  // Настраиваем график напряжений на батареях
  ui->grBatVoltage->addGraph();
  if (batCnt == 2) ui->grBatVoltage->addGraph();
  if (batCnt == 3) ui->grBatVoltage->addGraph();


  // Цвета батарей разные
  ui->grBatVoltage->graph(0)->setPen(QPen(Qt::darkGreen));
  if (batCnt == 2) ui->grBatVoltage->graph(1)->setPen(QPen(Qt::darkRed));
  if (batCnt == 3) ui->grBatVoltage->graph(2)->setPen(QPen(Qt::darkGray));

  ui->grBatVoltage->xAxis->setLabel(tr("Time"));
  ui->grBatVoltage->yAxis->setLabel(tr("Bat. Voltage"));
  ui->grBatVoltage->yAxis->setRange(GR_BAT_VOLTAGE_MIN, GR_BAT_VOLTAGE_MAX);
  ui->grBatVoltage->xAxis->setRange(0,time_points);
  ui->grBatVoltage->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);


  //Заполним массивы;
  for (int i=0; i < time_points; i++)
    {
      grUSB_x.push_front(i);
      grUSB_I_y.push_front(0);
      grUSB_U_y.push_front(0);
      grPower.push_front(0);
      grBat1.push_front(0);
      grBat2.push_front(0);
      grBat3.push_front(0);
    };

}

void MainWindow::graphUpdate()
{
  // Обновление графиков
  // Добавим данные в массив со сдвигом ток
  grUSB_I_y.push_front(curUsbI);
  grUSB_I_y.pop_back();
  // Добавим данные в массив со сдвигом напряжение
  grUSB_U_y.push_front(curUsbU);
  grUSB_U_y.pop_back();

  // Данные по мощности
  grPower.push_front(curPower);
  grPower.pop_back();

  // Батареи
  grBat1.push_front(curBat1);
  grBat2.push_front(curBat2);
  grBat3.push_front(curBat3);
  grBat1.pop_back();
  grBat2.pop_back();
  grBat3.pop_back();

  ui->grUsbU->graph(0)->setData(grUSB_x, grUSB_U_y,false);
  ui->grUsbI->graph(0)->setData(grUSB_x, grUSB_I_y);
  ui->grPower->graph(0)->setData(grUSB_x,grPower);
  ui->grBatVoltage->graph(0)->setData(grUSB_x,grBat1);
  if (batCnt == 2) ui->grBatVoltage->graph(1)->setData(grUSB_x,grBat2);
  if (batCnt == 3) ui->grBatVoltage->graph(2)->setData(grUSB_x,grBat3);


  ui->grUsbU->replot();
  ui->grUsbI->replot();
  ui->grPower->replot();
  ui->grBatVoltage->replot();


  //ui->grUsb->graph(0)->rescaleAxes(true);
  //ui->grUsb->graph(1)->rescaleAxes(true);


  //ui->lbVoltage->setText(QString("%1").arg(usb_v));


}

int MainWindow::getBatCnt()
{
  // Кол-во батарей
  if (port.isOpen())
    {
      QString tmp = sendConnand(CMD_BAT_CNT);
      tmp.remove(0,2);
      return tmp.toInt();
    }
  else
    {
      return 1;
    }
}

void MainWindow::updateInfo()
{
  ui->groupBox->setTitle(tr("Device info"));
  ui->lbDescr->setText(dnaSerialInfo.description());
  ui->lbManufacturer->setText((dnaSerialInfo.manufacturer()));
  ui->lbName->setText(dnaSerialInfo.portName());
  ui->lbSerial->setText(dnaSerialInfo.serialNumber());
}

void MainWindow::clearInfo()
{
  ui->groupBox->setTitle(tr("Connecting..."));
  ui->lbDescr->setText("-");
  ui->lbManufacturer->setText("-");
  ui->lbName->setText("-");
  ui->lbSerial->setText("-");
}

QString MainWindow::sendConnand(QString text)
{
  QString str = text + "\n";

  int interval = ui->spinUpdTime->value() - 5;

    //qDebug() << "Interval = " << interval;

  port.write(str.toLocal8Bit());
  if (!port.waitForBytesWritten(interval))
    {
      deviceDisconnect();
      return "";
    }

  if (!port.waitForReadyRead(interval))
    {
      deviceDisconnect();
      return "";
    }

  QByteArray ba;
  ba = port.readAll();
  str = QString().fromLocal8Bit(ba.data());
  return str.trimmed();
}

bool MainWindow::deviceConnect()
{
  if (port.isOpen())
    {
      qDebug() << "port is open!";
      return true;
    }

  if (!serialInit())
    {
      qDebug() << "not serialInit()";
      return false;
    };

  if (port.open(QIODevice::ReadWrite))
    {
      qDebug() << "Port opened!";
      batCnt = getBatCnt();
      updateInfo();
      graphInit();

      // Запускаем основной таймер
      timer->start(ui->spinUpdTime->value());

      return true;
    }
  else
    {
      qDebug()<< "Error open port! " << port.errorString();
      clearInfo();
      return false;
    };
}

bool MainWindow::deviceDisconnect()
{

  port.close();
  clearInfo();

  // Основной таймер останавливаем
  if (fireTimer->isActive())
    stopAnalyze();

  timer->stop();

  // Запускаем опять тайпер на поиск
  findTimer->start(FIND_TIMER);
  return true;
}

void MainWindow::getCur()
{
  if (!port.isOpen())
    {
      qDebug() << "getCur dev not cnnect!";
      deviceConnect();
    };

  // Ток USB
  QString tmp_i = sendConnand(CMD_USB_CURRENT);
  tmp_i.replace("U=","",Qt::CaseInsensitive);
  tmp_i.replace("A","",Qt::CaseInsensitive);
  curUsbI = tmp_i.toDouble();

  // Напряжение USB
  QString tmp_v = sendConnand(CMD_USB_VOLTAGE);
  tmp_v.replace("U=","",Qt::CaseInsensitive);
  tmp_v.replace("V","",Qt::CaseInsensitive);
  curUsbU = tmp_v.toDouble();

  // Мощность
  QString tmp_p = sendConnand(CMD_CURRENT_POWER);
  tmp_p.replace("P=","",Qt::CaseInsensitive);
  tmp_p.replace("W","",Qt::CaseInsensitive);
  curPower = tmp_p.toFloat();

  // Батареи
  QString tmp_b1 = sendConnand(CMD_GET_CELL_1);
  QString tmp_b2 = sendConnand(CMD_GET_CELL_2);
  QString tmp_b3 = sendConnand(CMD_GET_CELL_3);
  QString tmp_ball = sendConnand(CMD_GET_BATT);
  tmp_b1.replace("B=","",Qt::CaseInsensitive);
  tmp_b1.replace("V","",Qt::CaseInsensitive);
  curBat1 = tmp_b1.toFloat();
  tmp_b2.replace("B=","",Qt::CaseInsensitive);
  tmp_b2.replace("V","",Qt::CaseInsensitive);
  curBat2 = tmp_b2.toFloat();
  tmp_b3.replace("B=","",Qt::CaseInsensitive);
  tmp_b3.replace("V","",Qt::CaseInsensitive);
  curBat3 = tmp_b3.toFloat();
  tmp_ball.replace("B=","",Qt::CaseInsensitive);
  tmp_ball.replace("V","",Qt::CaseInsensitive);
  curBatAll = tmp_ball.toFloat();

  // Минимальное напряжение трёх батарей
  switch (batCnt)
    {
    case 1:
      curBatMin = curBat1;
      break;
    case 2:
      curBatMin = qMin(curBat1,curBat2);
      break;
    case 3:
      curBatMin = qMin(qMin(curBat1,curBat2),curBat3);
      break;
    default:
      curBatMin = curBat1;
      break;
    }

}

void MainWindow::stopAnalyze()
{
  ui->btnStop->setEnabled(false);

  // Остановка анализа
  fireTimer->stop();
  // Отключаем FIRE, если шел
  QString cmd_fire = "F=0.1S\n";
  port.write(cmd_fire.toLocal8Bit());
  if (!port.waitForBytesWritten(1000))
      deviceDisconnect();

  // Запускаем таймер для устаканивания напряжения
  // для поиска нулевого напряжения
  relaxTimer->start(5000);

}


void MainWindow::slotTimer()
{

  // Обновим текущие значения
  getCur();

  // Посчитаем мощность
  double t = (double(timer->interval()) / 1000) / 3600;
  curEnergy += (curPower * t);


  ui->lbUSBI->setText(QString("%1 A").arg(curUsbI,0,'L',2));
  ui->lbUSBV->setText(QString("%1 V").arg(curUsbU,0,'L',2));
  ui->lbPower->setText(QString("%1 W").arg(curPower,0,'L',2));
  ui->lbBat->setText(QString("%1 V").arg(curBatAll,0,'L',2));
  //ui->lbCurVB1->setText(QString("%1 V").arg(qMax(qMax(curBat1,curBat2),curBat3),0,'L',2));
  ui->lbVol->setText(QString("%1 Wh").arg(curEnergy,0,'L',3));

  if (graphRun)
    {
      graphUpdate();
    }

  // Анализ просадки и остановка
  if (curBatMin < ui->spinStopVolt->value() && fireTimer->isActive())
    stopAnalyze();
}

void MainWindow::slotFindTimer()
{
  qDebug() << "slotFindTimer";
  // Поиск устройства
  if (serialInit())
    {
      if (deviceConnect())
        findTimer->stop();
    }
  else
    deviceDisconnect();
}


void MainWindow::slotBtnApplyGraphSettings()
{
  // Применить настройки
  timer->stop();
  graphInit();
  timer->start(ui->spinUpdTime->value());
}

void MainWindow::slotGraphRun()
{
  graphRun = !graphRun;
}

void MainWindow::slotBtnStart()
{
  // Старт запуска
  // Отключаем интерфейс

  ui->tab->setEnabled(false);
  ui->btnStart->setEnabled(false);
  ui->btnStop->setEnabled(true);
  ui->btnGraphRun->setEnabled(false);
  ui->btnSave->setEnabled(false);

  // Расчет цикла таймера
  // длительность цикла = длительность Fire + длительность паузы

  int period = (ui->spinFIreDuration->value() * 1000) +
               (ui->spinRelax->value() * 1000);


  // Отключаем USB-зарядку
  QString cmd_off = "U=0\n";
  port.write(cmd_off.toLocal8Bit());
  if (!port.waitForBytesWritten(1000))
      deviceDisconnect();

  // Устанавливаем мощность
  QString cmd_power = QString("P=%1W\n").arg(ui->spinTestPower->value());
  port.write(cmd_power.toLocal8Bit());
  if (!port.waitForBytesWritten(1000))
      deviceDisconnect();

  // Очистим массив с данными
  volPoints.clear();
  // Текущую энергию
  curEnergy = 0;

  // Запуск
  fireTimer->start(period);
}

void MainWindow::slotBtnStop()
{
  stopAnalyze();
}

void MainWindow::slotFireTimer()
{
  // Добавим установившееся напряжение
  // и суммарную энергию в массив
  volPoint p;
  p.voltage = curBatMin;
  p.energy = curEnergy;
  volPoints.push_back(p);

  // Таймер запуска процесса
  // Жарим установленное время
  QString cmd = QString("F=%1S\n").arg(ui->spinFIreDuration->value());

  // Команда уходит без ответа
  port.write(cmd.toLocal8Bit());
  if (!port.waitForBytesWritten(1000))
      deviceDisconnect();

}

void MainWindow::slotRelaxTimer()
{
  // Остановка таймеров
  relaxTimer->stop();
  fireTimer->stop();

  // Включаем USB-зарядку
  QString cmd_off = "U=2\n";
  port.write(cmd_off.toLocal8Bit());
  if (!port.waitForBytesWritten(1000))
      deviceDisconnect();


  // Интерфейс
  ui->tab->setEnabled(true);
  ui->btnStart->setEnabled(true);
  ui->btnGraphRun->setEnabled(true);

  // Запишем последние данные
  lastEnergy = curEnergy; // Понадобится для отображения
  volPoint p;
  p.voltage = curBatMin;
  p.energy = curEnergy;
  volPoints.push_back(p);

  // Если данных недостаточно - выходим
  if (volPoints.size() < 5)
    return;

  // Расчет энергии и процентов
  for (int i = 0; i < volPoints.size(); ++i)
    {
      volPoints[i].reverse_energy = lastEnergy - volPoints[i].energy;
      volPoints[i].percent = (volPoints[i].reverse_energy / lastEnergy) * 100;
    };

  // Удаление лишних записей из массива
  // Сначала сортировка
  qSort(volPoints.begin(), volPoints.end(), lessThan);

  // Удаление повторяющихся значений напряжения
  for (int i = 1; i < volPoints.size(); ++i)
    {
      if (volPoints[i].voltage == volPoints[i-1].voltage &&
          volPoints.size() > 5)
        {
          volPoints.remove(i);
          i--;
        }
    }


  // TODO Оставляем только 30 точек графика
  int max = volPoints.size();
  if (max > 30)
    {
      // Проставим индексы по ненужному полю
      // по логарифмической шкале
      // кроме первой и последней записи


      for (int i = 1; i < (max-1); ++i)
        {
          volPoints[i].energy = int(qLn(double(i)*(double(30)/double(max)) + 1) * 9);
        }

      // а теперь удалим дубли по этому полю
      for (int i = 1; i < (max-1); ++i)
        {
          if (volPoints[i].energy == volPoints[i-1].energy
              && volPoints.size() > 30)
            {
              volPoints.remove(i);
              i--;
            }
        }
    }


  ui->btnSave->setEnabled(true);

}

bool lessThan(const volPoint &d1, const volPoint &d2)
{
    return d1.voltage > d2.voltage;
}

void MainWindow::slotBtnSave()
{
  // Сохраняем в csv

  QString fileName = QFileDialog::getSaveFileName(this,
          tr("Save CSV data"), "",
          tr("Text SCV (*.csv);;All Files (*)"));

  QFile f(fileName);
  QTextStream s(&f);
  if (f.open(QIODevice::ReadWrite))
    {
      s << "Battery Charge (%),Cell Voltage (V)" << endl;
      for (int i = volPoints.size()-1; i >= 0 ; i--)
        {
          s << QString::number(volPoints[i].percent)
            << ","
            << QString::number(volPoints[i].voltage)
            << endl;
        }
    }
  f.close();

}

// TODO delete this

/*
void MainWindow::slotPb()
{

  int dot = 20;
  int max = 300;

  for (int i = 1; i < max; ++i)
    {
      qDebug() << i << int(qLn(double(i)*(double(dot)/double(max)) + 1) * 9);
    }

}
*/






