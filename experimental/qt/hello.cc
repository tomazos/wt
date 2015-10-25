#include <cstring>
#include <sys/time.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <GL/glu.h>

#include "core/must.h"
#include "core/random.h"
#include "experimental/qt/triangle.h"

constexpr auto vertex_shader_code = R"(#version 430 core

  layout(location = 1) in vec4 position;
  layout(location = 2) in vec4 color;
  out vec4 color_thru;

  void main(void)
  {
    gl_Position = position;
    color_thru = color;
  }

)";

constexpr auto fragment_shader_code = R"(#version 430 core

  in vec4 color_thru;
  out vec4 color_out;

  void main(void)
  {
    color_out = color_thru;
  }

)";

static float64 now() {
  timeval tv;
  std::memset(&tv, 0, sizeof tv);
  int gettimeofday_result = gettimeofday(&tv, nullptr);
  if (gettimeofday_result != 0) THROW_ERRNO("gettimeofday");
  return float64(tv.tv_sec) + float64(tv.tv_usec) / 1'000'000;
}

int ntriangles;

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core {
 public:
  MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

 protected:
  void initializeGL() {
    MUST(initializeOpenGLFunctions());

    shader_program = new QOpenGLShaderProgram(this);
    if (!shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                 vertex_shader_code))
      FAIL(shader_program->log().toStdString());
    if (!shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                 fragment_shader_code))
      FAIL(shader_program->log().toStdString());
    if (!shader_program->link()) FAIL(shader_program->log().toStdString());
    if (!shader_program->bind()) FAIL(shader_program->log().toStdString());

    vertex_array_object = new QOpenGLVertexArrayObject(this);
    MUST(vertex_array_object->create());
    vertex_array_object->bind();

    buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    MUST(buffer->create());
    MUST(buffer->bind());
    buffer->allocate(verts.data(), verts.size() * sizeof(float32));

    glVertexAttribPointer(shader_program->attributeLocation("position"), 3,
                          GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    shader_program->enableAttributeArray("position");
    glVertexAttribPointer(shader_program->attributeLocation("color"), 3,
                          GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    shader_program->enableAttributeArray("color");

    epoch = now();
    start_time = epoch;
    frames = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1);
  }

  void resizeGL(int w, int h) { qDebug() << "resizeGL " << w << " " << h; }

#define GLCHKERR                                           \
  {                                                        \
    auto error = glGetError();                             \
    if (error != GL_NO_ERROR) FAIL(gluErrorString(error)); \
  }

  void DumpFPS() {
    frames++;
    float64 delta = now() - start_time;

    if (delta > 1) {
      qDebug() << "FPS " << (frames / delta);
      start_time = now();
      frames = 0;
    }
  }

  void paintGL() {
    DumpFPS();

    static const GLfloat background_color[] = {0, 0, 0, 0};
    glClearBufferfv(GL_COLOR, 0, background_color);

    shader_program->setAttributeValue("now", float(now() - epoch));

    glDrawArrays(GL_TRIANGLES, 0, 3 * ntriangles);

    GLCHKERR;
  }

 private:
  float64 epoch;
  float64 start_time;
  int64 frames = 0;
  QOpenGLShaderProgram *shader_program;
  QOpenGLVertexArrayObject *vertex_array_object;
  QOpenGLBuffer *buffer = nullptr;
  QTimer *timer;
};

int main(int argc, char **argv) {
  QApplication a(argc, argv);

  QSurfaceFormat format;
  format.setVersion(4, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
  format.setSwapInterval(1);
  QSurfaceFormat::setDefaultFormat(format);

  MyGLWidget hello(nullptr);
  hello.resize(500, 300);
  hello.showMaximized();

  return a.exec();
}
