#include "Info.h"
#include "ui_Info.h"

Info::Info(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Info)
{
  ui->setupUi(this);
}

Info::~Info()
{
  delete ui;
}

void Info::ChangeInfo(qint32 step, double energy)
{
  ui->stepDisplay->setText(QString::number(step));
  ui->energyDisplay->setText(QString::number(energy));
}
