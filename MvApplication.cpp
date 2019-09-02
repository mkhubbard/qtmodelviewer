#include <QDebug>

#include "MvApplication.h"

MvApplication::MvApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

MvApplication::~MvApplication()
{
}

bool MvApplication::notify(QObject* receiver, QEvent* event)
{
    try {
         return QApplication::notify(receiver,event);
    } catch (...) {
        qDebug() << "BIPSApplication caught an exception from" <<
        receiver->objectName() << "from event type" << event->type();
        qFatal("Exiting due to exception caught");
   }
}
