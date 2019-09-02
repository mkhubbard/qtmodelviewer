#ifndef _OGRERENDERWINDOW_H_
#define _OGRERENDERWINDOW_H_

#include <QtOpenGL/QGLWidget>
#include <QTimer>

#include "Ogre.h"
#include "Mesh.h"
#include "Common.h"
#include "Grid.h"

class OgreRenderWindow : public QGLWidget
{   
    Q_OBJECT
public:
    OgreRenderWindow(QWidget *parent = 0);
    ~OgreRenderWindow();

    ///
    /// Events
    ///

    virtual void initializeGL();
    virtual void resizeGL( int w, int h);
    virtual void paintGL();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    ///
    /// Methods
    ///

    void initializeScene();    
    void update();

    ///
    /// Member Accessors
    ///

    inline Ogre::SceneManager *getSceneMgr() { return this->mSceneMgr; }
    inline Ogre::Root *getOgreRoot() { return this->mOgreRoot; }
    inline Ogre::ResourceGroupManager *getResourceMgr() { return Ogre::ResourceGroupManager::getSingletonPtr(); }
    inline Ogre::Camera* camera() { return this->mCamera; }

protected:
    QTimer *mUpdateTimer;
    Ogre::Timer *mTimer;

    Ogre::Root *mOgreRoot;
    Ogre::RenderWindow *mOgreWindow;
    Ogre::Camera *mCamera;
    Ogre::Viewport *mViewport;
    Ogre::SceneManager *mSceneMgr;    

signals:
    void beforeFrameUpdate(unsigned long elapsed);
    void __mouseMoveEvent(QMouseEvent *event);
    void __mouseReleaseEvent(QMouseEvent *event);
    void __wheelEvent(QWheelEvent *event);

};
#endif // _OGRERENDERWINDOW_H_
