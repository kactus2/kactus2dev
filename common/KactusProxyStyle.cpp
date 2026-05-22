//-----------------------------------------------------------------------------
// File: KactusProxyStyle.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 04.05.2026
//
// Description:
// Overrides the default style (fusion, kde, windows...) with customizations
// for Kactus2.
//-----------------------------------------------------------------------------

#include "KactusProxyStyle.h"

#include <common/KactusUtils.h>

#include <QApplication>
#include <QDockWidget>

QIcon KactusProxyStyle::standardIcon(StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const
{
    // Adjust dock widget icon colors for dark theme
    QScopedPointer<QColor> dockWidgetIconColor;
    if (KactusUtils::darkThemeEnabled())
    {
        QColor* newColor = new QColor();
        *newColor = QApplication::palette().color(QPalette::ButtonText).darker(180);
        dockWidgetIconColor.reset(newColor);
    }

    // Override certain icons in the style. In this case dock widget title bar icons.
    switch (standardIcon)
    {
    case SP_TitleBarNormalButton:
        if (auto dockWidget = qobject_cast<const QDockWidget*>(widget))
        {
            if (dockWidget->isFloating())
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/dock-widget-attach.png", dockWidgetIconColor.data());
            }
            else
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/dock-widget-detach.png", dockWidgetIconColor.data());
            }
        }
        break;
    case SP_TitleBarMinButton:
        return QProxyStyle::standardIcon(standardIcon, option, widget);
    case SP_TitleBarCloseButton:
    case SP_DockWidgetCloseButton:
        return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/dock-widget-close.png", dockWidgetIconColor.data());
    default:
        break;
    }

    return QProxyStyle::standardIcon(standardIcon, option, widget);
}