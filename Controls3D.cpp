#include "Controls3D.h"
#include "ui_Controls3D.h"

Controls3D::Controls3D(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Controls3D)
{
  ui->setupUi(this);

  connect(ui->down,  &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->up,    &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->left,  &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->right, &QPushButton::clicked, this, &Controls3D::buttonClicked);
}

Controls3D::~Controls3D()
{
  delete ui;
}

void Controls3D::buttonClicked()
{
  QPushButton* button = (QPushButton*)sender();
  emit directionClicked(button->objectName());
}
