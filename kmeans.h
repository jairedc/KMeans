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

  void operator()(std::function<double(T, T)> d);

  ~kmeans();

private:
  int maxIterations_;
  int k_;
  QVector<T> data_;
  QVector<T> centroids_;
  QVector<quint32> assignments_;

  QRandomGenerator* rand_;
};

#include "kmeans.cpp"

#endif // KMEANS_H
