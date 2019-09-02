#include "Common.h"
#include "OgreRenderWindow.h"
#include <QMouseEvent>

OgreRenderWindow::OgreRenderWindow(QWidget *parent)
    : QGLWidget(parent)
{    
    mOgreRoot = new Ogre::Root();

    mOgreRoot->restoreConfig();
    mOgreRoot->initialise(false);

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("OgreCore", "FileSystem", "Bootstrap");
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC, ID_SCENEMGR);

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(RENDER_UPD_INT);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(update()));

    mTimer = new Ogre::Timer();
}


OgreRenderWindow::~OgreRenderWindow()
{
    mUpdateTimer->stop();

    mOgreWindow->removeViewport(mViewport->getZOrder());
    mOgreRoot->detachRenderTarget(mOgreWindow);
    mOgreRoot->shutdown();

    delete mUpdateTimer;
    delete mTimer;
    delete mOgreRoot;
}


/**
 * @brief Initialize the Ogre3D rendering context
 */
void OgreRenderWindow::initializeGL()
{        
    Ogre::NameValuePairList params;
    Ogre::String handle;

    /** @todo portability - obtaining the window handle for the rendering context needs to be adjusted for X11 and OSX */
    handle = Ogre::StringConverter::toString((size_t)this->parentWidget()->winId());

    params["externalWindowHandle"] = handle;

    mOgreWindow = mOgreRoot->createRenderWindow(ID_WINDOW, this->width(), this->height(), false, &params);    
    mCamera = mSceneMgr->createCamera(ID_CAMERA);

    mCamera->setPosition(Ogre::Vector3(0,0,0));
    mCamera->lookAt(Ogre::Vector3(0,0,0));
    mCamera->setNearClipDistance(1);
    mCamera->setPosition(Ogre::Vector3(0,0,-50));    

    Ogre::Light *light = mSceneMgr->createLight(ID_LIGHT);
    light->setType(Ogre::Light::LT_POINT);
    light->setPosition(Ogre::Vector3(0, 150, 250));

    mViewport = mOgreWindow->addViewport(mCamera);

    mViewport->setBackgroundColour( Ogre::ColourValue(0.3, 0.3, 0.3) );    

    // kick off the frame update timer
    mUpdateTimer->start();
}


void OgreRenderWindow::paintGL()
{
    this->update();
}


void OgreRenderWindow::resizeGL(int w, int h)
{
    mOgreWindow->windowMovedOrResized();
    mCamera->setAspectRatio(Ogre::Real(w) / Ogre::Real(h));
}


void OgreRenderWindow::update()
{
    /** NOTE: This is the recommended method for performing an update but this does
     * not handle animation updates correctly at the moment.
     */
    //mOgreRoot->_fireFrameStarted();
    //mOgreWindow->update();
    //mOgreRoot->_fireFrameEnded();

    //qDebug("%f", (float)mOgreWindow->getAverageFPS() / 1000);
    emit beforeFrameUpdate(mTimer->getMilliseconds());
    mTimer->reset();

    mOgreRoot->renderOneFrame();    
}

///
/// mouse event signals
///

void OgreRenderWindow::mouseMoveEvent(QMouseEvent *event)
{    
    emit __mouseMoveEvent(event);
}


void OgreRenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    emit __mouseReleaseEvent(event);
}


void OgreRenderWindow::wheelEvent(QWheelEvent *event)
{
    emit __wheelEvent(event);
}
