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
#include <qcustomplot.h>
#include <Info.h>
#include <ViewWidget.h>
#include <QWheelEvent>
#include <QWidget>
#include <Controls3D.h>

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

  static QVector<Pair2D> MakeRandomPairs(int size, double minX, double maxX,
                                                   double minY, double maxY)
  {
    QVector<Pair2D> pairs;
    QVector<double> xData, yData;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    uDistd xDist(minX, maxX);
    uDistd yDist(minY, maxY);

    xData = RandomData::Generate(xDist, gen, size);
    yData = RandomData::Generate(yDist, gen, size);

    for (int i = 0; i < size; i++)
      pairs.append(Pair2D(xData[i], yData[i]));

    return pairs;
  }
};

struct Pair3D
{
  QVector3D pair_;

  Pair3D() {}
  Pair3D(double x, double y, double z) { pair_ = QVector3D(x, y, z); }

  double operator[](const bool& i) const
  {
    return pair_[i];
  }

  Pair3D operator+(const Pair3D& rhs)
  {
    Pair3D sum(pair_[0] + rhs[0], pair_[1] + rhs[2], pair_[2] + rhs[2]);
    return sum;
  }

  Pair3D& operator+=(const Pair3D& rhs)
  {
    pair_[0] += rhs[0];
    pair_[1] += rhs[1];
    pair_[2] += rhs[2];
    return *this;
  }

  Pair3D operator/(const quint32& scalar)
  {
    Pair3D quotient(pair_[0] / scalar, pair_[1] / scalar, pair_[2] / scalar);
    return quotient;
  }

  static double EuclideanDistance(Pair3D lhs, Pair3D rhs)
  {
    return qSqrt(qPow(lhs[0] - rhs[0], 2) + qPow(lhs[1] - rhs[1], 2) +
                 qPow(lhs[2] - rhs[2], 2));
  }

  static double L1Distance(Pair3D lhs, Pair3D rhs)
  {
    return qAbs(lhs[0] - rhs[0]) + qAbs(lhs[1] - rhs[1]) +
           qAbs(lhs[2] - rhs[2]);
  }

  static QVector<Pair3D> MakeRandomPairs(int size, double minX, double maxX,
                                                   double minY, double maxY,
                                                   double minZ, double maxZ)
  {
    QVector<Pair3D> pairs;
    QVector<double> xData, yData, zData;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    uDistd xDist(minX, maxX);
    uDistd yDist(minY, maxY);
    uDistd zDist(minZ, maxZ);

    xData = RandomData::Generate(xDist, gen, size);
    yData = RandomData::Generate(yDist, gen, size);
    zData = RandomData::Generate(zDist, gen, size);

    for (int i = 0; i < size; i++)
      pairs.append(Pair3D(xData[i], yData[i], zData[i]));

    return pairs;
  }
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum Mode {TwoD, ThreeD, ND};

  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void SwitchTo2D();
  void SwitchTo3D();

  void SetSignals();
  void GenerateData();
  void Generate2D();
  void Generate3D();
  void SetGridBounds(double xMin, double xMax, double yMin, double yMax);
  void Step();
  void Step2D();
  void Step3D();
  void GoBackwardOneStep();
  void CopyLastStep();
  void Reset();
  void Reset2D();
  void Reset3D();
  void PointSizeChanged(int size);
  void PointShapeChanged(QString text);
  void CentroidShapeChanged(QString text);
  void Set2DPairVector(QVector<double> x, QVector<double> y);
  void Set3DPairVector(QVector<double> x, QVector<double> y, QVector<double> z);
  void Set2DGraphData();
  void Set3DGraphData();
  void DrawData(QVector<Pair2D>& centroids, PairBuckets& assignedPairs);
  void SetColorVector(int k);
  void PlaySteps();
  void StopPlaying();
  void EnableControls(bool state);
  void ImportData();
  void Import2D();
  void Import3D();
  void Parse2D(QTextStream& in);
  void Parse3D(QTextStream& in);
  void Zoom3D();
  void DefaultPlot2D();
  void DefaultPlot3D();
  PairBuckets GetPairBuckets(QVector<quint32>& assignments);
  void ShowInfoDialog();
  bool CheckDegenerateCases();
  void Show3DControls();
  void Rotate3D();
  void Change3DEye(QString direction, float amount);
  double FindXMin();
  double FindXMax();
  double FindYMin();
  double FindYMax();
  double FindZMin();
  double FindZMax();


  QVector<double> xData_;
  QVector<double> yData_;
  QVector<double> zData_;
  QVector<Pair2D> pairs_;
  QVector<Pair3D> pairs3D_;
  double minx_, miny_, maxx_, maxy_, minz_, maxz_;

  static QCPScatterStyle::ScatterShape GetStyleFromString(QString text);
protected:
  void showEvent(QShowEvent* event);

public slots:
  void ChangePlayTimeout(int timeout);
//  void ChangeK(int k);

private:
  bool kmeansExecuting_ = false;
  bool playing_ = false;

  Mode mode_;

  Ui::MainWindow *ui;
  Info* infoDialog_;
  RandomData* rndG_;
  QErrorMessage* eMsg_;
  kmeans<Pair2D>* kmeans_alg_;
  kmeans<Pair3D>* kmeans_alg3D_;
  QVector<QColor>* colors_;
  QTimer* timer_;
  Controls3D* controls3DDialog_;

  QCPScatterStyle pointStyle_, centroidStyle_;
  QVector<Pair2D> centroidsBackward_;
  QVector<Pair3D> centroids3DBackward_;
  QVector<quint32> assignmentsBackward_;
  ulong step_;
};
#endif // MAINWINDOW_H

