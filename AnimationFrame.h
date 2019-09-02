#ifndef __ANIMATIONFRAME_H__
#define __ANIMATIONFRAME_H__

#include <QtGui/QFrame>

namespace Ui {
    class AnimationFrame;
}

class AnimationFrame : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(AnimationFrame)
public:

    enum TransportCommand { PLAY, PAUSE, STOP, LOOP_ON, LOOP_OFF, BOUNCE_ON, BOUNCE_OFF };

    explicit AnimationFrame(QWidget *parent = 0);
    virtual ~AnimationFrame();

    void reset();
    void setAnimationNames(QStringList animationNames);
    void setPosition(float position);
    void setAnimationLength(float length);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::AnimationFrame *mUi;

    void _updateLabel();

private slots:
    void _currentAnimationChanged(const QString& name);
    void _playTriggered();
    void _stopTriggered();
    void _loopToggled(bool enabled);
    void _timelineMoved(int value);
    void _timelineSliderPressed();
    void _timelineSliderReleased();

signals:
    void currentAnimationChanged(const QString& name);
    void commandTriggered(const AnimationFrame::TransportCommand& command);
    void positionChanged(float position);
};

#endif // __ANIMATIONFRAME_H__
