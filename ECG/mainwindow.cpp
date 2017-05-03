#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

// #define READ_ALL_SAMPLES
static int absDif(int a, int b)
{
    a = a - b;
    if(a > 0)
        return a;
    else
        return -a;
}

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

  /*---------------------------------------*/
  /*Search for QRS waves and get heart rate*/
  /*---------------------------------------*/
  float minVal[2], maxVal[2];
  //minPoints - S waves
  //maxPoints - R waves
  //qPoints   - Q waves
  // Points[0] - xVal (time) / Points[1] - yVal (amplitude) / Points[2] - bufferIt
  float minPoints[3][20], maxPoints[3][20], qPoints[3][20];
  int noOfMinPoints = 0;
  int noOfMaxPoints = 0;
  int noOfQPoints   = 0;
  int state;
  maxVal[0] = minVal[0] = x[20];
  maxVal[1] = minVal[1] = yFiltered[20];
  maxVal[2] = minVal[2] = 20;

  // Find the actual minimum and maximum values, always skip first and last 20 samples
  for(int i = 20; i < (yFiltered.size() / 4) - 20; i++) {
      if(yFiltered[i] < minVal[1]) {
          minVal[0] = x[i];
          minVal[1] = yFiltered[i];
          minVal[2] = i;
      }
      if(yFiltered[i] > maxVal[1]) {
          maxVal[0] = x[i];
          maxVal[1] = yFiltered[i];
          minVal[2] = i;
      }
  }
  qDebug()<<"minVal: xVal "<<minVal[0]<<" yVal "<<minVal[1]<<"\n";
  qDebug()<<"maxVal: xVal "<<maxVal[0]<<" yVal "<<maxVal[1]<<"\n";

#if 1
  for(int i = 20; i < (yFiltered.size() / 4) - 20; i++) {
    //If this is close to the single minimum value
    if(absDif(yFiltered[i], minVal[1]) < 0.2) {
      state = 0;
      // Search if this is already in our  MinPoints vector
      for(int j = 0; j < noOfMinPoints; j++) {
        if(absDif(minPoints[0][j], x[i]) < 0.2) {
          if(yFiltered[i] < minPoints[1][j]) {
            minPoints[1][j] = yFiltered[i];
            minPoints[0][j] = x[i];
            minPoints[2][j] = i;
          }
          state = 1;
          break; // exit out of the first for loop..
        }
      }
      // If this is not one we know of
      if(state == 0) {
        // Add a new entry
        minPoints[0][noOfMinPoints] = x[i];
        minPoints[1][noOfMinPoints] = yFiltered[i];
        minPoints[2][noOfMinPoints] = i;
        noOfMinPoints++;
      }
    }

    //Repeat for max
    if(absDif(yFiltered[i], maxVal[1]) < 0.2) {
      state = 0;
      // Search if this is already in our  MinPoints vector
      for(int j = 0; j < noOfMaxPoints; j++) {
        if(absDif(maxPoints[0][j], x[i]) < 0.2) {
          if(yFiltered[i] > maxPoints[1][j]) {
            maxPoints[1][j] = yFiltered[i];
            maxPoints[0][j] = x[i];
            maxPoints[2][j] = i;
          }
          state = 1;
          break; // exit out of the first for loop..
        }
      }
      // If this is not one we know of
      if(state == 0) {
        // Add a new entry
        maxPoints[0][noOfMaxPoints] = x[i];
        maxPoints[1][noOfMaxPoints] = yFiltered[i];
        maxPoints[2][noOfMaxPoints] = i;
        noOfMaxPoints++;
      }
    }
  }
  // Search  for  Q waves, starting from R waves and going backwards to find the near minimum
  // TODO: fix this, it will not work if the sampling starts from the R wave  and the Q wave is not present
  // Take care to avoid this
  for(int i = 0; i < noOfMaxPoints; i++) {
    qPoints[0][i] = maxPoints[0][i];
    qPoints[1][i] = maxPoints[1][i];
    for(int j = maxPoints[2][i]; (maxPoints[0][i] - x[j]) < 0.12 ; j--) {
      if(qPoints[1][i] > yFiltered[j]) {
        qPoints[0][i] = x[j];
        qPoints[1][i] = yFiltered[j];
        qPoints[2][i] = j;
      }
    }
  }
  noOfQPoints = noOfMaxPoints;
#endif
  // Print the found values and check
  qDebug() << "Number of S (min) points " << noOfMinPoints << "\n";
  for(int i = 0; i < noOfMinPoints; i++)
      qDebug() << "Point " << i << " xVal " << minPoints[0][i] << " yVal " << minPoints[1][i] << "\n";

  qDebug() << "Number of R (max) points " << noOfMaxPoints << "\n";
  for(int i = 0; i < noOfMaxPoints; i++)
      qDebug() << "Point " << i << " xVal " << maxPoints[0][i] << " yVal " << maxPoints[1][i] << "\n";

  qDebug() << "Number of Q points " << noOfQPoints << "\n";
  for(int i = 0; i < noOfQPoints; i++)
      qDebug() << "Point " << i << " xVal " << qPoints[0][i] << " yVal " << qPoints[1][i] << "\n";

  // Calculate heart rate with R wave
  float intervalAvg = 0;
  for(int i = 0; i < noOfMaxPoints - 1; i++)
    intervalAvg += (maxPoints[0][i + 1] - maxPoints[0][i]);
  intervalAvg /= (noOfMaxPoints - 1);
  qDebug() << "Heart beat interval average (R wave)" << intervalAvg << "\n";
  qDebug() << "Heart rate (R wave)" << (1/intervalAvg) * 60 << "BPM\n";
  //  Calculate heart rate with S wave
  intervalAvg = 0;
  for(int i = 0; i < noOfMinPoints - 1; i++)
    intervalAvg += (minPoints[0][i + 1] - minPoints[0][i]);
  intervalAvg /= (noOfMinPoints - 1);
  qDebug() << "Heart beat interval average (S wave)" << intervalAvg << "\n";
  qDebug() << "Heart rate (S wave)" << (1/intervalAvg) * 60 << "BPM\n";
  // Calculate heart rate with Q wave
  intervalAvg = 0;
  for(int i = 0; i < noOfQPoints - 1; i++)
    intervalAvg += (qPoints[0][i + 1] - qPoints[0][i]);
  intervalAvg /= (noOfQPoints - 1);
  qDebug() << "Heart beat interval average (Q wave)" << intervalAvg << "\n";
  qDebug() << "Heart rate (Q wave)" << (1/intervalAvg) * 60 << "BPM\n";

  // create graph and assign data to it:
#if 0
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
  customPlot->yAxis->setRange(1.5, -12);
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
