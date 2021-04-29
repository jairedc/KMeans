#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->plot->xAxis->setRange(-10.0, 10.0);
  ui->plot->yAxis->setRange(-10.0, 10.0);

  QPen originPen(Qt::gray, 1);
  ui->plot->xAxis->grid()->setZeroLinePen(originPen);
  ui->plot->yAxis->grid()->setZeroLinePen(originPen);

  SetSignals();
}

void MainWindow::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  ui->xMinSpinBox->selectAll();
  ui->xMinSpinBox->setFocus();
}

void MainWindow::SetSignals()
{
  connect(ui->createDataButton, &QPushButton::clicked,
          this, &MainWindow::GenerateData);
}

void MainWindow::GenerateData()
{
  std::random_device rd;
  std::mt19937_64 gen(rd());
  uDistd xDist(ui->xMinSpinBox->value(), ui->xMaxSpinBox->value());
  uDistd yDist(ui->yMinSpinBox->value(), ui->yMaxSpinBox->value());

  xData = RandomData::Generate(xDist, gen, ui->nDataSpinBox->value());
  yData = RandomData::Generate(yDist, gen, ui->nDataSpinBox->value());

  if (ui->plot->graphCount() == 0)
    ui->plot->addGraph();
  QCPGraph* g = ui->plot->graph(0);
  g->setData(xData, yData);
  g->setLineStyle(QCPGraph::lsNone);
  g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
  ui->plot->replot();
}

MainWindow::~MainWindow()
{
  delete ui;
  delete rndG;
}

