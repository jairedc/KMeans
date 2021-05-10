#ifndef KMEANS_H
#define KMEANS_H

#include <QVector>
#include <QRandomGenerator>
#include <functional>
#include <random>
#include <QString>

enum InitializeType {Random, Sample, Kpp};

template <class T>
class kmeans
{
public:
  kmeans(int k, quint32 maxIterations = 1000);
  kmeans(int k, QVector<T> data, quint32 maxIterations = 1000);

  void setData(QVector<T> data);
  void setK(int k);
  void setInitialization(InitializeType type);
  double getEnergy() { return energy_; };
  void setRandomCentroids(QVector<T> centroids);
  void setIgnoreSameAssignments(bool flag);

  bool step(std::function<double(T, T)> d);
  bool step(std::function<double(T, T)> d, int steps);
  bool finish(std::function<double(T, T)> d);
  void reset();

  QString stopReason;

  int k() const;
  QVector<T>& centroids();
  QVector<quint32>& assignments();

  ~kmeans();

private:
  InitializeType initType_;
  double energy_;
  bool initialized_, randomCentroidsInitialized_, ignoreSame_;
  int maxIterations_;
  int currIteration_;
  int k_;
  QVector<T> data_;
  QVector<T> centroids_;
  QVector<quint32> assignments_;

  QRandomGenerator* rand_;

  bool initialize(std::function<double(T, T)> d);
  bool checkRandomCentroids();
  bool initializeSample();
  bool initializeKpp(std::function<double(T, T)> d);
};

#include "kmeans.cpp"

#endif // KMEANS_H
