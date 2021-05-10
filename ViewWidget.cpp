#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget *parent, Qt::WindowFlags f) :
  QOpenGLWidget(parent, f)
{
  m_center = {0, 0, 5};
  m_eye = {0, 0, 0};
  m_previousSet = false;
  m_pointSize = 4.0f;
  m_rotate = false;
  m_lastRotation = 0.0f;
  m_currentRotation = 0.0f;

  m_elapsedTimer.start();
  m_fpsTimer.start();
}

void ViewWidget::initializeGL()
{
  // begin native paint drawing
  initializeOpenGLFunctions();

  glClearColor(0.25, 0.25, 0.25, 1.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SMOOTH);

  m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexCode);
  m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragCode);
  m_pointProgram.link();
}

QVector<GLfloat> ViewWidget::createPolygon(float x, float y, float z, float radius,
                               int sides)
{
  QVector<GLfloat> result;

  float angle = -M_PI_2;
  float step = 2.0f * M_PI/sides;

  for (int i = 0; i < sides; ++i)
  {
    result.push_back(x + radius * qCos(angle));
    result.push_back(y + radius * qSin(angle));
    result.push_back(z);
    angle += step;
  }

  return result;
}

float ViewWidget::angleForTime(qint64 msTime, float secondsPerRotation) const
{
  float msPerRotation = secondsPerRotation * 1000.0;
  float t = msTime / msPerRotation;

  return (t - qFloor(t)) * 360.0;
}

void ViewWidget::setPointColors(QVector<float> colors)
{
  m_pointColors.clear();
  m_pointColors = colors;
}

void ViewWidget::setCentroidColors(QVector<float> colors)
{
  m_centroidColors.clear();
  m_centroidColors = colors;
}

void ViewWidget::setPoints(QVector<double> xPoints, QVector<double> yPoints, QVector<double> zPoints)
{
  m_points.clear();
  for (int i = 0; i < xPoints.size(); i++)
  {
    m_points.append(xPoints[i]);
    m_points.append(yPoints[i]);
    m_points.append(zPoints[i]);
  }
}

void ViewWidget::setPointSize(float pointsize)
{
  m_pointSize = pointsize;
}

void ViewWidget::setCentroidPoints(QVector<float> centroids)
{
  m_centroids.clear();
  m_centroids = centroids;
}

void ViewWidget::switchRotate()
{
  m_rotate = !m_rotate;
  if(!m_rotate)
    m_lastRotation = m_currentRotation;
}

void ViewWidget::reset()
{
  m_previousSet = false;
  m_pointSize = 4.0f;

  m_centroids.clear();
  m_centroidColors.clear();
  m_pointColors.clear();
  for (int i = 0; i < m_points.size(); i++)
  {
    m_pointColors.append(0.0f);
    m_pointColors.append(1.0f);
    m_pointColors.append(1.0f);
  }

}

void ViewWidget::moveEye(float x, float y, float z)
{
  m_eye.setX(m_eye.x() - x);
  m_eye.setY(m_eye.y() + y);
  m_eye.setZ(m_eye.z() + z);
}

void ViewWidget::paintGL()
{
  glEnable(GL_DEPTH_TEST);

  QMatrix4x4 pmvMatrix;
  pmvMatrix.perspective(40.0, float(width())/height(), 1.0f, 2000.0f);
  pmvMatrix.lookAt(m_center, m_eye, {0, 1, 0});
  if (m_rotate)
    pmvMatrix.rotate(angleForTime(m_elapsedTimer.elapsed(),15),
                     {0.0f, 1.0f, 0.0f});

  m_pointProgram.bind();
  m_pointProgram.enableAttributeArray("vertex");
  m_pointProgram.enableAttributeArray("color");

  m_pointProgram.setUniformValue("matrix", pmvMatrix);
  m_pointProgram.setAttributeArray("vertex", m_points.constData(), 3);
  m_pointProgram.setAttributeArray("color", m_pointColors.constData(), 3);

  glPointSize(m_pointSize);
  glDrawArrays(GL_POINTS, 0, m_points.count()/3);

  m_pointProgram.disableAttributeArray("vertex");
  m_pointProgram.enableAttributeArray("vertex");
  m_pointProgram.setAttributeArray("vertex", m_centroids.constData(), 3);
  m_pointProgram.setAttributeArray("color", m_centroidColors.constData(), 3);
  glPointSize(m_pointSize + 20.0f);
  glDrawArrays(GL_POINTS, 0, m_centroids.count()/3);
  m_pointProgram.disableAttributeArray("vertex");

  QPainter painter(this);
  painter.drawText(QRect(0, height() - 20, width(), 20),
                   QString::number(m_fps, 'G', 4) + QString(" FPS"));

  m_frameCount++;

  if (m_fpsTimer.elapsed() > 500)
  {
    m_fps = float(m_frameCount) / m_fpsTimer.restart() * 1000.0f;
    m_frameCount = 0;
  }
  update();
}

void ViewWidget::wheelEvent(QWheelEvent *event)
{
  int angle = event->angleDelta().y();

  if (angle > 0)
    m_center.setZ(m_center.z() - 1.00f);
  else
    m_center.setZ(m_center.z() + 1.00f);
}

//void ViewWidget::updateTurntable()
//{
//  update();
//}
