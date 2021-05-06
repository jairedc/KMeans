#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QtMath>
#include <QTimer>
#include <chrono>
#include <random>
#include <QElapsedTimer>
#include <QPainter>

class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  ViewWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  QVector<GLfloat> createPolygon(float x, float y, float z, float radius,
                                 int sides);
  float angleForTime(qint64 msTime, float secondsPerRotation) const;

protected:
  void initializeGL() override;
  void paintGL() override;

private slots:
  void updateTurntable();

private:
  float m_turntableAngle = 0.0f;
  QVector<float> m_points;
  QVector<float> m_colors;
  QOpenGLShaderProgram m_pointProgram;
  QElapsedTimer m_elapsedTimer;

  QElapsedTimer m_fpsTimer;
  int m_frameCount;
  float m_fps;
};

#endif // VIEWWIDGET_H
