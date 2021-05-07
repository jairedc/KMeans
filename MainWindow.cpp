#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  infoDialog_ = new Info(this);
  infoDialog_->ChangeInfo(0, 0.0);

  controls3DDialog_ = new Controls3D(this);

  eMsg_ = new QErrorMessage(this);
  eMsg_->setWindowModality(Qt::WindowModal);

  kmeans_alg_ = nullptr;
  colors_ = nullptr;
  timer_ = new QTimer(this);
  timer_->callOnTimeout(this, &MainWindow::Step);

  kmeansExecuting_ = false;
  playing_ = false;
  step_ = 0;

  pointStyle_.setShape(QCPScatterStyle::ssDisc);
  pointStyle_.setSize(4);
  centroidStyle_.setShape(QCPScatterStyle::ssCrossCircle);
  centroidStyle_.setSize(20);

  SetSignals();
  SwitchTo3D();
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
  ui->plot->setInteraction(QCP::iRangeDrag, true);
  ui->plot->setInteraction(QCP::iRangeZoom, true);

  // Cross circle
  ui->centroidShapeComboBox->setCurrentIndex(11);

  ui->stopButton->setEnabled(false);
  ui->backOneButton->setEnabled(false);
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
  connect(ui->pointSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &MainWindow::PointSizeChanged);
  connect(ui->importAction, &QAction::triggered,
          this, &MainWindow::ImportData);
  connect(ui->infoAction, &QAction::triggered,
          this, &MainWindow::ShowInfoDialog);
  connect(ui->pointShapeComboBox, &QComboBox::currentTextChanged,
          this, &MainWindow::PointShapeChanged);
  connect(ui->centroidShapeComboBox, &QComboBox::currentTextChanged,
          this, &MainWindow::CentroidShapeChanged);
  connect(ui->backOneButton, &QPushButton::clicked,
          this, &MainWindow::GoBackwardOneStep);
  connect(ui->switch2DAction, &QAction::triggered,
          this, &MainWindow::SwitchTo2D);
  connect(ui->switch3DAction, &QAction::triggered,
          this, &MainWindow::SwitchTo3D);
  connect(ui->controls3DAction, &QAction::triggered,
          this, &MainWindow::Show3DControls);
  connect(controls3DDialog_, &Controls3D::directionClicked,
          this, &MainWindow::Change3DEye);
}

void MainWindow::PlaySteps()
{
  ui->stepButton->setEnabled(false);
  ui->playButton->setEnabled(false);
  ui->stopButton->setEnabled(true);
  ui->resetButton->setEnabled(false);
  ui->backOneButton->setEnabled(false);
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
  ui->backOneButton->setEnabled(true);
  ui->resetButton->setEnabled(true);
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
  if (mode_ == Mode::TwoD)
    Import2D();
  else if (mode_ == Mode::ThreeD)
    Import3D();


}

void MainWindow::Import2D()
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
    Set2DPairVector(xData_, yData_);
  }
}

void MainWindow::Import3D()
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

    if (dimText.toInt() == 3) Parse3D(in);

    file.close();
    // TODO: set 3D bounds and other information
    Set3DPairVector(xData_, yData_, zData_);
    ui->viewWidget->setPoints(xData_, yData_, zData_);
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

void MainWindow::Parse3D(QTextStream &in)
{
  QString line = in.readLine();
  QStringList data = line.split(' ');

  maxx_ = data[0].toDouble();
  minx_ = data[0].toDouble();
  xData_.append(minx_);

  maxy_ = data[1].toDouble();
  miny_ = data[1].toDouble();
  yData_.append(miny_);

  maxz_ = data[2].toDouble();
  minz_ = data[2].toDouble();
  zData_.append(minz_);

  double x, y, z;
  while (!in.atEnd())
  {
    QString line = in.readLine();
    QStringList data = line.split(' ');

    x = data[0].toDouble();
    y = data[1].toDouble();
    z = data[2].toDouble();
    xData_.append(x);
    yData_.append(y);
    zData_.append(z);

    if (x > maxx_) maxx_ = x;
    if (y > maxy_) maxy_ = y;
    if (z > maxz_) maxz_ = z;
    if (x < minx_) minx_ = x;
    if (y < miny_) miny_ = y;
    if (z < minz_) minz_ = z;
  }
}

void MainWindow::Zoom3D()
{

}

void MainWindow::DefaultPlot()
{
  ui->plot->clearGraphs();
  if (ui->plot->graphCount() == 0)
    ui->plot->addGraph();
  QCPGraph* g = ui->plot->graph(0);
  g->setData(xData_, yData_);
  g->setLineStyle(QCPGraph::lsNone);
  g->setScatterStyle(pointStyle_);
  ui->plot->replot();
}

void MainWindow::GenerateData()
{
  if (mode_ == Mode::TwoD)
    Generate2D();
  else if (mode_ == Mode::ThreeD)
    Generate3D();
}

void MainWindow::Generate2D()
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  uDistd xDist(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value());
  uDistd yDist(ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());

  xData_ = RandomData::Generate(xDist, gen, ui->nDataSpinBox->value());
  yData_ = RandomData::Generate(yDist, gen, ui->nDataSpinBox->value());

  Set2DPairVector(xData_, yData_);

  SetGridBounds(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value(),
                ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());

  DefaultPlot();
}

void MainWindow::Generate3D()
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  uDistd xDist(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value());
  uDistd yDist(ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());
  uDistd zDist(ui->zMinSpinBox->value(), ui->zMaxSpinBox->value());

  xData_ = RandomData::Generate(xDist, gen, ui->nDataSpinBox->value());
  yData_ = RandomData::Generate(yDist, gen, ui->nDataSpinBox->value());
  zData_ = RandomData::Generate(zDist, gen, ui->nDataSpinBox->value());

  Set3DPairVector(xData_, yData_, zData_);

  // TODO: set 3D bounds
  ui->viewWidget->setPoints(xData_, yData_, zData_);
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
    bool degenerate = false;
    int k = ui->kSpinBox->value();
    if (kmeans_alg_ == nullptr)
      kmeans_alg_ = new kmeans<Pair2D>(k, pairs_);

    if (!kmeansExecuting_)
    {
      degenerate = CheckDegenerateCases();
      if (!degenerate)
      {
        kmeansExecuting_ = true;
        kmeans_alg_->setK(k);
        SetColorVector(k);
        EnableControls(false);

        if (ui->initComboBox->currentText() == "Random")
        {
          QVector<Pair2D> randomCentroids = Pair2D::MakeRandomPairs(k,
                                FindXMin(), FindXMax(), FindYMin(), FindYMax());
          kmeans_alg_->setInitialization(InitializeType::Random);
          kmeans_alg_->setRandomCentroids(randomCentroids);
        }
        if (ui->initComboBox->currentText() == "Kpp")
          kmeans_alg_->setInitialization(InitializeType::Kpp);
        if (ui->initComboBox->currentText() == "Sample")
          kmeans_alg_->setInitialization(InitializeType::Sample);
      }
    }
    if (!degenerate)
    {
      std::function<double(Pair2D, Pair2D)> distF;
      if (ui->distanceFComboBox->currentText() == "L1")
        distF = Pair2D::L1Distance;
      else
        distF = Pair2D::EuclideanDistance;

      if (step_)
      {
        CopyLastStep();
        if (!playing_)
          ui->backOneButton->setEnabled(true);
      }

      int stepValue = ui->stepSpinBox->value();
      if (stepValue == 1)
        kmeans_alg_->step(distF);
      else
        kmeans_alg_->step(distF, stepValue);
      step_++;
      Set2DGraphData();
      infoDialog_->ChangeInfo(step_, kmeans_alg_->getEnergy());
    }
  }
}

void MainWindow::GoBackwardOneStep()
{
  ui->backOneButton->setEnabled(false);
  PairBuckets assignedPairs = GetPairBuckets(assignmentsBackward_);

  DrawData(centroidsBackward_, assignedPairs);
  kmeans_alg_->centroids() = centroidsBackward_;
}

void MainWindow::CopyLastStep()
{
  if (step_)
  {
    centroidsBackward_ = kmeans_alg_->centroids();
    assignmentsBackward_ = kmeans_alg_->assignments();
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
  ui->backOneButton->setEnabled(false);
  infoDialog_->ChangeInfo(0, 0.0);
  step_ = 0;
}

void MainWindow::PointSizeChanged(int size)
{
  pointStyle_.setSize(size);
  centroidStyle_.setSize(size + 16);
  if (!playing_)
  {
    if(kmeansExecuting_)
      Set2DGraphData();
    else
      DefaultPlot();
  }
}

void MainWindow::PointShapeChanged(QString text)
{
  pointStyle_.setShape(GetStyleFromString(text));
  if (!playing_)
  {
    if(kmeansExecuting_)
      Set2DGraphData();
    else
      DefaultPlot();
  }
}

void MainWindow::CentroidShapeChanged(QString text)
{
  centroidStyle_.setShape(GetStyleFromString(text));
  if (!playing_)
  {
    if(kmeansExecuting_)
      Set2DGraphData();
    else
      DefaultPlot();
  }
}

void MainWindow::Set2DPairVector(QVector<double> x, QVector<double> y)
{
  pairs_.clear();
  for (int i = 0; i < x.size(); i++)
    pairs_.append(Pair2D(x[i], y[i]));
}

void MainWindow::Set3DPairVector(QVector<double> x, QVector<double> y,
                                 QVector<double> z)
{
  pairs3D_.clear();
  for (int i = 0; i < x.size(); i++)
    pairs3D_.append(Pair3D(x[i], y[i], z[i]));
}

void MainWindow::Set2DGraphData()
{
  QVector<Pair2D>& centroids = kmeans_alg_->centroids();
  QVector<quint32>& assignments = kmeans_alg_->assignments();
  PairBuckets assignedPairs = GetPairBuckets(assignments);

  DrawData(centroids, assignedPairs);
}

void MainWindow::DrawData(QVector<Pair2D> &centroids, PairBuckets &assignedPairs)
{
  int k = kmeans_alg_->k();
  QCustomPlot* plot = ui->plot;
  ui->plot->clearGraphs();

  for (int i = 0; i < k; i++)
  {
    plot->addGraph();
    QCPGraph* g = plot->graph(i);
    g->setData(assignedPairs[i].first, assignedPairs[i].second);
    g->setLineStyle(QCPGraph::lsNone);
    g->setScatterStyle(pointStyle_);
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
    g->setScatterStyle(centroidStyle_);
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

void MainWindow::ShowInfoDialog()
{
  infoDialog_->show();
}

bool MainWindow::CheckDegenerateCases()
{
  int k = ui->kSpinBox->value();
  int n = pairs_.size();

  if (k == 0 || k == 1)
  {
    eMsg_->showMessage("k must be 2 or greater.");
    return true;
  }
  else if (k > n)
  {
    eMsg_->showMessage("k can't be greater than the number of points.");
    return true;
  }
  else
    return false;
}

void MainWindow::Show3DControls()
{
  if (mode_ == Mode::ThreeD)
    controls3DDialog_->show();
}

void MainWindow::Change3DEye(QString direction)
{
  ViewWidget* vw = ui->viewWidget;
  if (direction == "up")
    vw->moveEye(0.0f, -0.01f, 0.0f);
  else if (direction == "down")
    vw->moveEye(0.0f, 0.01f, 0.0f);
  else if (direction == "left")
    vw->moveEye(-0.01f, 0.0f, 0.0f);
  else if (direction == "right")
    vw->moveEye(0.01f, 0.0f, 0.0f);
}

double MainWindow::FindXMin()
{
  double minX = pairs_[0][0];
  for (int i = 1; i < pairs_.size(); i++)
    if (pairs_[i][0] < minX)
      minX = pairs_[i][0];
  return minX;
}

double MainWindow::FindXMax()
{
  double maxX = pairs_[0][0];
  for (int i = 1; i < pairs_.size(); i++)
    if (pairs_[i][0] > maxX)
      maxX = pairs_[i][0];
  return maxX;
}

double MainWindow::FindYMin()
{
  double minY = pairs_[0][1];
  for (int i = 1; i < pairs_.size(); i++)
    if (pairs_[i][1] < minY)
      minY = pairs_[i][1];
  return minY;
}

double MainWindow::FindYMax()
{
  double maxY = pairs_[0][1];
  for (int i = 1; i < pairs_.size(); i++)
    if (pairs_[i][1] > maxY)
      maxY = pairs_[i][1];
  return maxY;
}

QCPScatterStyle::ScatterShape MainWindow::GetStyleFromString(QString text)
{
  text = text.trimmed();
  text = text.toUpper();

  if (text == "DOT")
    return QCPScatterStyle::ssDot;
  if (text == "CROSS")
    return QCPScatterStyle::ssCross;
  else if (text == "PLUS")
    return QCPScatterStyle::ssPlus;
  else if (text == "CIRCLE")
    return QCPScatterStyle::ssCircle;
  else if (text == "DISC")
    return QCPScatterStyle::ssDisc;
  else if (text == "SQUARE")
    return QCPScatterStyle::ssSquare;
  else if (text == "DIAMOND")
    return QCPScatterStyle::ssDiamond;
  else if (text == "STAR")
    return QCPScatterStyle::ssStar;
  else if (text == "TRIANGLE")
    return QCPScatterStyle::ssTriangle;
  else if (text == "INVERTED TRIANGLE")
    return QCPScatterStyle::ssTriangleInverted;
  else if (text == "CROSS SQUARE")
    return QCPScatterStyle::ssCrossSquare;
  else if (text == "PLUS SQUARE")
    return QCPScatterStyle::ssPlusSquare;
  else if (text == "CROSS CIRCLE")
    return QCPScatterStyle::ssCrossCircle;
  else if (text == "PLUS CIRCLE")
    return QCPScatterStyle::ssPlusCircle;
  else if (text == "PEACE")
    return QCPScatterStyle::ssPeace;
  else
    return QCPScatterStyle::ssDisc;
}

MainWindow::~MainWindow()
{
  delete ui;
  delete rndG_;
  delete eMsg_;
  delete kmeans_alg_;
  delete colors_;
  delete infoDialog_;
  delete controls3DDialog_;
}

void MainWindow::SwitchTo2D()
{
  mode_ = Mode::TwoD;
  ui->viewWidget->hide();
  ui->plot->show();
  ui->switch2DAction->setEnabled(false);
  ui->switch3DAction->setEnabled(true);

  ui->zBoundsLabel->setEnabled(false);
  ui->zMinSpinBox->setEnabled(false);
  ui->zMaxSpinBox->setEnabled(false);
}

void MainWindow::SwitchTo3D()
{
  mode_ = Mode::ThreeD;
  ui->plot->hide();
  ui->viewWidget->show();
  ui->switch2DAction->setEnabled(true);
  ui->switch3DAction->setEnabled(false);

  ui->zBoundsLabel->setEnabled(true);
  ui->zMinSpinBox->setEnabled(true);
  ui->zMaxSpinBox->setEnabled(true);
}
