#ifndef KMEANS_H
#define KMEANS_H

#include <QVector>
#include <QRandomGenerator>
#include <functional>


template <class T>
class kmeans
{
public:
  kmeans(int k, quint32 maxIterations = 1000);
  kmeans(int k, QVector<T> data, quint32 maxIterations = 1000);

  void setData(QVector<T> data);
  void setK(int k);

  void step(std::function<double(T, T)> d);
  void step(std::function<double(T, T)> d, int steps);
  void finish(std::function<double(T, T)> d);
  void reset();

  int k() const;
  QVector<T>& centroids();
  QVector<quint32>& assignments();

  ~kmeans();

private:
  bool initialized_;
  int maxIterations_;
  int currIteration_;
  int k_;
  QVector<T> data_;
  QVector<T> centroids_;
  QVector<quint32> assignments_;

  QRandomGenerator* rand_;
};

#include "kmeans.cpp"

#endif // KMEANS_H
