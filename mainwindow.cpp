#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->lbHot->setVisible(false);

  mod1 = new DnaMod(this);
  mod2 = new NuvotonMod(this);
  mod = NULL;

  findTimer = new QTimer(this);

  connect(this->findTimer,SIGNAL(timeout()),this,SLOT(slotFindTimer()));
  connect(ui->btnApplyGraphSettings,SIGNAL(clicked(bool)),this,SLOT(slotBtnApplyGraphSettings()));
  connect(ui->btnGraphRun,SIGNAL(clicked(bool)),this,SLOT(slotGraphRun()));
  connect(ui->btnStart,SIGNAL(clicked(bool)),this,SLOT(slotBtnStart()));
  connect(ui->btnStop,SIGNAL(clicked(bool)),this,SLOT(slotBtnStop()));
  connect(ui->btnSave,SIGNAL(clicked(bool)),this,SLOT(slotBtnSave()));



  // TODO delete this
  connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(slotPb()));

  // При старте запуск поиска устройства
  findTimer->start(FIND_TIMER);

  graphRun = true;

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::modInit()
{
  // Инициализация мода
  connect(mod,SIGNAL(sigStopAnalyze(bool)),this,SLOT(slotStopAnalyze(bool)));
  connect(mod,SIGNAL(sigGeneralTimer()),this,SLOT(slotGeneralTimer()));
  connect(mod,SIGNAL(sigBoardHot(int)),this,SLOT(slotHot(int)));
  connect(mod,SIGNAL(sigBoardOk()),this,SLOT(slotHotOk()));

  mod->setBatPlot(ui->grBatVoltage);
  mod->setPowerPlot(ui->grPower);
  mod->setUSBPlotI(ui->grUsbI);
  mod->setUSBPlotU(ui->grUsbU);
  mod->setPlotTimeline(ui->spinTimeLine->value());
  mod->setPlotPowerMax(ui->spinDefPower->value());

  mod->plotInit();
  mod->startGenTimer(ui->spinUpdTime->value());
}

void MainWindow::updateInfo()
{
  deviceInfo d = mod->getDeviceInfo();
  ui->groupBox->setTitle(tr("Device info"));
  ui->lbDescr->setText(d.description);
  ui->lbManufacturer->setText((d.manufacturer));
  ui->lbName->setText(d.portName);
  ui->lbSerial->setText(d.serialNumber);
}

void MainWindow::clearInfo()
{
  ui->groupBox->setTitle(tr("Connecting..."));
  ui->lbDescr->setText("-");
  ui->lbManufacturer->setText("-");
  ui->lbName->setText("-");
  ui->lbSerial->setText("-");
}


void MainWindow::slotFindTimer()
{
  if (mod != NULL)
    {
      {
        if (mod->devIsConnected())
          return;
      }
      mod->stopTimers();
    }
  mod = NULL;

  if (mod1->devConnect())
    {
      ui->spinTestPower->setEnabled(true);
      mod = mod1;
    }

  if (mod2->devConnect())
    {
      ui->spinTestPower->setEnabled(false);
      mod = mod2;
    }

  if (mod != NULL)
    {
      modInit();
      updateInfo();
    }
  else
    {
      clearInfo();
    }
}


void MainWindow::slotBtnApplyGraphSettings()
{
  // Применить настройки
  mod->stopTimers();
  modInit();
}

void MainWindow::slotGraphRun()
{
  graphRun = !graphRun;
  ui->btnStart->setEnabled(graphRun);
  if (graphRun)
    {
      mod->startGenTimer(ui->spinUpdTime->value());
    }
  else
    {
      mod->stopTimers();
    }
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

  mod->startAnalyze(ui->spinFIreDuration->value() * 1000,
                    ui->spinRelax->value() * 1000,
                    ui->spinTestPower->value(),
                    ui->spinStartVolt->value(),
                    ui->spinStopVolt->value());

}

void MainWindow::slotBtnStop()
{
  ui->btnStop->setEnabled(false);
  mod->stopAnalyze();
}



void MainWindow::slotBtnSave()
{
  // Сохраняем в csv

  QString selectedFilter;
  QString fileName = QFileDialog::getSaveFileName(this,
          tr("Save file"), "",
          tr("DNA format (*.csv);;ArcticFox format (*.xml);;All Files (*.*)"),&selectedFilter);

  if (selectedFilter == tr("DNA format (*.csv)"))
    {
      saveFormatDNA d(mod->getOutCurve(), mod->getCurInfo());
      d.saveToFile(fileName);
    }

  if (selectedFilter == tr("ArcticFox format (*.xml)"))
    {
      saveFormatArctocFox d(mod->getOutCurve(), mod->getCurInfo());
      d.saveToFile(fileName);
    }
}

void MainWindow::slotStopAnalyze(bool ok)
{
  //
  ui->tab->setEnabled(true);
  ui->btnStart->setEnabled(true);
  ui->btnGraphRun->setEnabled(true);
  ui->btnSave->setEnabled(ok);
}

void MainWindow::slotGeneralTimer()
{
  // Основной таймерю Обновляем интерфейс
  ui->lbUSBI->setText(QString("%1 A").arg(mod->getCurInfo()->usb_i, 0, 'L', 2));
  ui->lbUSBV->setText(QString("%1 V").arg(mod->getCurInfo()->usb_u, 0, 'L', 2));
  ui->lbPower->setText(QString("%1 W").arg(mod->getCurInfo()->power, 0, 'L', 2));
  ui->lbBat->setText(QString("%1 V").arg(mod->getCurInfo()->bat_all, 0, 'L', 2));
  ui->lbVol->setText(QString("%1 Wh").arg(mod->getCurInfo()->energy, 0, 'L', 3));
  ui->lbTemp->setText(QString("%1 C").arg(mod->getCurInfo()->dev_temp, 0, 'L', 2));
}

void MainWindow::slotHot(int period)
{
  ui->lbHot->setVisible(true);
  ui->lbHot->setText(QString(tr("BOARD TOO HOT!!! period = %1 sec.")).arg(period / 1000));
}

void MainWindow::slotHotOk()
{
  ui->lbHot->setVisible(false);
}

// TODO delete this
void MainWindow::slotPb()
{

}







