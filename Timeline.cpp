#include "Timeline.h"
#include "ProxyStyle.h"

class TimelineStyle : public ProxyStyle
{
public:
    TimelineStyle(const QString& baseStyle) : ProxyStyle(baseStyle) { }

    int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0, const QWidget* widget = 0, QStyleHintReturn* returnData = 0) const
    {
        // enables 'click anywhere' slider positioning
        if (hint == QStyle::SH_Slider_AbsoluteSetButtons) {
            return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
        }

        return ProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

Timeline::Timeline(QWidget* parent)
        : QSlider(parent)
{               
    this->setStyle( new TimelineStyle(this->style()->objectName()) );
}
