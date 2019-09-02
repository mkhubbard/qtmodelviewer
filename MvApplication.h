#ifndef __MVAPPLICATION_H__
#define __MVAPPLICATION_H__

#include <QtGui/QApplication>

class MvApplication : public QApplication
{
public:
    MvApplication(int & argc, char ** argv);
    ~MvApplication();

    bool notify(QObject* receiver, QEvent* event);
};

#endif // __MVAPPLICATION_H__


