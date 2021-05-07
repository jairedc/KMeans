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
#include <QColor>
#include <QDebug>
#include <QWheelEvent>

class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  ViewWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  QVector<GLfloat> createPolygon(float x, float y, float z, float radius,
                                 int sides);
  float angleForTime(qint64 msTime, float secondsPerRotation) const;

  void zoom();
  QVector3D getEye() { return m_eye; };
  void setColors(QVector<float> colors);
  void setPoints(QVector<double> xPoints, QVector<double> yPoints,
                 QVector<double> zPoints);
  void moveEye(float x, float y, float z);
//  void setPoints(QVector)

protected:
  void initializeGL() override;
  void paintGL() override;
  void wheelEvent(QWheelEvent* event) override;

private slots:
  void updateTurntable();

private:
  float m_turntableAngle = 0.0f;
  QVector<float> m_points;
  QVector<float> m_colors;
  QOpenGLShaderProgram m_pointProgram;
  QElapsedTimer m_elapsedTimer;
  QVector3D m_center;
  QVector3D m_eye;
  QPoint m_lastPos;
  bool m_previousSet;

  QElapsedTimer m_fpsTimer;
  int m_frameCount;
  float m_fps;
};

#endif // VIEWWIDGET_H
