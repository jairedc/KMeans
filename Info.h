#ifndef INFO_H
#define INFO_H

#include <QDialog>

namespace Ui {
class Info;
}

class Info : public QDialog
{
  Q_OBJECT

public:
  explicit Info(QWidget *parent = nullptr);
  ~Info();
  void ChangeInfo(qint32 step, double energy, QString reason = "");

private:
  Ui::Info *ui;
};

#endif // INFO_H
