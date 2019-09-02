#ifndef __TIMELINE_H__
#define __TIMELINE_H__

#include <QSlider>
#include <QMouseEvent>

class Timeline : public QSlider
{
public:
    Timeline(QWidget* parent = 0);
};

#endif // __TIMELINE_H__
