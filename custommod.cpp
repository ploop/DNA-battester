#include "custommod.h"

CustomMod::CustomMod(QObject *parent) : QObject(parent)
{
  generalTimer = new QTimer(this);
  relaxTimer = new QTimer(this);
  fireTimer = new QTimer(this);

  connect(generalTimer,SIGNAL(timeout()),this,SLOT(slotGeneralTimer()));
  connect(fireTimer,SIGNAL(timeout()),this,SLOT(slotFireTimer()));
  connect(relaxTimer,SIGNAL(timeout()),this,SLOT(slotRelaxTimer()));

}

CustomMod::~CustomMod()
{
  //qDebug() << "Destructor CustomMod";
}

double CustomMod::getUsbI()
{
  return -1;
}

double CustomMod::getUsbU()
{
  return -1;
}

double CustomMod::getPower()
{
  return -1;
}

double CustomMod::getBat1()
{
  return -1;
}

double CustomMod::getBat2()
{
  return -1;
}

double CustomMod::getBat3()
{
  return -1;
}

double CustomMod::getBatAll()
{
  return -1;
}

double CustomMod::getBoardTemp()
{
  return -1;
}

void CustomMod::sendFire(int ms)
{
  ms = ms;
}

void CustomMod::sendStopFire()
{

}

void CustomMod::sendUsbCharge(int mA)
{
  mA = mA;
}

void CustomMod::sendPower(double p)
{
  p = p;
}

int CustomMod::batCount()
{
  return 1;
}



void CustomMod::setUSBPlotI(QCustomPlot *p)
{
  USBPlotI = p;
}

void CustomMod::setUSBPlotU(QCustomPlot *p)
{
  USBPlotU = p;
}

void CustomMod::setBatPlot(QCustomPlot *p)
{
  batPlot = p;
}

void CustomMod::setPowerPlot(QCustomPlot *p)
{
  powerPlot = p;
}

void CustomMod::setPlotTimeline(int tl)
{
  timeLine = tl;
}

void CustomMod::setPlotPowerMax(double powerMax)
{
  plotPowerMax = powerMax;
}

void CustomMod::plotInit()
{
  // Инициализация графиков

  // Очищаем массивы с данными
  x_axis.clear();
  y_usb_i.clear();
  y_usb_u.clear();
  y_power.clear();
  y_bat_1.clear();
  y_bat_2.clear();
  y_bat_3.clear();

  // Очищаем сами графики
  USBPlotI->clearItems();
  USBPlotU->clearItems();
  batPlot->clearItems();
  powerPlot->clearItems();

  // График напряжения USB
  USBPlotU->addGraph();
  USBPlotU->graph(0)->setPen(QPen(Qt::blue));
  USBPlotU->xAxis->setLabel(tr("Time"));
  USBPlotU->yAxis->setLabel(tr("USB Voltage"));
  USBPlotU->yAxis->setRange(GR_USB_VOLTAGE_MIN, GR_USB_VOLTAGE_MAX);
  USBPlotU->xAxis->setRange(0, timeLine);
  USBPlotU->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // График тока USB
  USBPlotI->addGraph();
  USBPlotI->graph(0)->setPen(QPen(Qt::red));
  USBPlotI->xAxis->setLabel(tr("Time"));
  USBPlotI->yAxis->setLabel(tr("USB Current"));
  USBPlotI->yAxis->setRange(0, GR_USB_CURRENT_MAX);
  USBPlotI->xAxis->setRange(0, timeLine);
  USBPlotI->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // График мощности
  powerPlot->addGraph();
  powerPlot->graph(0)->setPen(QPen(Qt::darkYellow));
  powerPlot->xAxis->setLabel(tr("Time"));
  powerPlot->yAxis->setLabel(tr("Power"));
  powerPlot->yAxis->setRange(0, plotPowerMax);
  powerPlot->xAxis->setRange(0, timeLine);
  powerPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // График напряжений на батареях
  batPlot->addGraph();
  if (batCount() == 2) batPlot->addGraph();
  if (batCount() == 3) batPlot->addGraph();
  batPlot->graph(0)->setPen(QPen(Qt::darkGreen));
  if (batCount() == 2) batPlot->graph(1)->setPen(QPen(Qt::darkRed));
  if (batCount() == 3) batPlot->graph(2)->setPen(QPen(Qt::darkGray));
  batPlot->xAxis->setLabel(tr("Time"));
  batPlot->yAxis->setLabel(tr("Bat. Voltage"));
  batPlot->yAxis->setRange(GR_BAT_VOLTAGE_MIN, GR_BAT_VOLTAGE_MAX);
  batPlot->xAxis->setRange(0, timeLine);
  batPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  //Очистим массивы;
  for (int i=0; i < timeLine; i++)
    {
      x_axis.push_front(i);
      y_usb_i.push_front(0);
      y_usb_u.push_front(0);
      y_power.push_front(0);
      y_bat_1.push_front(0);
      y_bat_2.push_front(0);
      y_bat_3.push_front(0);
    };
}

void CustomMod::stopTimers()
{
  generalTimer->stop();
  fireTimer->stop();
  relaxTimer->stop();
}

void CustomMod::startGenTimer(int interval)
{
  generalTimer->start(interval);
}

void CustomMod::startAnalyze(int fireTime, int interval, double power, double startVoltage, double stopVoltage)
{
  // Расчет цикла таймера
  // длительность цикла = длительность Fire + длительность паузы
  period = fireTime + interval;

  // Включаем/отключаем USB зарядку
  startVolt = startVoltage;
  stopVolt = stopVoltage;
  this->fireTime = fireTime;
  if (startVoltage > stopVoltage)
    {
      sendUsbCharge(DEFAULT_USB_CHARGE);
    }
  else
    {
      sendUsbCharge(0);
    };

  // Устанавливаем мощность
  sendPower(power);

  // Очищаем массив с данными и текущую энергию
  curve.clear();
  info.energy = 0;

  // Запуск
  fireTimer->start(period);
}

curInfo *CustomMod::getCurInfo()
{
  return &info;
}

QVector<outCurve> *CustomMod::getOutCurve()
{
  return &curve;
}

bool CustomMod::devConnect()
{
  return false;
}

bool CustomMod::devDisconnect()
{
  return false;
}

bool CustomMod::devIsConnected()
{
  return false;
}

deviceInfo CustomMod::getDeviceInfo()
{
  deviceInfo d;
  return d;
}

void CustomMod::plotUpdate()
{
  // Обновление графиков
  // Добавим данные в массив со сдвигом
  y_usb_i.push_front(info.usb_i);
  y_usb_i.pop_back();
  y_usb_u.push_front(info.usb_u);
  y_usb_u.pop_back();
  y_power.push_front(info.power);
  y_power.pop_back();
  y_bat_1.push_front(info.bat1);
  y_bat_2.push_front(info.bat2);
  y_bat_3.push_front(info.bat3);
  y_bat_1.pop_back();
  y_bat_2.pop_back();
  y_bat_3.pop_back();

  // Устанавливаем массивы для отображения
  USBPlotI->graph(0)->setData(x_axis, y_usb_i);
  USBPlotU->graph(0)->setData(x_axis, y_usb_u);
  powerPlot->graph(0)->setData(x_axis, y_power);
  batPlot->graph(0)->setData(x_axis, y_bat_1);
  if (batCount() == 2) batPlot->graph(1)->setData(x_axis, y_bat_2);
  if (batCount() == 3) batPlot->graph(2)->setData(x_axis, y_bat_3);

  USBPlotI->replot();
  USBPlotU->replot();
  powerPlot->replot();
  batPlot->replot();
}

void CustomMod::curUpdate()
{
  // Обновление текущих значений info

  info.usb_i = getUsbI();
  info.usb_u = getUsbU();
  info.power = getPower();
  info.bat1 = getBat1();
  info.bat2 = getBat2();
  info.bat3 = getBat3();
  info.bat_all = getBatAll();
  info.dev_temp = getBoardTemp();

  int bat_cnt = batCount();

  // Минимальное напряжение трёх батарей
  switch (bat_cnt)
    {
    case 1:
      info.bat_min = info.bat1;
      break;
    case 2:
      info.bat_min = qMin(info.bat1, info.bat2);
      break;
    case 3:
      info.bat_min = qMin(qMin(info.bat1, info.bat2), info.bat3);
      break;
    default:
      info.bat_min = info.bat1;
      break;
    }
  // Посчитаем мощность
  double t = (double(generalTimer->interval()) / 1000) / 3600;
  info.energy += (info.power * t);
}

void CustomMod::stopAnalyze()
{
  fireTimer->stop();
  sendStopFire();
  relaxTimer->start(RELAX_TIMER_INTERVAL);
}


// --------------------------
void CustomMod::slotGeneralTimer()
{
  // Основной таймер

  curUpdate();
  plotUpdate();

  // Анализ необходимости остановки
  if (info.bat_min < stopVolt && fireTimer->isActive())
    stopAnalyze();

  // Сигнал наверх для обновления интерфейса
  emit sigGeneralTimer();
}

void CustomMod::slotRelaxTimer()
{
  // Остановка анализа: остановка таймеров

  relaxTimer->stop();
  fireTimer->stop();

  // Включение USB-зарядки на 2А
  sendUsbCharge(DEFAULT_USB_CHARGE);

  // Запишем последние данные
  outCurve c;
  c.voltage = info.bat_min;
  c.energy = info.energy;

  if (curve.last().voltage > info.bat_min &&
      curve.last().energy < info.energy)
    curve.push_back(c);

  info.last_energy = curve.last().energy;

  // Если данных недостаточно - выходим
  // 11 точек минимум для ArcticFox
  bool ok;
  if (curve.size() < 11)
    {
      curve.clear();
      ok = false;
    }
  else
    ok = true;

  // Сигнализируем о готовности
  emit sigStopAnalyze(ok);
}

void CustomMod::slotFireTimer()
{
  // Проверим температуру

  if (info.dev_temp < BOARD_TEMP_LIMIT_1)
    {
      fireTimer->setInterval(period);
      emit sigBoardOk();
    }

  if (info.dev_temp >= BOARD_TEMP_LIMIT_1 &&
      info.dev_temp < BOARD_TEMP_LIMIT_2)
    {
      fireTimer->setInterval(period * 3);
      emit sigBoardHot(period * 3);
    }

  if (info.dev_temp >= BOARD_TEMP_LIMIT_2)
    {
      fireTimer->setInterval(period * 10);
      emit sigBoardHot(period * 10);
    }


  // Если работаем с зарядкой
  if (startVolt > stopVolt)
    {
      if (info.bat_min < startVolt)
        return;
      else
        {
          startVolt = 0;
          sendUsbCharge(0);
        }
    }

  outCurve c;
  c.voltage = info.bat_min;
  c.energy = info.energy;

  if (curve.size() == 0  ||
      (curve.last().voltage > info.bat_min &&
       curve.last().energy < info.energy))
    curve.push_back(c);

  sendFire(fireTime);
}



saveFormatDNA::saveFormatDNA(QVector<outCurve> *c, curInfo *i)
{

  curve = *c;
  info = *i;

  // Cортировка
  qSort(curve.begin(), curve.end(), lessThan);

  // Расчет процентов
  for (int i = 0; i < curve.size(); ++i)
    {
      curve[i].percent = ((info.last_energy - curve[i].energy) / info.last_energy) * 100;
    }

  // Убираем лишние точки, оставляем 30
  int max = curve.size();
  if (max > 30)
    qDebug() << "del > 30";
  {
    // Проставим индексы по ненужному полю
    // по логарифмической шкале
    // кроме первой и последней записи

    for (int i = 1; i < (max-1); ++i)
      {
        curve[i].energy = int(qLn(double(i)*(double(30)/double(max)) + 1) * 9);
      }

    // а теперь удалим дубли по этому полю
    // TODO: В обратном порядке!!!
    for (int i = 1; i < (max-1); ++i)
      {
        if (curve[i].energy == curve[i-1].energy
            && curve.size() > 30)
          {
            curve.remove(i);
            i--;
          }
      }
  }
}


bool saveFormatDNA::saveToFile(QString fileName)
{
  QFile f(fileName);
  QTextStream s(&f);
  if (f.open(QIODevice::ReadWrite))
    {
      s << "Battery Charge (%),Cell Voltage (V)" << endl;
      for (int i = curve.size()-1 ; i >= 0 ; i--)
        {
          s << QString::number(curve[i].percent)
            << ","
            << QString::number(curve[i].voltage)
            << endl;
        }
      f.close();
      return true;
    }
  return false;
}

saveFormatArctocFox::saveFormatArctocFox(QVector<outCurve> *c, curInfo *i) : saveFormatDNA(c, i)
{

  QVector<outCurve> tmpCurve = curve;
  curve.clear();
  for (int i = 0; i < 11; i++)
    {
       outCurve x;
       x.percent = i * 10;
       curve.push_front(x);
    }
  curve.first() = tmpCurve.first();
  curve.last() = tmpCurve.last();

  for (int i = 0; i < 10; i++)
    {
      for (int j = 1; j < tmpCurve.size()-1; j++)
        {
          if (tmpCurve[j].percent < curve[i-1].percent)
            {
              curve[i].voltage = tmpCurve[j].voltage;
              break;
            }
        }
    }

}

bool saveFormatArctocFox::saveToFile(QString fileName)
{
  if (curve.size() == 11)
    {
      double cutoff = curve[10].voltage - 0.02;

      QFile f(fileName);
      if (f.open(QIODevice::WriteOnly))
        {
          QTextStream out(&f);
          out << "<BatteryProfile>\n"
              << "  <Cutoff>"
              << QString::number(cutoff)
              << "</Cutoff>\n"
              << "  <Data>\n";
          for (int i = 10; i >= 0; i--)
            out << QString("    <Point Percent=\"%1\" Voltage=\"%2\" />\n")
                   .arg(curve[i].percent)
                   .arg(curve[i].voltage);
          out << "  </Data>\n"
              << "</BatteryProfile>";
          f.close();
          return true;
        }
    }
return false;
}


// Для сортировки
bool lessThan(const outCurve &d1, const outCurve &d2)
{
  return d1.voltage > d2.voltage;
}


