

#include <QApplication>
#include"director.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Director w;
    w.show();
    return a.exec();
}
