#ifndef KMEANS_CPP
#define KMEANS_CPP

#include "kmeans.h"

template <class T>
kmeans<T>::kmeans(int k, quint32 maxIterations)
{
  maxIterations_ = maxIterations;
  k_ = k;
  centroids_.resize(k_);

  rand_ = QRandomGenerator::global();
}

template <class T>
kmeans<T>::kmeans(int k, QVector<T> data, quint32 maxIterations)
{
  maxIterations_ = maxIterations;
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

template <class T>
void kmeans<T>::operator()(std::function<double(T, T)> d)
{
  // Random assignment of centroids to data
  std::generate(centroids_.begin(), centroids_.end(),
                [this]() { return data_[rand_->bounded(data_.size())]; });

  // Assign cluster centers
  for (quint32 n = 0; n < maxIterations_; n++)
  {
    T newCentroids[k_] = {};
    quint32 cCount[k_] = {};

    double currentD, minD, assignedC;
    for (quint32 p = 0; p < data_.size(); p++)
    {
      minD = d(data_[p], centroids_[0]);
      assignedC = 0;
      for (quint32 c = 1; c < centroids_.size(); c++)
      {
        currentD = d(data_[p], centroids_[0]);
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
    for (quint32 i = 0; i < k_; i++)
      centroids_[i] = newCentroids[i] / cCount[i];
  }
}

template <class T>
kmeans<T>::~kmeans()
{
  delete rand_;
}

#endif
