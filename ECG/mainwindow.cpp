#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setGeometry(400, 250, 542, 390);
  setupMyDemo(ui->customPlot);

  setWindowTitle("QCustomPlot: "+demoName);
  statusBar()->clearMessage();
  ui->customPlot->replot();
  connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}


void MainWindow::setupMyDemo(QCustomPlot *customPlot)
{
  demoName = "My Demo";
  // generate some data:
  QVector<double> x(101), y(101), y1(101), y2(101), y3(101); // initialize with entries 0..100

#if 1
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  "",
                                                  tr("Samples (*.csv)"));
  QFile file(fileName);
#else
  QString fileName = "/home/nope/Qt/Apps/ECG/samples.csv";
  QFile file(fileName);
#endif

  qDebug() << fileName;
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << file.errorString();
    return;
  }
  QStringList wordList;
  double xVal = 0.000;
  // Read first two lines, parse later
  QByteArray line = file.readLine();
  line = file.readLine();
  while (!file.atEnd()) {
    line = file.readLine();
    x.append(xVal);
    xVal += 0.001;
    x.append(xVal);
    xVal += 0.001;
    x.append(xVal);
    xVal += 0.001;
    x.append(xVal);
    xVal += 0.001;
    y.append(line.split(',').at(1).toDouble());
    y1.append(line.split(',').at(2).toDouble() - 2.5);
    y2.append(line.split(',').at(3).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);
  }

  file.seek(0);
  line = file.readLine();
  line = file.readLine();
  while (!file.atEnd()) {
    line = file.readLine();
    y.append(line.split(',').at(4).toDouble());
    y1.append(line.split(',').at(5).toDouble() - 2.5);
    y2.append(line.split(',').at(6).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);
  }

  file.seek(0);
  line = file.readLine();
  line = file.readLine();
  while (!file.atEnd()) {
    line = file.readLine();
    y.append(line.split(',').at(7).toDouble());
    y1.append(line.split(',').at(8).toDouble() - 2.5);
    y2.append(line.split(',').at(9).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);
  }

  file.seek(0);
  line = file.readLine();
  line = file.readLine();
  while (!file.atEnd()) {
    line = file.readLine();
    y.append(line.split(',').at(7).toDouble());
    y1.append(line.split(',').at(11).toDouble() - 2.5);
    y2.append(line.split(',').at(12).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);

  }
  file.close();


  // create graph and assign data to it:
  customPlot->addGraph();
  customPlot->graph(0)->setData(x, y);
  customPlot->addGraph();
  customPlot->graph(1)->setData(x, y1);
  customPlot->addGraph();
  customPlot->graph(2)->setData(x, y2);
  customPlot->addGraph();
  customPlot->graph(3)->setData(x, y3);

  // give the axes some labels:
  customPlot->xAxis->setLabel("x");
  customPlot->yAxis->setLabel("y");
  // set axes ranges, so we see all data:
  customPlot->xAxis->setRange(-5, 45);
  customPlot->yAxis->setRange(-10, 10);
  customPlot->rescaleAxes();

  customPlot->xAxis->grid()->setVisible(true);
  customPlot->yAxis->grid()->setVisible(true);
  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->yAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));
  customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));
  customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 0.75, Qt::DotLine));
  customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 0.75, Qt::DotLine));

//  QSharedPointer <QCPAxisTickerFixed> ticker();
////  ticker->setTickStepStrategy(QCPAxisTickerFixed);
//  ticker->setTickCount(x.length() / 50000);
//  customPlot->yAxis->setTicker(ticker);

  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
}

void MainWindow::mouseWheel()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis,ui->customPlot->yAxis);
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  }
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis,ui->customPlot->yAxis);
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  }
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

MainWindow::~MainWindow()
{
    delete ui;
}
