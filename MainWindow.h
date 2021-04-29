#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <RandomData.h>
#include <kmeans.h>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef std::uniform_real_distribution<double> uDistd;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void SetSignals();
  void GenerateData();

  QVector<double> xData;
  QVector<double> yData;

protected:
  void showEvent(QShowEvent* event);

public slots:


private:
  Ui::MainWindow *ui;
  RandomData* rndG;
};
#endif // MAINWINDOW_H
