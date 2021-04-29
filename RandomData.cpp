#include "RandomData.h"

QVector<double> RandomData::Generate(uDistd& dist, std::mt19937_64& gen,
                                     qint32 total)
{
  QVector<double> randVec;

  for (qint32 i = 0; i < total; i++)
    randVec.push_back(dist(gen));

  return randVec;
}

double RandomData::Next(uDistd& dist, std::mt19937_64& gen)
{
  return dist(gen);
}
