#ifndef KGLWIDGET_H
#define KGLWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QColor>
#include <QPoint>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QMatrix4x4>
#include <QDebug>
#include <QVector>
#include <QVector3D>
#include <QtMath>
#include <QTimer>
#include <QImage>
#include <QScopedPointer>
#include <math.h>
#include "camera.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define TEXTURE_NUM 2
#define VERTEX_ELEMENT_LENGTH 6

class KGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    KGLWidget(QWidget *parent = nullptr);
    ~KGLWidget() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void setClearColor(const QColor &color);

    const int scr_width = 800;
    const int scr_height = 600;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QColor m_clearColor;
    bool m_mouseClicked = false;
    float m_tmValue = 0.f;

    QOpenGLTexture *m_textures[TEXTURE_NUM];
    QOpenGLShaderProgram *m_pCubeShader = nullptr, *m_pLampShader = nullptr;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_cubeVao, m_lampVao;
    QVector3D m_lightPos, m_lightColor;

    QPoint m_lastPos;
    QVector3D m_cubePositions[10] = {
      QVector3D( 0.0f,  0.0f,  0.0f),
      QVector3D( 2.0f,  5.0f, -15.0f),
      QVector3D(-1.5f, -2.2f, -2.5f),
      QVector3D(-3.8f, -2.0f, -12.3f),
      QVector3D( 2.4f, -0.4f, -3.5f),
      QVector3D(-1.7f,  3.0f, -7.5f),
      QVector3D( 1.3f, -2.0f, -2.5f),
      QVector3D( 1.5f,  2.0f, -2.5f),
      QVector3D( 1.5f,  0.2f, -1.5f),
      QVector3D(-1.3f,  1.0f, -1.5f)
    };

    // 立方体及纹理
    QVector<GLfloat> vertices;
    QTimer *m_ptmr;
    QScopedPointer<Camera> m_pcam;

public:
    void loadCube();  // 载入立方体
    void loadLamp();  // 载入光源

signals:
    void sig_clicked();

public slots:
    void slot_tm();
};

#endif // KGLWIDGET_H
