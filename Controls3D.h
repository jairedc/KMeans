#ifndef CONTROLS3D_H
#define CONTROLS3D_H

#include <QDialog>
#include <QString>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

namespace Ui {
class Controls3D;
}

class Controls3D : public QDialog
{
  Q_OBJECT

public:
  explicit Controls3D(QWidget *parent = nullptr);
  ~Controls3D();

public slots:
  void buttonClicked();

signals:
  void directionClicked(QString button, float amount);
  void rotateClicked();

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  Ui::Controls3D *ui;
};

#endif // CONTROLS3D_H
