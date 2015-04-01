//-----------------------------------------------------------------------------
// File: ImportColors.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.09.2014
//
// Description:
// Common highlight colors for imported elements.
//-----------------------------------------------------------------------------

#ifndef IMPORTCOLORS_H
#define IMPORTCOLORS_H

#include <QColor>
#include <common/KactusColors.h>

namespace ImportColors
{
    QColor const PORT = KactusColors::SW_COMPONENT;

    QColor const MODELPARAMETER = KactusColors::HW_BUS_COMPONENT;

    QColor const PARAMETER = KactusColors::MEMORY_BLOCK;

    QColor const VIEWNAME = QColor("Moccasin");
}

#endif // IMPORTCOLORS_H
