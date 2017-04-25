#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

// #define READ_ALL_SAMPLES

QVector<double> GaussianBlur(QVector<double> &src)
{
    int i;
    QVector<double> dst(src.size());

    dst[0] = (src[0] * 96 + src[1] * 64 + src[2] * 16) / (96 + 64 + 16);
    dst[1] = (src[0] * 64 + src[1] * 96 + src[2] * 64 + src[3] + 16) / (64 + 96 + 64 + 16);
    for(i = 2; i < src.size() - 2; i++)
    {
  //      qDebug() << It;
        dst[i] = (src[i-2] + src[i+2]) * 16 + (src[i-1] + src[i+1]) * 64  +  src[i] * 96;
        dst[i] /= 256;
    }
    dst[i] = src[i-2] * 16 + (src[i-1] + src[i+1]) * 64  +  src[i] * 96;
    dst[i] /= 64 + 96 + 64 + 16;
    dst[i] = src[i-2]* 16 + src[i-1] * 64  +  src[i] * 96;
    dst[i] /= 96 + 64 + 16;

    return dst;
}

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
  connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void MainWindow::setupMyDemo(QCustomPlot *customPlot)
{
  demoName = "My Demo";
  // generate some data:
  QVector<double> x(101), y(101), y1(101), y2(101), y3(101); // initialize with entries 0..100
  QVector<double> yFiltered(101), y1Filtered(101), y2Filtered(101), y3Filtered(101);

#if 0
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  "~/git/ECGsInterpreter/ECG/",
                                                  tr("Samples (*.csv)"));
  QFile file(fileName);
#else
//  QString fileName = "/home/nope/Qt/Apps/ECG/samples.csv";
  QString fileName = "/home/savum/git/ECGsInterpreter/ECG/samples.csv";
  QFile file(fileName);
#endif

  qDebug() << fileName;
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << file.errorString();
    return;
  }
  int counter = 0;
  QStringList wordList;
  double xVal = 0.000;
  // Read first two lines, parse later
  QByteArray line = file.readLine();
  line = file.readLine();
#ifdef READ_ALL_SAMPLES
  while (!file.atEnd()) {
#else
  counter = 0;
  while(!file.atEnd() && (counter++) < 2500) {
#endif
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
#ifdef READ_ALL_SAMPLES
  while (!file.atEnd()) {
#else
  counter = 0;
  while(!file.atEnd() && (counter++) < 2500) {
#endif
    line = file.readLine();
    y.append(line.split(',').at(4).toDouble());
    y1.append(line.split(',').at(5).toDouble() - 2.5);
    y2.append(line.split(',').at(6).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);
  }

  file.seek(0);
  line = file.readLine();
  line = file.readLine();
#ifdef READ_ALL_SAMPLES
  while (!file.atEnd()) {
#else
  counter = 0;
  while(!file.atEnd() && (counter++) < 2500) {
#endif
    line = file.readLine();
    y.append(line.split(',').at(7).toDouble());
    y1.append(line.split(',').at(8).toDouble() - 2.5);
    y2.append(line.split(',').at(9).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);
  }

  file.seek(0);
  line = file.readLine();
  line = file.readLine();
#ifdef READ_ALL_SAMPLES
  while (!file.atEnd()) {
#else
  counter = 0;
  while(!file.atEnd() && (counter++) < 2500) {
#endif
    line = file.readLine();
    y.append(line.split(',').at(7).toDouble());
    y1.append(line.split(',').at(11).toDouble() - 2.5);
    y2.append(line.split(',').at(12).toDouble() - 5);
    y3.append(line.split(',').at(2).toDouble() - 7.5);

  }
  file.close();

  yFiltered  = GaussianBlur(y);
  y1Filtered = GaussianBlur(y1);
  y2Filtered = GaussianBlur(y2);
  y3Filtered = GaussianBlur(y3);

#if 1
  for(int bla = 0; bla < 100; bla++) {
      yFiltered  = GaussianBlur(yFiltered);
      y1Filtered = GaussianBlur(y1Filtered);
      y2Filtered = GaussianBlur(y2Filtered);
      y3Filtered = GaussianBlur(y3Filtered);
  }
#endif

  // create graph and assign data to it:
#if 1
  customPlot->addGraph();
  customPlot->graph(0)->setData(x, y);
  customPlot->addGraph();
  customPlot->graph(1)->setData(x, y1);
  customPlot->addGraph();
  customPlot->graph(2)->setData(x, y2);
  customPlot->addGraph();
  customPlot->graph(3)->setData(x, y3);
#elif 0
  customPlot->addGraph();
  customPlot->graph(0)->setData(x, yFiltered);
  customPlot->addGraph();
  customPlot->graph(1)->setData(x, y1Filtered);
  customPlot->addGraph();
  customPlot->graph(2)->setData(x, y2Filtered);
  customPlot->addGraph();
  customPlot->graph(3)->setData(x, y3Filtered);
#else
  for(int i = 0 ; i < yFiltered.size(); i++) {
      yFiltered[i] -= 10;
      y1Filtered[i] -= 10;
      y2Filtered[i] -= 10;
      y3Filtered[i] -= 10;
  }

  customPlot->addGraph();
  customPlot->graph(0)->setData(x, y);
  customPlot->addGraph();
  customPlot->graph(1)->setData(x, y1);
  customPlot->addGraph();
  customPlot->graph(2)->setData(x, y2);
  customPlot->addGraph();
  customPlot->graph(3)->setData(x, y3);
  customPlot->addGraph();
  customPlot->graph(4)->setData(x, yFiltered);
  customPlot->addGraph();
  customPlot->graph(5)->setData(x, y1Filtered);
  customPlot->addGraph();
  customPlot->graph(6)->setData(x, y2Filtered);
  customPlot->addGraph();
  customPlot->graph(7)->setData(x, y3Filtered);
#endif

  // give the axes some labels:
  customPlot->xAxis->setLabel("x [s]");
  customPlot->yAxis->setLabel("y [mV]");
  // set axes ranges, so we see all data:
  customPlot->xAxis->setRange(0, 10);
  customPlot->yAxis->setRange(5, -10);
//  customPlot->rescaleAxes();

#if 1
  customPlot->xAxis->grid()->setVisible(true);
  customPlot->yAxis->grid()->setVisible(true);
  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->yAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));
  customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));
  customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 0.75, Qt::DotLine));
  customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 0.75, Qt::DotLine));

  QSharedPointer <QCPAxisTickerFixed> tickerX(new QCPAxisTickerFixed);
  QSharedPointer <QCPAxisTickerFixed> tickerY(new QCPAxisTickerFixed);
  tickerX->setTickStep(0.2);
  tickerY->setTickStep(0.5);

  customPlot->xAxis->setTicker(tickerX);
  customPlot->yAxis->setTicker(tickerY);
#endif

  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
}

void MainWindow::mouseWheel()
{
    // TODO: check for label selected or units selected
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

void MainWindow::mousePress()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

MainWindow::~MainWindow()
{
    delete ui;
}
