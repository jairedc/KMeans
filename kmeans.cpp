#ifndef KMEANS_CPP
#define KMEANS_CPP

#include "kmeans.h"

template <class T>
kmeans<T>::kmeans(int k, quint32 maxIterations)
{
  initialized_ = false;
  maxIterations_ = maxIterations;
  currIteration_ = 0;
  k_ = k;
  centroids_.resize(k_);

  rand_ = QRandomGenerator::global();
}

template <class T>
kmeans<T>::kmeans(int k, QVector<T> data, quint32 maxIterations)
{
  initialized_ = false;
  maxIterations_ = maxIterations;
  currIteration_ = 0;
  k_ = k;
  data_ = data;

  centroids_.resize(k_);
  assignments_.resize(data_.size());
  rand_ = QRandomGenerator::global();
}

template <class T>
void kmeans<T>::setData(QVector<T> data)
{
  data_ = data;
}

template<class T>
void kmeans<T>::setK(int k)
{
  k_ = k;
  initialized_ = false;
  reset();
}

template <class T>
void kmeans<T>::step(std::function<double(T, T)> d)
{
  // Random assignment of centroids to data
  if (!initialized_)
  {
    initialized_ = true;
    std::generate(centroids_.begin(), centroids_.end(),
                [this]() { return data_[rand_->bounded(data_.size())]; });
  }
  if (currIteration_ >= maxIterations_)
    return;

  // Assign cluster centers
  T newCentroids[k_] = {};
  quint32 cCount[k_] = {};

  qint32 assignedC;
  double currentD, minD;
  for (qint32 p = 0; p < data_.size(); p++)
  {
    minD = d(data_[p], centroids_[0]);
    assignedC = 0;
    for (qint32 c = 1; c < centroids_.size(); c++)
    {
      currentD = d(data_[p], centroids_[c]);
      if (currentD < minD)
      {
        currentD = minD;
        assignedC = c;
      }
    }
    assignments_[p] = assignedC;

    newCentroids[assignedC] += data_[p];
    cCount[assignedC]++;
  }

  // Calculate new cluster centers
  for (qint32 i = 0; i < k_; i++)
    centroids_[i] = newCentroids[i] / cCount[i];
}

template <class T>
void kmeans<T>::step(std::function<double(T, T)> d, int steps)
{
  for (int i = 0; i < steps; i++) step(d);
}

template <class T>
void kmeans<T>::finish(std::function<double(T, T)> d)
{
  qint32 finishCount = maxIterations_ - currIteration_;
  for (qint32 i = 0; i < finishCount; i++)
    step(d);
}

template<class T>
void kmeans<T>::reset()
{
  centroids_.resize(k_);
  initialized_ = false;
}

template<class T>
int kmeans<T>::k() const
{
  return k_;
}

template<class T>
QVector<T> &kmeans<T>::centroids()
{
  return centroids_;
}

template<class T>
QVector<quint32> &kmeans<T>::assignments()
{
  return assignments_;
}

template <class T>
kmeans<T>::~kmeans()
{
  delete rand_;
}

#endif
