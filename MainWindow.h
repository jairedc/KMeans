#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPair>
#include <RandomData.h>
#include <kmeans.h>
#include <random>
#include <iostream>
#include <QDebug>
#include <QErrorMessage>
#include <QtMath>
#include <QGradient>
#include <QPen>
#include <QTimer>
#include <QtGlobal>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef std::uniform_real_distribution<double> uDistd;
typedef QPair<double, double> QPair_d;
typedef QVector<QPair<QVector<double>, QVector<double>>> PairBuckets;

struct Pair2D
{
  QPair_d pair_;

  Pair2D() {}
  Pair2D(double x, double y) { pair_ = QPair_d(x, y); }

  double operator[](const bool& i) const
  {
    // false == 0, true == 1
    if (i) return pair_.second;
    else return pair_.first;
  }

  Pair2D operator+(const Pair2D& rhs)
  {
    Pair2D sum(pair_.first + rhs[0], pair_.second + rhs[1]);
    return sum;
  }

  Pair2D& operator+=(const Pair2D& rhs)
  {
    pair_.first += rhs[0];
    pair_.second += rhs[1];
    return *this;
  }

  Pair2D operator/(const quint32& scalar)
  {
    Pair2D quotient(pair_.first / scalar, pair_.second / scalar);
    return quotient;
  }

  static double EuclideanDistance(Pair2D lhs, Pair2D rhs)
  {
    return qSqrt(qPow(lhs[0] - rhs[0], 2) + qPow(lhs[1] - rhs[1], 2));
  }

  static double L1Distance(Pair2D lhs, Pair2D rhs)
  {
    return qAbs(lhs[0] - rhs[0]) + qAbs(lhs[1] - rhs[1]);
  }
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void SetSignals();
  void GenerateData();
  void SetGridBounds(double xMin, double xMax, double yMin, double yMax);
  void Step();
  void Reset();
  void SetPairVector(QVector<double> x, QVector<double> y);
  void Set2DGraphData();
  void SetColorVector(int k);
  void PlaySteps();
  void StopPlaying();
  void EnableControls(bool state);
  void ImportData();
  void Parse2D(QTextStream& in);
  void DefaultPlot();
  PairBuckets GetPairBuckets(QVector<quint32>& assignments);

  QVector<double> xData_;
  QVector<double> yData_;
  QVector<Pair2D> pairs_;
  double minx_, miny_, maxx_, maxy_;

protected:
  void showEvent(QShowEvent* event);

public slots:
  void ChangePlayTimeout(int timeout);
//  void ChangeK(int k);

private:
  bool kmeansExecuting = false;

  Ui::MainWindow *ui;
  RandomData* rndG_;
  QErrorMessage* eMsg_;
  kmeans<Pair2D>* kmeans_alg_;
  QVector<QColor>* colors_;
  QTimer* timer_;
};
#endif // MAINWINDOW_H
