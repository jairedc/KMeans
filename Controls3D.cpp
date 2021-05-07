#include "Controls3D.h"
#include "ui_Controls3D.h"

Controls3D::Controls3D(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Controls3D)
{
  ui->setupUi(this);

  connect(ui->down,   &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->up,     &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->left,   &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->right,  &QPushButton::clicked, this, &Controls3D::buttonClicked);
  connect(ui->rotate, &QPushButton::clicked, this, &Controls3D::buttonClicked);
}

Controls3D::~Controls3D()
{
  delete ui;
}

void Controls3D::buttonClicked()
{
  QPushButton* button = (QPushButton*)sender();
  if (button->objectName() == "rotate")
    emit rotateClicked();
  else
    emit directionClicked(button->objectName(), ui->step->value());
}

void Controls3D::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Left)
    emit directionClicked("left", ui->step->value());
  else if (event->key() == Qt::Key_Right)
    emit directionClicked("right", ui->step->value());
  else if (event->key() == Qt::Key_Down)
    emit directionClicked("down", ui->step->value());
  else if (event->key() == Qt::Key_Up)
    emit directionClicked("up", ui->step->value());
}

void Controls3D::mousePressEvent(QMouseEvent *event)
{
  setFocus();
}
