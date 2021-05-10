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
  void setPointColors(QVector<float> colors);
  void setCentroidColors(QVector<float> colors);
  void setPoints(QVector<double> xPoints, QVector<double> yPoints,
                 QVector<double> zPoints);
  void setPointSize(float pointsize);
  void setCentroidPoints(QVector<float> centroids);
  void switchRotate();
  void reset();
  void moveEye(float x, float y, float z);
//  void setPoints(QVector)

protected:
  void initializeGL() override;
  void paintGL() override;
  void wheelEvent(QWheelEvent* event) override;

//private slots:
//  void updateTurntable();

private:
  float m_turntableAngle = 0.0f;
  float m_pointSize = 0.0f;
  QVector<float> m_points, m_centroids;
  QVector<float> m_pointColors, m_centroidColors;
  QOpenGLShaderProgram m_pointProgram;
  QOpenGLShaderProgram m_axesProgram;
  QElapsedTimer m_elapsedTimer;
  qint64 m_lastTime;
  QVector3D m_center;
  QVector3D m_eye;
  QPoint m_lastPos;
  bool m_previousSet;
  bool m_rotate;
  float m_currentRotation, m_lastRotation;

  QElapsedTimer m_fpsTimer;
  int m_frameCount;
  float m_fps;
  const char* vertexCode =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 color;\n"
    "varying mediump vec4 vColor;\n"
    "uniform highp mat4 matrix;\n"
    "void main(void)\n"
    "{\n"
    "   gl_Position = matrix * vertex;\n"
    "   vColor = color;\n"
    "}";
  const char* fragCode =
    "varying mediump vec4 vColor;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor = vColor;\n"
    "}";
};

#endif // VIEWWIDGET_H
