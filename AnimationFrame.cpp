#include "AnimationFrame.h"
#include "ui_AnimationFrame.h"

AnimationFrame::AnimationFrame(QWidget *parent) :
    QFrame(parent),
    mUi(new Ui::AnimationFrame)
{
    mUi->setupUi(this);

    // connect signals
    connect(mUi->cbAnimationNames, SIGNAL(currentIndexChanged(QString)), this, SLOT(_currentAnimationChanged(QString)));

    connect(mUi->btPlay, SIGNAL(clicked()), this, SLOT(_playTriggered()));
    connect(mUi->btStop, SIGNAL(clicked()), this, SLOT(_stopTriggered()));
    connect(mUi->btLoop, SIGNAL(toggled(bool)), this, SLOT(_loopToggled(bool)));

    connect(mUi->timeline, SIGNAL(valueChanged(int)), this, SLOT(_timelineMoved(int)));
    connect(mUi->timeline, SIGNAL(sliderPressed()), this, SLOT(_timelineSliderPressed()));
    connect(mUi->timeline, SIGNAL(sliderReleased()), this, SLOT(_timelineSliderReleased()));
}

AnimationFrame::~AnimationFrame()
{
    delete mUi;
}

void AnimationFrame::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        mUi->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AnimationFrame::reset()
{
    mUi->btLoop->setChecked(false);
    mUi->timeline->setValue(0);
    mUi->timeline->setMaximum(0);

    _updateLabel();
}

void AnimationFrame::setAnimationNames(QStringList animationNames)
{
    if ( animationNames.count() == 0 ) {
        animationNames.append("No animations present");
    } else {
        animationNames.insert(0, "None");
    }

    mUi->cbAnimationNames->clear();
    mUi->cbAnimationNames->addItems(animationNames);
}

void AnimationFrame::setAnimationLength(float length)
{
    mUi->timeline->setValue(0);
    mUi->timeline->setMaximum( (ulong)(length * 1000) );

    _updateLabel();
}

void AnimationFrame::setPosition(float position)
{
    mUi->timeline->setValue( (ulong)(position * 1000) );
}

///
/// private methods
///

void AnimationFrame::_updateLabel()
{
    int ms  = (int) (mUi->timeline->value() % 1000);
    int sec = (int)((mUi->timeline->value() / 1000)  % 60);
    int min = (int)((mUi->timeline->value() / 60000) % 60);

    //unlikely to ever have HOURS of animation in the mesh
    //int hrs = (int)((time/3600000)   % 24);

    mUi->laTime->setText(
        QString("%1:%2.%3")
            .arg(min, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'))
            .arg(ms,  3, 10, QChar('0'))
    );
}


///
/// private slots
///

void AnimationFrame::_currentAnimationChanged(const QString& name)
{
    mUi->timeline->setValue(0);

    if ( mUi->cbAnimationNames->currentIndex() == 0 ) {
        mUi->btPlay->setEnabled(false);
        mUi->btStop->setEnabled(false);
        mUi->timeline->setEnabled(false);        
        emit currentAnimationChanged("");
    } else {
        mUi->btPlay->setEnabled(true);
        mUi->btStop->setEnabled(true);
        mUi->timeline->setEnabled(true);

        emit currentAnimationChanged(name);
    }
}

void AnimationFrame::_playTriggered()
{    
    emit commandTriggered(PLAY);
}

void AnimationFrame::_stopTriggered()
{    
    emit commandTriggered(STOP);
}

void AnimationFrame::_loopToggled(bool enabled)
{    
    if ( enabled ) {
        emit commandTriggered(LOOP_ON);
    } else {
        emit commandTriggered(LOOP_OFF);
    }
}

void AnimationFrame::_timelineMoved(int value)
{    
    _updateLabel();

    emit positionChanged( (float)abs(value)/1000 );
}

void AnimationFrame::_timelineSliderPressed()
{
    emit commandTriggered(PAUSE);
}

void AnimationFrame::_timelineSliderReleased()
{
    // nothing currently, may want to have this resume playback under
    // some circumstances
}
