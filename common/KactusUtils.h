//-----------------------------------------------------------------------------
// File: KactusUtils.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 26.02.2026
//
// Description:
// Common utility functions used by main Kactus2 application.
//-----------------------------------------------------------------------------

#pragma once

#include <QPixmap>
#include <QColor>
#include <QIcon>

namespace KactusUtils
{

    /*!
     *  Get an icon with given path stylized to current theme.
     *
     *    @param[in] srcPath   The icon source path.
     *
     *    @return An icon whose color is changed to match the system theme.
     */
    QIcon getIconStyledToTheme(QString const& srcPath);
}