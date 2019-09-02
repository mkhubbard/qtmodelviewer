#ifndef __MAINWINDOW_H___
#define __MAINWINDOW_H__

#include <QtGui/QMainWindow>
#include <QActionGroup>

#include "OgreRenderWindow.h"
#include "AnimationFrame.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;    

    QActionGroup *agMesh;

    Mesh *mMesh;
    Grid *mGrid;

    ViewState mViewState;

    bool mAnimResume;
    bool mMouseFirstHit;

    void _tweakUi();
    void _connectSignals(); 
    void _resetScene();
    void _resetMeshControls();
    void _disableMeshControls(bool disabled);
    void _updateAnimTimeLabel(unsigned int position);

private slots:
    // general commands
    void _openMesh();
    void _closeMesh();

    // general toolbar
    void _toggleGrid(bool checked);
    void _toggleBoundingBox(bool checked);
    void _toggleSkeleton(bool checked);
    void _polygonModeChanged(QAction *action);    

    // animations
    void _currentAnimationChanged(const QString& name);
    void _transportCommandTriggered(const AnimationFrame::TransportCommand& command);
    void _animationChanged(float length);
    void _animationPositionChanged(float position);
    void _animationPositionSeek(float position);

    // render window
    void _beforeFrameUpdate(unsigned long elapsed);

    // mouse / keyboard input
    void _mouseMove(QMouseEvent *event);
    void _mouseRelease(QMouseEvent *event);
    void _mouseWheel(QWheelEvent *event);
};

#endif // __MAINWINDOW_H__
