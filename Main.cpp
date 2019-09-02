#include "MvApplication.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    MvApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
