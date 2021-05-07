#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget *parent, Qt::WindowFlags f) :
  QOpenGLWidget(parent, f)
{
  m_center = {0, 0, 5};
  m_eye = {0, 0, 0};
  m_previousSet = false;

  auto turntableTimer = new QTimer(this);
  turntableTimer->callOnTimeout(this, &ViewWidget::updateTurntable);
  turntableTimer->start(1000.0/30.0);

  m_elapsedTimer.start();

  // Create random 3D points
  int samples = 100000;

  // Get seed from clock
  long seed = std::chrono::system_clock::now().time_since_epoch().count();

  // Seed engine and set random distribution to [-1, 1]
  std::default_random_engine engine(seed);
  std::uniform_real_distribution<float> distribution(-1.0, 1.0);

  // Create points inside a sphere
  int count = 0;
  while(count < samples)
  {
    // Uniformly sample cube
    float x = distribution(engine);
    float y = distribution(engine);
    float z = distribution(engine);

    // Reject all points outside the sphere
    if(std::sqrt(x*x + y*y + z*z) <= 1.0)
    {
      m_points.append({x, y, z});

      // Re-map positions to [0, 1] and use as color
      float r = (x + 1.0f)/2.0;
      float g = (y + 1.0f)/2.0;
      float b = (z + 1.0f)/2.0;

      m_colors.append({r, g, b});

      count++;
    }
  }

  m_fpsTimer.start();
}

void ViewWidget::initializeGL()
{
  // begin native paint drawing
  initializeOpenGLFunctions();

  glClearColor(0.25, 0.25, 0.25, 1.0);

  glEnable(GL_DEPTH_TEST);
  glPointSize(4.0f);
  glEnable(GL_POINT_SMOOTH);

  m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 color;\n"
    "varying mediump vec4 vColor;\n"
    "uniform highp mat4 matrix;\n"
    "void main(void)\n"
    "{\n"
    "   gl_Position = matrix * vertex;\n"
//    "   vColor = (vertex + vec4(1.0))/2.0;\n"
    "   vColor = color;\n"
    "}");

  m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
    "varying mediump vec4 vColor;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor = vColor;\n"
    "}");
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

void ViewWidget::setColors(QVector<float> colors)
{
  m_colors.clear();
  m_colors = colors;
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

void ViewWidget::moveEye(float x, float y, float z)
{
  m_eye.setX(m_eye.x() - x);
  m_eye.setY(m_eye.y() + y);
  m_eye.setZ(m_eye.z() + z);
}

void ViewWidget::paintGL()
{
  glEnable(GL_DEPTH_TEST);
//  QOpenGLShaderProgram program;
//  program.addShaderFromSourceCode(QOpenGLShader::Vertex,
//      "attribute highp vec4 vertex;\n"
//      "uniform highp mat4 matrix;\n"
//      "void main(void)\n"
//      "{\n"
//      "   gl_Position = matrix * vertex;\n"
//      "}");
//  program.addShaderFromSourceCode(QOpenGLShader::Fragment,
//      "uniform mediump vec4 color;\n"
//      "void main(void)\n"
//      "{\n"
//      "   gl_FragColor = color;\n"
//      "}");
//  program.link();
//  program.bind();

//  int vertexLocation = program.attributeLocation("vertex");
//  int matrixLocation = program.uniformLocation("matrix");
//  int colorLocation = program.uniformLocation("color");

//  static GLfloat const triangleVertices[] = {
//       60.0f,  10.0f,  0.0f,
//       110.0f, 110.0f, 0.0f,
//       10.0f,  110.0f, 0.0f
//   };

   // projection * view * model
  QMatrix4x4 pmvMatrix;
//     pmvMatrix.ortho(-width()/2.0, width()/2.0, -height()/2.0, height()/2.0, -1, 1);
  pmvMatrix.perspective(40.0, float(width())/height(), 1.0f, 2000.0f);
//     pmvMatrix.ortho(rect());
  pmvMatrix.lookAt(m_center, m_eye, {0, 1, 0});
//  pmvMatrix.rotate(angleForTime(m_elapsedTimer.elapsed(), 15), {0.0f, 1.0f, 0.0f});
//  program.setUniformValue(matrixLocation, pmvMatrix);

  m_pointProgram.bind();
  m_pointProgram.enableAttributeArray("vertex");
  m_pointProgram.enableAttributeArray("color");

  m_pointProgram.setUniformValue("matrix", pmvMatrix);
  m_pointProgram.setAttributeArray("vertex", m_points.constData(), 3);
  m_pointProgram.setAttributeArray("color", m_colors.constData(), 3);

  glDrawArrays(GL_POINTS, 0, m_points.count()/3);
  m_pointProgram.disableAttributeArray("vertex");
//  m_pointProgram.disableAttributeArray("color");

//  program.enableAttributeArray(vertexLocation);
//  program.setUniformValue(matrixLocation, pmvMatrix);

//  auto pentagon = createPolygon(60, 60, 0, 50, 5);
//  program.setAttributeArray(vertexLocation, pentagon.constData(), 3);
//  program.setUniformValue(colorLocation, QColor(0, 255, 0, 255));
//  glDrawArrays(GL_POLYGON, 0, pentagon.count()/3);

//  auto octagon = createPolygon(60, 60, -0.5f, 60, 8);
//  program.setAttributeArray(vertexLocation, octagon.constData(), 3);
//  program.setUniformValue(colorLocation, QColor(255, 0, 0, 255));
//  glDrawArrays(GL_POLYGON, 0, octagon.count()/3);

  //
  // Draw axes
  //

//  GLfloat axisWidth = 4.0;
//  GLfloat axisLength = 500.0;

//  GLfloat const xAxis[] = {       0.0f, 0.0f, 0.0f,   // From vertex 1
//                            axisLength, 0.0f, 0.0f }; // to vertex 1

//  glEnable(GL_LINE_SMOOTH);
//  glLineWidth(axisWidth);

//  program.setAttributeArray("vertex", xAxis, 3);
//  program.setUniformValue("color", QColor(255, 0, 0, 255));
//  glDrawArrays(GL_LINES, 0, 2);

//  GLfloat const yAxis[] = { 0.0f,       0.0f, 0.0f,   // From vertex 1
//                            0.0f, axisLength, 0.0f }; // to vertex 1

//  program.setAttributeArray("vertex", yAxis, 3);
//  program.setUniformValue("color", QColor(0, 255, 0, 255));
//  glDrawArrays(GL_LINES, 0, 2);

//  GLfloat const zAxis[] = { 0.0f, 0.0f, 0.0f,   // From vertex 1
//                            0.0f, 0.0f, axisLength }; // to vertex 1

//  program.setAttributeArray("vertex", zAxis, 3);
//  program.setUniformValue("color", QColor(0, 0, 255, 255));
//  glDrawArrays(GL_LINES, 0, 2);

//  // Reset line width
//  glLineWidth(1.0);

//  program.disableAttributeArray(vertexLocation);
  QPainter painter(this);
  painter.drawText(QRect(0, height() - 20, width(), 20),
                   QString::number(m_fps, 'G', 4) + QString(" FPS"));

  m_frameCount++;

  if (m_fpsTimer.elapsed() > 500)
  {
    m_fps = float(m_frameCount) / m_fpsTimer.restart() * 1000.0f;
    m_frameCount = 0;
  }
}

void ViewWidget::wheelEvent(QWheelEvent *event)
{
  int angle = event->angleDelta().y();

  if (angle > 0)
    m_center.setZ(m_center.z() - 0.25f);
  else
    m_center.setZ(m_center.z() + 0.25f);
}

void ViewWidget::updateTurntable()
{
//  m_turntableAngle += 1.0f;

  update();
}
