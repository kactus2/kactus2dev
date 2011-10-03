#include <QApplication>
#include "listmanager.h"


#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ListManager w("List");
    w.show();

    return a.exec();
}

