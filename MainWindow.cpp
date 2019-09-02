#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QFileInfo>
#include <QMouseEvent>

#include "Common.h"
#include "MainWindow.h"
#include "Mesh.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);

    mMesh = new Mesh(ui->ogreWindow->getSceneMgr(), ui->ogreWindow->getResourceMgr());
    mGrid = new Grid(ui->ogreWindow->getSceneMgr());

    this->_tweakUi();
    this->_connectSignals();

    // HACK: this is used to track a state in the mouse presses and reset the lastX/lastY positions
    // to prevent jumping which has been introduced when moving to QT
    mMouseFirstHit = true;
}

MainWindow::~MainWindow()
{
    delete mMesh;
    delete mGrid;

    delete ui;
}

/**
 * @brief Apply interface options that cannot be done in the designer
 */
void MainWindow::_tweakUi()
{
    // setup the polygon mode menu and toolbar button
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(ui->actionTexture);
    ag->addAction(ui->actionWireframe);
    ag->addAction(ui->actionPoints);
    ag->setExclusive(true);
    ui->actionTexture->setChecked(true);

    QMenu *qm = new QMenu(this);
    qm->addAction(ui->actionTexture);
    qm->addAction(ui->actionWireframe);
    qm->addAction(ui->actionPoints);    

    ui->polyMode->setMenu(qm);
    ui->viewToolBar->addWidget(ui->polyMode);

    ui->actionRenderMode->setMenu(qm);

    // setup a shortcut for the mesh actions
    agMesh = new QActionGroup(this);
    agMesh->addAction(ui->actionBoundingBox);
    agMesh->addAction(ui->actionGrid);
    agMesh->addAction(ui->actionSkeleton);
    agMesh->addAction(ui->actionTexture);
    agMesh->addAction(ui->actionWireframe);
    agMesh->addAction(ui->actionPoints);
    agMesh->addAction(ui->actionRenderMode);
    agMesh->setExclusive(false);


    // animation toolbar    
    ui->tbAnimation->addWidget(ui->animControls);

    _resetMeshControls();
    _disableMeshControls(true);

    connect(ag, SIGNAL(triggered(QAction*)), this, SLOT(_polygonModeChanged(QAction*)));
    connect(ui->polyMode, SIGNAL(clicked()), this, SLOT(_polygonModeCycle(void)));
}

void MainWindow::_connectSignals()
{
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSkeleton, SIGNAL(toggled(bool)), this, SLOT(_toggleSkeleton(bool)));
    connect(ui->actionBoundingBox, SIGNAL(toggled(bool)), this, SLOT(_toggleBoundingBox(bool)));
    connect(ui->actionGrid, SIGNAL(toggled(bool)), this, SLOT(_toggleGrid(bool)));
    connect(ui->actionOpenMesh, SIGNAL(triggered()), this, SLOT(_openMesh()));
    connect(ui->actionCloseMesh, SIGNAL(triggered()), this, SLOT(_closeMesh()));

    connect(ui->animControls, SIGNAL(currentAnimationChanged(QString)), this, SLOT(_currentAnimationChanged(QString)));
    connect(ui->animControls, SIGNAL(commandTriggered(AnimationFrame::TransportCommand)), this, SLOT(_transportCommandTriggered(AnimationFrame::TransportCommand)));
    connect(ui->animControls, SIGNAL(positionChanged(float)), this, SLOT(_animationPositionSeek(float)));

    connect(ui->ogreWindow, SIGNAL(beforeFrameUpdate(unsigned long)), this, SLOT(_beforeFrameUpdate(unsigned long)));

    connect(ui->ogreWindow, SIGNAL(__mouseMoveEvent(QMouseEvent*)), this, SLOT(_mouseMove(QMouseEvent*)));
    connect(ui->ogreWindow, SIGNAL(__mouseReleaseEvent(QMouseEvent*)), this, SLOT(_mouseRelease(QMouseEvent*)));
    connect(ui->ogreWindow, SIGNAL(__wheelEvent(QWheelEvent*)), this, SLOT(_mouseWheel(QWheelEvent*)));

    connect(mMesh, SIGNAL(animationChanged(float)), this, SLOT(_animationChanged(float)));
    connect(mMesh, SIGNAL(animationPositionChanged(float)), this, SLOT(_animationPositionChanged(float)));
} // connectActions

///
/// ui tweaking
///

void MainWindow::_resetMeshControls()
{
    ui->actionBoundingBox->setChecked(false);
    ui->actionSkeleton->setChecked(false);
    ui->actionTexture->setChecked(true);
    ui->polyMode->setText(ui->actionTexture->text());

    ui->tbAnimation->setEnabled(false);
    ui->animControls->reset();    
}

void MainWindow::_disableMeshControls(bool disabled)
{
    agMesh->setDisabled(disabled);
    ui->actionCloseMesh->setDisabled(disabled);
    ui->polyMode->setDisabled(disabled);
}

///
/// mesh loading / unloading
///

void MainWindow::_openMesh()
{    
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Mesh"), 0, tr("Ogre Mesh Files (*.mesh)"));

    if ( filename != "" ) {
        QFileInfo fn(filename);
        mMesh->load(fn.fileName().toStdString(), fn.absolutePath().toStdString(), true);        
        this->_resetScene();

        // update ui state
	_resetMeshControls();
        _disableMeshControls(false);

        ui->actionCloseMesh->setDisabled(false);
        ui->actionSkeleton->setDisabled( !mMesh->hasSkeleton() );

        mGrid->setVisible(ui->actionGrid->isChecked());
        mMesh->showSkeleton(ui->actionSkeleton->isChecked());
        mMesh->showBoundingBox(ui->actionBoundingBox->isChecked());

        // load animations
        QStringList animNames;

        Ogre::Entity *e = mMesh->getOgreEntity();
        Ogre::AnimationStateSet *as = e->getAllAnimationStates();
        if ( as ) {
            Ogre::AnimationStateIterator animations = as->getAnimationStateIterator();
            while ( animations.hasMoreElements() ) {
                Ogre::AnimationState* anim = animations.getNext();
                animNames.append( anim->getAnimationName().c_str() );
            }
        }

        if ( animNames.count() == 0 ) {
            ui->tbAnimation->setDisabled(true);
        } else {
            ui->tbAnimation->setDisabled(false);            
        }

        ui->animControls->setAnimationNames(animNames);
    }
}

void MainWindow::_closeMesh()
{
    mMesh->unload();

    // update control states
    _resetMeshControls();
    _disableMeshControls(true);

    ui->actionCloseMesh->setDisabled(true);
    mGrid->setVisible(false);
}

void MainWindow::_resetScene()
{
    // calculate camera start position
    if ( mMesh->isMeshLoaded() ) {
        Ogre::AxisAlignedBox aabb = mMesh->getOgreEntity()->getBoundingBox();
        Ogre::Vector3 aabbSize = aabb.getSize();

        float camDist = 0.0f;
        if ( aabbSize.x > camDist ) { camDist = aabbSize.x; }
        if ( aabbSize.y > camDist ) { camDist = aabbSize.y; }
        if ( aabbSize.z > camDist ) { camDist = aabbSize.z; }

        mViewState.defCamPos = Ogre::Vector3(0, trunc(aabb.getCenter().y), camDist*2);
        mViewState.curCamPos = mViewState.defCamPos;
        ui->ogreWindow->camera()->setPosition(mViewState.curCamPos);

        mGrid->setSize(40, mGrid->clamp(mViewState.curCamPos.z / 10));
    }
}

///
/// toolbar events
///

void MainWindow::_toggleGrid(bool checked)
{
    mGrid->setVisible(checked);
}

void MainWindow::_toggleBoundingBox(bool checked)
{    
    mMesh->showBoundingBox(checked);    
}

void MainWindow::_toggleSkeleton(bool checked)
{
    mMesh->showSkeleton(checked);
}

void MainWindow::_polygonModeChanged(QAction *action)
{
    if ( action->objectName() == "actionTexture") {
        mMesh->setPolygonMode(Mesh::PM_SOLID);
    } else if (action->objectName() == "actionWireframe" ) {
        mMesh->setPolygonMode(Mesh::PM_WIREFRAME);
    } else if (action->objectName() == "actionPoints" ) {
        mMesh->setPolygonMode(Mesh::PM_POINTS);
    }

    ui->polyMode->setText(action->text());
}

///
/// animation controls
///

void MainWindow::_transportCommandTriggered(const AnimationFrame::TransportCommand& command)
{
    qDebug("Command Triggered!");

    switch(command) {
        case AnimationFrame::PLAY:
            mMesh->playAnimation();
            break;

        case AnimationFrame::STOP:
        case AnimationFrame::PAUSE:
            mMesh->stopAnimation();
            break;

        case AnimationFrame::LOOP_ON:
            mMesh->setAnimationLoop(true);
            break;

        case AnimationFrame::LOOP_OFF:
            mMesh->setAnimationLoop(false);
            break;

        default:
            break;
    }
}

void MainWindow::_currentAnimationChanged(const QString &name)
{
    if ( name == "" ) { // don't like this but doesn't seem like another event would be worth it.
        mMesh->clearActiveAnimation();
    } else {
        mMesh->setActiveAnimation(name.toStdString());
    }
}

// signal from Mesh object when animation has been changed
void MainWindow::_animationChanged(float length)
{
    ui->animControls->setAnimationLength(length);
}

void MainWindow::_animationPositionChanged(float position)
{
    ui->animControls->setPosition(position);
}

void MainWindow::_animationPositionSeek(float position)
{
    mMesh->setAnimationPosition(position);
}

///
/// render window events
///

void MainWindow::_beforeFrameUpdate(unsigned long elapsed)
{    
    ui->ogreWindow->camera()->setPosition(mViewState.curCamPos);

    //qDebug("Elapsed: %ul %f", elapsed, (float)((float)elapsed/1000));
    mMesh->updateViewState(mViewState, ((float)elapsed)/1000);

    mViewState.pitch = 0.0f;
    mViewState.roll = 0.0f;
    mViewState.yaw = 0.0f;
}

///
/// input routines
///

void MainWindow::_mouseMove(QMouseEvent *event)
{    
    // welcome to cast and magic number hell.

    // also, this needs a complete re-working after the whole QT move due to the mouse co-ords no longer having sub-pixel precision.
    // movement just feels too chunky and i don't see any clear work arounds

    int deltaX;
    int deltaY;

    int width = this->width();
    int height = this->height();

    Qt::MouseButtons buttons = event->buttons();
    Qt::KeyboardModifiers keymod = QApplication::keyboardModifiers();

    float speed    = 30.0f;
    float camSpeed = speed * (float)(abs((int)trunc(mViewState.curCamPos.z)) * 0.05f);
    if ( camSpeed < 30.0f ) { camSpeed = 30.0f; }

    //qDebug("Cam Speed: %f", (float)(abs((int)trunc(mViewState.curCamPos.z)) * 0.05f) );
    //qDebug("Cam Pos: %f", mViewState.curCamPos.z );

    if ( mMouseFirstHit ) {
        mViewState.mouseLastX = event->x();
        mViewState.mouseLastY = event->y();
        mMouseFirstHit = false;
    }

    deltaX = (event->x() - mViewState.mouseLastX);
    deltaY = (event->y() - mViewState.mouseLastY);

    // due to the precision of the system, we are going to toss out the first movement,
    // unfortunately this makes the input feel a little sluggish HOWEVER this also makes
    // the movement get exaggerated.
    if ( (abs(deltaX) <= 1) && (abs(deltaY) <= 1) ) { return; }

    /*
    if ( deltaX != 0 ) {
        if ( deltaX < 0 ) { ++deltaX; } else { --deltaX; }
    }

    if ( deltaY != 0 ) {
        if ( deltaY < 0 ) { ++deltaY; } else { --deltaY; }
    }
    */

    // check lateral movement
    if ( deltaX > 0 ) {
        // right movement
        if ( buttons & Qt::RightButton ) {
            mViewState.roll = -(speed * ((float)abs(deltaX) / (float)width));
        } else if ( buttons & Qt::LeftButton ) {
            mViewState.yaw = speed * ((float)abs(deltaX) / (float)width);
        } else if ( (buttons & Qt::MidButton) && (keymod & Qt::ControlModifier) ) {
            mViewState.curCamPos.z -= camSpeed * ((float)abs(deltaX) / (float)width);
        } else if ( buttons & Qt::MidButton ) {
            mViewState.curCamPos.x -= camSpeed * ((float)abs(deltaX) / (float)width);
        }

    } else if (  deltaX < 0 ) {
        // left movement
        if ( buttons & Qt::RightButton ) {
            mViewState.roll = (speed * ((float)abs(deltaX) / (float)width));
        } else if ( buttons & Qt::LeftButton ) {
            mViewState.yaw = -(speed * ((float)abs(deltaX) / (float)width));
        } else if ( (buttons & Qt::MidButton) && (keymod & Qt::ControlModifier) ) {
            mViewState.curCamPos.z += camSpeed * ((float)abs(deltaX) / (float)width);
        } else if ( buttons & Qt::MidButton ) {
            mViewState.curCamPos.x += camSpeed * ((float)abs(deltaX) / (float)width);
        }
    }

    // NOTE: Think i want this to control null node yaw when ctrl is down
    // check vertical movement
    if ( deltaY > 0 ) {
        // down movement
        if ( (buttons & Qt::RightButton) || (buttons & Qt::LeftButton) ) {
            mViewState.pitch = speed * ((float)abs(deltaY) / (float)height);
        } else if ( buttons & Qt::MidButton ) {
            mViewState.curCamPos.y += camSpeed * ((float)abs(deltaY) / (float)height);
        }
    } else if ( deltaY < 0 ) {
        // up movement
        if ( (buttons & Qt::RightButton) || (buttons & Qt::LeftButton) ) {
            mViewState.pitch = -(speed * ((float)abs(deltaY) / (float)height) );
        } else if ( buttons & Qt::MidButton ) {
            mViewState.curCamPos.y -= camSpeed * ((float)abs(deltaY) / (float)height);
        }
    }

    // NOTE: Since porting to QT this is not working as smoothly and seems to be too
    // sensitive when moving slowly, a couple hacks are in place to cope with this
    //
    // in order to keep the movement a little more predictable we will
    // toss out any rotation for the axis with the smaller delta.
    //
    // @TODO possibly do the same kinda of stomping for camera movement.
    //
    if ( abs(deltaX) > abs(deltaY) ) { mViewState.pitch = 0.0f; } else
    if ( abs(deltaY) > abs(deltaX) ) { mViewState.yaw = 0.0f; }

    if ( (event->modifiers() & Qt::ControlModifier) ) {
        mViewState.pitch *= 0.2f;
        mViewState.yaw *= 0.2f;
        mViewState.roll *= 0.2f;
    }

    // update known mouse positions
    mViewState.mouseLastX = event->x();
    mViewState.mouseLastY = event->y();
}

void MainWindow::_mouseRelease(QMouseEvent *event)
{
    qDebug("Recieved release.");
    // HACK: this is used to track a state in the mouse presses and reset the lastX/lastY positions
    // to prevent jumping which has been introduced when moving to QT
    mMouseFirstHit = true;
}

void MainWindow::_mouseWheel(QWheelEvent *event)
{
    qDebug("Recieved wheel.");

    float step = 2 * ( abs( (int)trunc(mViewState.curCamPos.z) ) * 0.05f ) ;
    if ( step < 2.0f ) { step = 2.0f; }

    int deg   = event->delta() / 8;
    int steps = deg / 15;

    if ( steps > 0 ) {
        // forward
        mViewState.curCamPos.z -= step;
    } else if ( steps < 0 ) {
        // back
        mViewState.curCamPos.z += step;
    }
}
