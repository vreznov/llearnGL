#include "kglwidget.h"


KGLWidget::KGLWidget(QWidget *parent) : QOpenGLWidget(parent),
    m_clearColor(51, 76, 76),
    m_lightPos(1.2f, 1.0f, 2.0f)
{
    memset(m_textures, 0, sizeof (m_textures));

    vertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
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
    m_cubeVao.destroy();
    m_lampVao.destroy();
    for(int i=0; i<TEXTURE_NUM; i++)
    {
        delete m_textures[i];
    }
    delete m_pCubeShader;
    delete m_pLampShader;
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

void KGLWidget::setClearColor(const QColor &color)
{
    m_clearColor = color;
    update();
}

void KGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    loadCube();
    loadLamp();
}

void KGLWidget::paintGL()
{
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pcam->processInput(0.2f);

    float angle = 0.5f * m_tmValue;

    // 立方体画图
    m_textures[0]->bind(0);
    m_textures[1]->bind(1);
    m_pCubeShader->bind();

    m_pCubeShader->setUniformValue("objectColor", QVector3D(1.0f, 0.5f, 0.31f));
//    m_pCubeShader->setUniformValue("lightColor",  QVector3D(1.0f, 1.0f, 1.0f));
    // 改变颜色
    m_lightColor.setX(sin(angle * 2.0f));
    m_lightColor.setY(sin(angle * 0.7f));
    m_lightColor.setZ(sin(angle * 1.3f));

    // 立方体光照
    m_pCubeShader->setUniformValue("lightPos", m_lightPos);
    m_pCubeShader->setUniformValue("viewPos", m_pcam->position);

    // 材质相关
//    QVector3D diffuseColor = m_lightColor * QVector3D(0.5f, 0.5f, 0.5f);
//    QVector3D ambientColor = diffuseColor * QVector3D(0.5f, 0.5f, 0.5f);
//    m_pCubeShader->setUniformValue("material.ambient", diffuseColor);
//    m_pCubeShader->setUniformValue("material.diffuse", ambientColor);
    m_pCubeShader->setUniformValue("material.ambient", QVector3D(1.f, 0.5f, 0.31f));
//    m_pCubeShader->setUniformValue("material.diffuse", QVector3D(1.f, 0.5f, 0.31f));
//    m_pCubeShader->setUniformValue("material.diffuse", 0);
    m_pCubeShader->setUniformValue("material.specular", QVector3D(0.5f, 0.5f, 0.5f));
    m_pCubeShader->setUniformValue("material.shininess", 64.f);
    // 设置光照强度
    m_pCubeShader->setUniformValue("light.ambient", QVector3D(0.2f, 0.2f, 0.2f));
    m_pCubeShader->setUniformValue("light.diffuse", QVector3D(0.2f, 0.2f, 0.2f));
    m_pCubeShader->setUniformValue("light.specular", QVector3D(0.2f, 0.2f, 0.2f));

    QMatrix4x4 projection, view;
    view = m_pcam->getViewMatrix();
    projection.perspective(m_pcam->zoom, 1.0f * width() / height(), 0.1f, 100.f);
    m_pCubeShader->setUniformValue("projection", projection);
    m_pCubeShader->setUniformValue("view", view);

    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_cubeVao);
        for (unsigned int i = 0; i < 1; i++)
        {
            QMatrix4x4 model;
            model.translate(m_cubePositions[i]);
//            model.rotate(angle, QVector3D(1.0f, 0.3f, 0.5f));
            m_pCubeShader->setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    m_textures[0]->release();
    m_textures[1]->release();
    m_pCubeShader->release();

    // 光源画图
    m_pLampShader->bind();
    m_pLampShader->setUniformValue("projection", projection);
    m_pLampShader->setUniformValue("view", view);
    QMatrix4x4 model;
    model.translate(m_lightPos);
    model.scale(0.2f);
    m_pLampShader->setUniformValue("model", model);
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_lampVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    m_pLampShader->release();
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
    bool ret1 = vshader->compileSourceFile("://cube_vertex.txt");
    bool ret2 = fshader->compileSourceFile("://cube_frag.txt");
    if(!ret1) { qDebug() << vshader->log(); }
    if(!ret2) { qDebug() << fshader->log(); }


    m_pCubeShader = new QOpenGLShaderProgram();
    m_pCubeShader->addShader(vshader);
    m_pCubeShader->addShader(fshader);
    m_pCubeShader->link();


    QOpenGLVertexArrayObject::Binder vaoBinder(&m_cubeVao);
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices.constData(), static_cast<int>(vertices.count()) * static_cast<int>(sizeof (GLfloat)) );

    m_pCubeShader->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, VERTEX_ELEMENT_LENGTH * sizeof(GLfloat));
    m_pCubeShader->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_pCubeShader->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT,  6 * sizeof(GLfloat), 2, VERTEX_ELEMENT_LENGTH * sizeof(GLfloat));
    m_pCubeShader->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

    QImage img1(QString("://images/container2.png"));
    QImage img2(QString("://images/container2_specular.png"));
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

    m_pCubeShader->bind();

    // 设置材质参数
    m_pCubeShader->setUniformValue("material.diffuse", 0);
    m_pCubeShader->setUniformValue("material.specular", 1);

    m_pCubeShader->release();
    m_vbo.release();

}

void KGLWidget::loadLamp()
{
//    m_vbo.allocate(vertices.constData(), static_cast<int>(vertices.count()) * static_cast<int>(sizeof (GLfloat)) );

    m_pLampShader = new QOpenGLShaderProgram();
    bool ret1, ret2;
    ret1 = m_pLampShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "://lamp_vertex.txt");
    ret2 = m_pLampShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "://lamp_frag.txt");
    if(!ret1 || !ret2) { qDebug() << m_pLampShader->log(); }

    m_pLampShader->link();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lampVao);
    m_vbo.bind();

    m_pLampShader->setAttributeBuffer(0, GL_FLOAT, 0, 3, VERTEX_ELEMENT_LENGTH * sizeof (GLfloat));
    m_pLampShader->enableAttributeArray(0);
    m_pLampShader->bind();

    m_vbo.release();
}

void KGLWidget::slot_tm()
{
    m_tmValue += 0.1f;
    update();
}
