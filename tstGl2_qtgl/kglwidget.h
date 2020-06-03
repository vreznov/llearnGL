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

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define TEXTURE_NUM 2

class KGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    KGLWidget(QWidget *parent = nullptr);
    ~KGLWidget() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void rotateBy(int fml_xAngle, int fml_yAngle, int fml_zAngle);
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

private:
    void makeObject();
    QColor m_clearColor;
    QPoint m_lastPos;
    bool m_mouseClicked = false;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;

    QOpenGLTexture *m_textures[TEXTURE_NUM];
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QMatrix4x4 m_projection, m_view, m_model;
    QVector3D cam_pos, cam_target, cam_up, cam_front, cam_right, world_up;
    float cam_yaw, cam_pitch, cam_roll;
    QVector3D m_cubePositions[10];

    QMatrix4x4 getViewMatrix();  // 获取摄像机位置
    void updateCameraVectors();  // 更新计算

    QTimer *m_ptmr;

public:
    void loadCube();  // 载入立方体

signals:
    void sig_clicked();

public slots:
    void slot_tm();
};

#endif // KGLWIDGET_H
