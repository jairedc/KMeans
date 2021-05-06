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
  initType_ = InitializeType::Sample;
  energy_ = 0.0;
//  minX_ = -10.0;
//  maxX_ = 10.0;
//  minY_ = -10.0;
//  maxY_ = 10.0;

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
  initType_ = InitializeType::Sample;
  energy_ = 0.0;

  centroids_.resize(k_);
  assignments_.resize(data_.size());
  rand_ = QRandomGenerator::global();
}

template <class T>
void kmeans<T>::setInitialization(InitializeType type)
{
  initType_ = type;
}

//template<class T>
//void kmeans<T>::setRandomInitBounds2D(double minX, double maxX,
//                                      double minY, double maxY)
//{
//  minX_ = minX;
//  maxX_ = maxX;
//  minY_ = minY;
//  maxY_ = maxY;
//}

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
    initialize(d);
  }
  if (currIteration_ >= maxIterations_)
    return;
  energy_ = 0.0;

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
    energy_ += minD;
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
  energy_ = 0.0;
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

template<class T>
void kmeans<T>::initialize(std::function<double(T, T)> d)
{
  switch (initType_)
  {
    case InitializeType::Random: initializeSample(); break;
    case InitializeType::Sample: initializeSample(); break;
    case InitializeType::Kpp:    initializeKpp(d);   break;
  }
}

//template<class T>
//void kmeans<T>::initializeRandom()
//{
//  std::generate(centroids_.begin(), centroids_.end(),
//  [this]()
//  {
//    return data_[rand_->bounded(data_.size())];
//  });
//}

template<class T>
void kmeans<T>::initializeSample()
{
  std::generate(centroids_.begin(), centroids_.end(),
              [this]() { return data_[rand_->bounded(data_.size())]; });
}

template<class T>
void kmeans<T>::initializeKpp(std::function<double(T, T)> d)
{
  QVector<double> distances(data_.size()), cdf(data_.size());
  double currentDistance, minDistance, totalDistance;
  double pick;

  // Initialize first centroid at random
  centroids_[0] = data_[rand_->bounded(data_.size())];

  for (int c = 1; c < centroids_.size(); c++)
  {
    totalDistance = 0.0;

    // Find minimum distance between a point and all centroids
    for (int i = 0; i < data_.size(); i++)
    {
      minDistance = std::numeric_limits<double>::max();
      for (int j = 0; j < c; j++)
      {
        currentDistance = d(data_[i], centroids_[j]);
        if (currentDistance < minDistance)
          minDistance = currentDistance;
      }
      distances[i] = minDistance;
      totalDistance += minDistance;
    }
    // Transform distances to a PDF
    std::transform(distances.begin(), distances.end(), distances.begin(),
                  [&totalDistance](double& dist){return dist / totalDistance;});

    // Create CDF from PDF
    cdf[0] = distances[0];
    for (int i = 1; i < cdf.size(); i++)
      cdf[i] = cdf[i - 1] + distances[i];

    // Pick point weighted on largest distance
    pick = rand();
    for (int i = 0; i < cdf.size(); i++)
    {
      if (pick < cdf[i])
      {
        centroids_[c] = data_[i];
        break;
      }
    }
  }
}

#endif



























