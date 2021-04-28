#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->xMinSpinBox->selectAll();

  ui->plot->xAxis->setRange(-10.0, 10.0);
  ui->plot->yAxis->setRange(-10.0, 10.0);

  QPen originPen(Qt::gray, 1);
  ui->plot->xAxis->grid()->setZeroLinePen(originPen);
  ui->plot->yAxis->grid()->setZeroLinePen(originPen);

  SetSignals();
}

void MainWindow::SetSignals()
{
  connect(ui->createDataButton, &QPushButton::clicked,
          this, &MainWindow::GenerateData);
}

void MainWindow::GenerateData()
{
  RandomData xRandomGenerator(ui->nDataSpinBox->value(),
               (double)ui->xMinSpinBox->value(), (double)ui->xMaxSpinBox->value());
  RandomData yRandomGenerator(ui->nDataSpinBox->value(),
               (double)ui->yMinSpinBox->value(), (double)ui->yMaxSpinBox->value());

  xData = xRandomGenerator.Generate();
  yData = yRandomGenerator.Generate();

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
}

