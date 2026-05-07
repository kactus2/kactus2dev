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
     *  Get a pixmap with given path stylized to current theme. Returns default pixmap
     *  if dark mode is not enabled, and stylized pixmap for dark mode.
     *
     *    @param[in] srcPath        The pixmap source path.
     *    @param[in] colorOverride  Optional color to override default derived color.
     *
     *    @return A pixmap whose color is changed to match the system theme.
     */
    QPixmap getPixmapStyledToTheme(const QString& srcPath, QColor* colorOverride = nullptr);

    /*!
     *  Get an icon with given path stylized to current theme. Returns default icon
     *  if dark mode is not enabled, and stylized icon for dark mode.
     *
     *    @param[in] srcPath        The icon source path.
     *    @param[in] colorOverride  Optional color to override default derived color.
     *
     *    @return An icon whose color is changed to match the system theme.
     */
    QIcon getIconStyledToTheme(QString const& srcPath, QColor* colorOverride = nullptr);

    /*!
     *  Checks if dark mode is enabled.
     * 
     *    @return True, if darkmode is enabled
     */
    bool darkThemeEnabled();

    /*!
     *	Set palette colors based on platform style and theme with slight modifications.
     */
    void applyThemeToPalette();

    QString colorToRgbString(QColor const& color);
}