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
  randomCentroidsInitialized_ = false;
  stopReason = "";
  ignoreSame_ = false;

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
  randomCentroidsInitialized_ = false;
  stopReason = "";
  ignoreSame_ = false;

  centroids_.resize(k_);
  assignments_.resize(data_.size());
  rand_ = QRandomGenerator::global();
}

template <class T>
void kmeans<T>::setInitialization(InitializeType type)
{
  initType_ = type;
}

template<class T>
void kmeans<T>::setRandomCentroids(QVector<T> centroids)
{
  centroids_ = centroids;
  randomCentroidsInitialized_ = true;
}

template<class T>
void kmeans<T>::setIgnoreSameAssignments(bool flag)
{
  ignoreSame_ = flag;
}

template <class T>
void kmeans<T>::setData(QVector<T> data)
{
  data_ = data;
  assignments_.resize(data_.size());
}

template<class T>
void kmeans<T>::setK(int k)
{
  k_ = k;
  initialized_ = false;
  reset();
}

template <class T>
bool kmeans<T>::step(std::function<double(T, T)> d)
{
  if (!stopReason.isEmpty())
    return false;

  bool sameAssignments = true;
  // Random assignment of centroids to data
  if (!initialized_)
  {
    initialized_ = true;
    if (!initialize(d))
    {
      stopReason = "Not initialized.";
      return false;
    }
  }
  if (currIteration_ >= maxIterations_)
  {
    stopReason = "Maximum number of iterations.";
    return false;
  }
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
    if (assignments_[p] != assignedC)
      sameAssignments = false;
    assignments_[p] = assignedC;

    newCentroids[assignedC] += data_[p];
    cCount[assignedC]++;
  }

  // Calculate new cluster centers
  for (qint32 i = 0; i < k_; i++)
    if (cCount[i] != 0)
      centroids_[i] = newCentroids[i] / cCount[i];
  currIteration_++;
  if (sameAssignments && !ignoreSame_)
  {
    stopReason = "Assignments didn't change.";
    return false;
  }
  ignoreSame_ = false;
  return true;
}

template <class T>
bool kmeans<T>::step(std::function<double(T, T)> d, int steps)
{
  bool running = true;
  for (int i = 0; i < steps && running; i++)
    running = step(d);
  return running;
}

template <class T>
bool kmeans<T>::finish(std::function<double(T, T)> d)
{
  qint32 finishCount = maxIterations_ - currIteration_;
  bool running = true;
  for (qint32 i = 0; i < finishCount && running; i++)
    running = step(d);
  return running;
}

template<class T>
void kmeans<T>::reset()
{
  centroids_.resize(k_);
  assignments_.resize(data_.size());
  initialized_ = false;
  randomCentroidsInitialized_ = false;
  energy_ = 0.0;
  stopReason = "";
  currIteration_ = 0;
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
bool kmeans<T>::initialize(std::function<double(T, T)> d)
{
  switch (initType_)
  {
    case InitializeType::Random: return checkRandomCentroids(); break;
    case InitializeType::Sample: return initializeSample();     break;
    case InitializeType::Kpp:    return initializeKpp(d);       break;
    default: return false;                                      break;
  }
}

template<class T>
bool kmeans<T>::checkRandomCentroids()
{
  return randomCentroidsInitialized_;
}

template<class T>
bool kmeans<T>::initializeSample()
{
  std::generate(centroids_.begin(), centroids_.end(),
              [this]() { return data_[rand_->bounded(data_.size())]; });
  return true;
}

template<class T>
bool kmeans<T>::initializeKpp(std::function<double(T, T)> d)
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
  return true;
}

#endif



























