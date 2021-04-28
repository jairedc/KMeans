#ifndef RANDOMDATA_H
#define RANDOMDATA_H

#include <QRandomGenerator>
#include <QVector>

class RandomData
{
private:
  qint32 numberOfData_;
  qint32 low_, high_;
public:
  RandomData(qint32 numberOfData, qint32 low, qint32 high);
  QVector<double> Generate();
};

#endif // RANDOMDATA_H
