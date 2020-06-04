#include "kglwidget.h"


KGLWidget::KGLWidget(QWidget *parent) : QOpenGLWidget(parent),
    m_clearColor(51, 76, 76),
    m_program(nullptr)
{
    memset(m_textures, 0, sizeof (m_textures));

    // 初始化观察相机
    m_pcam.reset(new Camera(QVector3D(5.0f, 0.0f, 10.0f)));
    // 初始化位置

    m_ptmr = new QTimer();
    connect(m_ptmr, &QTimer::timeout, this, &KGLWidget::slot_tm);
    m_ptmr->start(50);
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
    loadCube();

}

void KGLWidget::paintGL()
{
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pcam->processInput(1.f);

    m_textures[0]->bind(0);
    m_textures[1]->bind(1);
    m_program->bind();

    m_view = QMatrix4x4();
    m_program->setUniformValue("view", m_view);

    QMatrix4x4 projection;
    projection.perspective(m_pcam->zoom, 1.0f * width() / height(), 0.1f, 100.f);
    m_program->setUniformValue("projection", projection);
    m_program->setUniformValue("view", m_pcam->getViewMatrix());

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    for (unsigned int i = 0; i < 1; i++)
    {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        float angle = (i + 1.0f) * m_tmValue;
        model.rotate(angle, QVector3D(1.0f, 0.3f, 0.5f));
        m_program->setUniformValue("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

//    glDrawArrays( GL_TRIANGLES, 0, 36);

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
    if(event->button() == Qt::LeftButton)
    {
        m_mouseClicked = true;
        m_lastPos = event->pos();
    }
}

void KGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_mouseClicked) return;

    if (m_mouseClicked) {
        int xpos = event->pos().x();
        int ypos = event->pos().y();

        int xoffset = xpos - m_lastPos.x();
        int yoffset = m_lastPos.y() - ypos;
        m_lastPos = event->pos();
        m_pcam->processMouseMovement(xoffset, yoffset);
    }
}

void KGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_mouseClicked = false;
    emit sig_clicked();
}

void KGLWidget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key >= 0 && key < 1024)
        m_pcam->keys[key] = true;
}

void KGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key >= 0 && key < 1024)
        m_pcam->keys[key] = false;
}

void KGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint offset = event->angleDelta();
    m_pcam->processMouseScroll(offset.y()/20.0f);
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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices.constData(), static_cast<int>(vertices.count()) * static_cast<int>(sizeof (GLfloat)) );

    m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

    glEnable(GL_DEPTH_TEST);

    QImage img1(QString("://images/wall1.jpg"));
    QImage img2(QString("://images/awesomeface.jpg"));
    m_textures[0] = new QOpenGLTexture(img1) ;
    m_textures[1] = new QOpenGLTexture(img2.mirrored()) ;
    bool suc_texture[TEXTURE_NUM] = {false};
    suc_texture[0] = m_textures[0]->isCreated();
    suc_texture[1] = m_textures[1]->isCreated();
    if(!m_textures[0]->isCreated() || !m_textures[1]->isCreated())
    {
        qDebug() << "creat texture failed";
    }

    m_textures[0]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[0]->setMagnificationFilter(QOpenGLTexture::Linear);
    m_textures[0]->setWrapMode(QOpenGLTexture::Repeat);
    m_textures[1]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[1]->setMagnificationFilter(QOpenGLTexture::Linear);
    m_textures[1]->setWrapMode(QOpenGLTexture::Repeat);

    m_program->bind();

    m_program->setUniformValue("texture1", 0);
    m_program->setUniformValue("texture2", 1);
    m_program->release();

    m_vbo.release();
}

void KGLWidget::slot_tm()
{

    m_tmValue += 0.1f;
    qDebug() << "time value " << m_tmValue;
    update();
}
