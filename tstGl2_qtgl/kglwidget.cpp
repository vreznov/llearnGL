#include "kglwidget.h"


KGLWidget::KGLWidget(QWidget *parent) : QOpenGLWidget(parent),
    m_clearColor(51, 76, 76),
    m_xRot(0),
    m_yRot(0),
    m_zRot(0),
    m_program(nullptr)
{
    memset(m_textures, 0, sizeof (m_textures));

    // 初始化观察相机
    cam_pos = QVector3D(0.f, 0.f, 3.f);
    world_up = QVector3D(0.f, 1.f, 0.f);
    cam_up = QVector3D(0.f, 1.f, 0.f);
    cam_front = QVector3D(0.f, 0.f, -1.f);
    cam_right = QVector3D(0.f, 1.f, 0.f);

    // 初始化位置
    m_cubePositions[0] = QVector3D(0.0f,  0.0f,  0.0f);
    m_cubePositions[1] = QVector3D(0.1f,  0.1f, -0.1f);
    m_cubePositions[2] = QVector3D(-0.5f, -0.2f, -0.5f);
    m_cubePositions[3] = QVector3D(-0.8f, -0.0f, -0.3f);
    m_cubePositions[4] = QVector3D(0.4f, -0.4f, -0.5f);

    m_ptmr = new QTimer();
    connect(m_ptmr, &QTimer::timeout, this, &KGLWidget::slot_tm);
    m_ptmr->setInterval(100);
    m_ptmr->start();
}

KGLWidget::~KGLWidget()
{
    m_ptmr->stop();
    delete m_ptmr;
    m_ptmr = nullptr;

    makeCurrent();
    m_vbo.destroy();
    m_vao.destroy();
    for(int i=0; i<TEXTURE_NUM; i++)
    {
        delete m_textures[i];
    }
    delete m_program;
    doneCurrent();
}

QSize KGLWidget::minimumSizeHint() const
{
    return  QSize(scr_width, scr_height);
}

QSize KGLWidget::sizeHint() const
{
    return QSize(scr_width, scr_height);
}

void KGLWidget::rotateBy(int fml_xAngle, int fml_yAngle, int fml_zAngle)
{
    m_xRot += fml_xAngle;
    m_yRot += fml_yAngle;
    m_zRot += fml_zAngle;
    update();
}

void KGLWidget::setClearColor(const QColor &color)
{
    m_clearColor = color;
    update();
}

void KGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    makeObject();
    loadCube();

}

void KGLWidget::paintGL()
{
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT);

    m_textures[0]->bind(0);
    m_textures[1]->bind(1);

    m_view = getViewMatrix();

    QMatrix4x4 transform;
    transform.translate(QVector3D(0.0f, -0.0f, -2.0f));
    transform.rotate(M_PI_2, QVector3D(0.f, 1.f, 0.f));
//    m_program->setUniformValue("model", m_model);
//    m_program->setUniformValue("projection", m_projection);

//    qDebug() << m_view;

    m_program->bind();
    m_program->setUniformValue("view", transform);

    m_model = QMatrix4x4();
//    for (unsigned int i = 0; i < 1; i++)
//    {
//        m_model.translate(m_cubePositions[i].normalized());
//        m_model.rotate(20.f * (i+1) , QVector3D(1.f, 0.3f, 0.0f));
//        m_program->setUniformValue("model", m_model);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//    }

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    glDrawArrays( GL_TRIANGLES, 0, 36);

    m_textures[0]->release();
    m_textures[1]->release();
    m_program->release();
}

void KGLWidget::resizeGL(int w, int h)
{
    int side = qMin(w,  h);
//    glViewport((w - side) / 2, (h - side) / 2, side, side);
    glViewport(0, 0, w, h);
}

void KGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseClicked = true;
    m_lastPos = event->pos();
}

void KGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_mouseClicked) return;

    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        rotateBy(8 * dy, 8 * dx, 0);
    }
    else if(event->buttons() & Qt::RightButton)
    {
        rotateBy(8 * dy, 0, 8 * dx);
    }
    m_lastPos = event->pos();
}

void KGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseClicked = false;
    emit sig_clicked();
}

void KGLWidget::keyPressEvent(QKeyEvent *event)
{
    float dis = 0.1f;
    if(event->key() == Qt::Key_Escape)
    {
        this->close();
    }
    else if(event->key() == Qt::Key_W)
    {
//        cam_pos += cam_front * dis;
        cam_pos.setZ(cam_pos.z() + 0.1);
    }
    else if(event->key() == Qt::Key_S)
    {
//        cam_pos -= cam_front * dis;
        cam_pos.setZ(cam_pos.z() + -0.1);
    }
    else if(event->key() == Qt::Key_A)
    {
//        cam_pos -= cam_right * dis;
        cam_pos.setY(cam_pos.y() + 0.1);
    }
    else if(event->key() == Qt::Key_D)
    {
//        cam_pos += cam_right * dis;
        cam_pos.setY(cam_pos.y() + -0.1);
    }

    qDebug() << cam_pos;
}

void KGLWidget::keyReleaseEvent(QKeyEvent *event)
{

}

void KGLWidget::makeObject()
{

}

QMatrix4x4 KGLWidget::getViewMatrix()
{
    updateCameraVectors();

    QMatrix4x4 view;
    cam_target = cam_pos + cam_front;
//    view.lookAt(cam_pos, cam_target, cam_up);

//    qDebug() << view;
    return view;
}

void KGLWidget::updateCameraVectors()
{
    QVector3D front;

    front.setX(static_cast<float>(qCos(qRadiansToDegrees(static_cast<qreal>(cam_yaw))) * qCos(qRadiansToDegrees(static_cast<qreal>(cam_pitch)))));
    front.setY(static_cast<float>(qSin(qRadiansToDegrees(static_cast<qreal>(cam_pitch)))));
    front.setZ(static_cast<float>(qSin(qRadiansToDegrees(static_cast<qreal>(cam_yaw))) * qCos(qRadiansToDegrees(static_cast<qreal>(cam_pitch)))));
//    cam_front = front;

    QVector3D right = QVector3D::crossProduct(cam_front, world_up);
//    cam_right = right.normalized();
    cam_up = QVector3D::crossProduct(right, cam_front);
}

void KGLWidget::loadCube()
{
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    bool ret1 = vshader->compileSourceFile("://vertex.txt");
    bool ret2 = fshader->compileSourceFile("://fragment.txt");

    if(!ret1) { qDebug() << vshader->log(); }
    if(!ret2) { qDebug() << fshader->log(); }


    m_program = new QOpenGLShaderProgram();
    m_program->addShader(vshader);
    m_program->addShader(fshader);
//    m_program->bindAttributeLocation("aPos", PROGRAM_VERTEX_ATTRIBUTE);
//    m_program->bindAttributeLocation("aTexCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->link();

    // 立方体及纹理
    QVector<GLfloat> vertices = {
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,

        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,

        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,

        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,

        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,

        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f
    };

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices.constData(), static_cast<int>(vertices.count()) * static_cast<int>(sizeof (GLfloat)) );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QImage img1(QString("://images/wall1.jpg"));
    QImage img2(QString("://images/awesomeface.jpg"));
    m_textures[0] = new QOpenGLTexture(img1) ;
    m_textures[1] = new QOpenGLTexture(img2) ;

    m_textures[0]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[0]->setMagnificationFilter(QOpenGLTexture::Linear);
    m_textures[0]->setWrapMode(QOpenGLTexture::Repeat);
    m_textures[1]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[1]->setMagnificationFilter(QOpenGLTexture::Linear);
    m_textures[1]->setWrapMode(QOpenGLTexture::Repeat);

    m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

    m_program->bind();

    m_program->setUniformValue("texture1", 0);
    m_program->setUniformValue("texture2", 1);
    m_program->release();

    m_vbo.release();
}

void KGLWidget::slot_tm()
{
    paintGL();
}
