#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  eMsg_ = new QErrorMessage(this);
  eMsg_->setWindowModality(Qt::WindowModal);

  kmeans_alg_ = nullptr;
  colors_ = nullptr;
  timer_ = new QTimer(this);
  timer_->callOnTimeout(this, &MainWindow::Step);

  kmeansExecuting_ = false;
  playing_ = false;
  SetSignals();
}

void MainWindow::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  ui->xMinSpinBox->selectAll();
  ui->xMinSpinBox->setFocus();

  SetGridBounds(-10.0, 10.0, -10.0, 10.0);

  QPen originPen(Qt::gray, 1);
  ui->plot->xAxis->grid()->setZeroLinePen(originPen);
  ui->plot->yAxis->grid()->setZeroLinePen(originPen);

  ui->stopButton->setEnabled(false);
}

void MainWindow::SetSignals()
{
  connect(ui->createDataButton, &QPushButton::clicked,
          this, &MainWindow::GenerateData);
  connect(ui->stepButton, &QPushButton::clicked,
          this, &MainWindow::Step);
  connect(ui->resetButton, &QPushButton::clicked,
          this, &MainWindow::Reset);
  connect(ui->playButton, &QPushButton::clicked,
          this, &MainWindow::PlaySteps);
  connect(ui->stopButton, &QPushButton::clicked,
          this, &MainWindow::StopPlaying);
  connect(ui->playSpeedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &MainWindow::ChangePlayTimeout);
  connect(ui->importAction, &QAction::triggered,
          this, &MainWindow::ImportData);
}

void MainWindow::PlaySteps()
{
  ui->stepButton->setEnabled(false);
  ui->playButton->setEnabled(false);
  ui->stopButton->setEnabled(true);
  timer_->start(ui->playSpeedSpinBox->value());
  playing_ = true;
}

void MainWindow::ChangePlayTimeout(int timeout)
{
  timer_->stop();
  if (playing_)
    timer_->start(timeout);
}

void MainWindow::StopPlaying()
{
  timer_->stop();
  playing_ = false;
  ui->playButton->setEnabled(true);
  ui->stepButton->setEnabled(true);
  ui->stopButton->setEnabled(false);
}

void MainWindow::EnableControls(bool state)
{
  ui->nDataSpinBox->setEnabled(state);
  ui->xMinSpinBox->setEnabled(state);
  ui->xMaxSpinBox->setEnabled(state);
  ui->yMinSpinBox->setEnabled(state);
  ui->yMaxSpinBox->setEnabled(state);
  ui->createDataButton->setEnabled(state);
  ui->kSpinBox->setEnabled(state);
  ui->distanceFComboBox->setEnabled(state);
  ui->initComboBox->setEnabled(state);
}

void MainWindow::ImportData()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open Data File",
                                                  "/home", tr("*.txt"));
  qDebug() << "Filename: " << filename;
  if (filename.isEmpty())
    eMsg_->showMessage("Empty filename, data not set.");
  else
  {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      eMsg_->showMessage("Unable to open data file.");
      return;
    }

    QTextStream in(&file);

    QString nText = in.readLine();
    QString dimText = in.readLine();

    if (dimText.toInt() == 2) Parse2D(in);

    file.close();
    SetGridBounds(minx_, maxx_, miny_, maxy_);
    DefaultPlot();
    SetPairVector(xData_, yData_);
  }
}

void MainWindow::Parse2D(QTextStream& in)
{
  QString line = in.readLine();
  QStringList data = line.split(' ');

  maxx_ = data[0].toDouble();
  minx_ = data[0].toDouble();
  xData_.append(minx_);

  maxy_ = data[1].toDouble();
  miny_ = data[1].toDouble();
  yData_.append(miny_);

  double x, y;
  while (!in.atEnd())
  {
    QString line = in.readLine();
    QStringList data = line.split(' ');

    x = data[0].toDouble();
    y = data[1].toDouble();
    xData_.append(x);
    yData_.append(y);

    if (x > maxx_) maxx_ = x;
    if (y > maxy_) maxy_ = y;
    if (x < minx_) minx_ = x;
    if (y < miny_) miny_ = y;
  }
}

void MainWindow::DefaultPlot()
{
  ui->plot->clearGraphs();
  if (ui->plot->graphCount() == 0)
    ui->plot->addGraph();
  QCPGraph* g = ui->plot->graph(0);
  g->setData(xData_, yData_);
  g->setLineStyle(QCPGraph::lsNone);
  g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
  ui->plot->replot();
}

void MainWindow::GenerateData()
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  uDistd xDist(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value());
  uDistd yDist(ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());

  xData_ = RandomData::Generate(xDist, gen, ui->nDataSpinBox->value());
  yData_ = RandomData::Generate(yDist, gen, ui->nDataSpinBox->value());

  SetPairVector(xData_, yData_);

  SetGridBounds(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value(),
                ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());

  DefaultPlot();
}

void MainWindow::SetGridBounds(double xMin, double xMax,
                               double yMin, double yMax)
{
  ui->plot->xAxis->setRange(xMin, xMax);
  ui->plot->yAxis->setRange(yMin, yMax);
}

void MainWindow::Step()
{
  if (pairs_.size() == 0)
  {
    eMsg_->showMessage("Data not initialized. Can't perform kmeans.");
  }
  else
  {
    int k = ui->kSpinBox->value();
    if (kmeans_alg_ == nullptr)
      kmeans_alg_ = new kmeans<Pair2D>(k, pairs_);

    if (!kmeansExecuting_)
    {
      kmeansExecuting_ = true;
      kmeans_alg_->setK(k);
      SetColorVector(k);
      EnableControls(false);

      if (ui->initComboBox->currentText() == "Random")
        kmeans_alg_->setInitialization(InitializeType::Random);
      if (ui->initComboBox->currentText() == "Kpp")
        kmeans_alg_->setInitialization(InitializeType::Kpp);
      if (ui->initComboBox->currentText() == "Sample")
        kmeans_alg_->setInitialization(InitializeType::Sample);
    }
    std::function<double(Pair2D, Pair2D)> distF;
    if (ui->distanceFComboBox->currentText() == "L1")
      distF = Pair2D::L1Distance;
    else
      distF = Pair2D::EuclideanDistance;

    int stepValue = ui->stepSpinBox->value();
    if (stepValue == 1)
      kmeans_alg_->step(distF);
    else
      kmeans_alg_->step(distF, stepValue);
    Set2DGraphData();
  }
}

void MainWindow::Reset()
{
  timer_->stop();
  DefaultPlot();
  kmeansExecuting_ = false;
  playing_ = false;
  EnableControls(true);
  ui->stopButton->setEnabled(false);
}

void MainWindow::SetPairVector(QVector<double> x, QVector<double> y)
{
  pairs_.clear();
  for (int i = 0; i < x.size(); i++)
    pairs_.append(Pair2D(x[i], y[i]));
}

void MainWindow::Set2DGraphData()
{
  int k = kmeans_alg_->k();
  QCustomPlot* plot = ui->plot;
  QVector<Pair2D>& centroids = kmeans_alg_->centroids();
  QVector<quint32>& assignments = kmeans_alg_->assignments();
  PairBuckets assignedPairs = GetPairBuckets(assignments);

  ui->plot->clearGraphs();

  for (int i = 0; i < k; i++)
  {
    plot->addGraph();
    QCPGraph* g = plot->graph(i);
    g->setData(assignedPairs[i].first, assignedPairs[i].second);
    g->setLineStyle(QCPGraph::lsNone);
    g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    g->setPen(QPen(colors_->at(i)));
  }

  for (int i = 0; i < k; i++)
  {
    plot->addGraph();
    QCPGraph* g = plot->graph(i + k);
    QVector<double> x, y;
    x.append(centroids[i][0]);
    y.append(centroids[i][1]);
    g->setData(x, y);
    g->setLineStyle(QCPGraph::lsNone);
    g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 20));
    g->setPen(QPen(colors_->at(i)));
  }
  plot->replot();
}

void MainWindow::SetColorVector(int k)
{
  colors_ = new QVector<QColor>();

  for (int i = 0; i < k; i++)
    colors_->append(QColor::fromHsvF(fmod(0.618033988749895 * i, 1.0),
                                     1.0, 1.0));
}

PairBuckets MainWindow::GetPairBuckets(QVector<quint32> &assignments)
{
  PairBuckets assignedPairs(kmeans_alg_->k());

  for (int i = 0; i < assignments.size(); i++)
  {
    assignedPairs[assignments[i]].first.append(xData_[i]);
    assignedPairs[assignments[i]].second.append(yData_[i]);
  }

  return assignedPairs;
}

MainWindow::~MainWindow()
{
  delete ui;
  delete rndG_;
  delete eMsg_;
  delete kmeans_alg_;
  delete colors_;
}

