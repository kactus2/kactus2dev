/* $Id$
 *
 *  Author: Antti Kamppi
 */

#include "libraryhandler.h"

#include <QApplication>


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    LibraryHandler LibraryWidget;
    LibraryWidget.resize(640, 480);
    LibraryWidget.show();
    return app.exec();

}


