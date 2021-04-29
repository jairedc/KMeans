#ifndef RANDOMDATA_H
#define RANDOMDATA_H

#include <random>
#include <QVector>

typedef std::uniform_real_distribution<double> uDistd;

class RandomData
{
public:
  static QVector<double> Generate(uDistd& dist, std::mt19937_64& gen,
                                  qint32 total);
  static double Next(uDistd& dist, std::mt19937_64& gen);
};

#endif // RANDOMDATA_H
