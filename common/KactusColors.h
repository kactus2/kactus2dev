//-----------------------------------------------------------------------------
// File: KactusColors.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.6.2012
//
// Description:
// Common colors used in Kactus2.
//-----------------------------------------------------------------------------

#ifndef KACTUSCOLORS_H
#define KACTUSCOLORS_H

#include <QColor>

namespace KactusColors
{
    //! Color used for selections in design diagrams.
    QColor const DIAGRAM_SELECTION = Qt::blue;

    //! Color used for broken/invalid connections in design diagrams.
    QColor const BROKEN_CONNECTION = Qt::red;

    //! Special color used for COM connections.
    QColor const COM_CONNECTION = QColor(49, 132, 61);//QColor(136, 206, 134);

    //! Color used for allowed visualization in design diagrams.
    QColor const DIAGRAM_ALLOWED_INTERFACE = Qt::magenta; //QColor(0, 220, 0);
}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
