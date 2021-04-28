#include "RandomData.h"

RandomData::RandomData(qint32 numberOfData, qint32 low, qint32 high)
{
  numberOfData_ = numberOfData;
  low_ = low;
  high_ = high;
}

QVector<double> RandomData::Generate()
{
  QVector<double> randVec;

  for (qint32 i = 0; i < numberOfData_; i++)
    randVec.push_back((double)QRandomGenerator64::system()->bounded(low_, high_));

  return randVec;
}
