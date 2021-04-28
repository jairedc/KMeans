#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <RandomData.h>
#include <kmeans.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

public slots:


private:
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
