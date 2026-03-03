//-----------------------------------------------------------------------------
// File: KactusUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 26.02.2026
//
// Description:
// Common utility functions used by main Kactus2 application.
//-----------------------------------------------------------------------------

#include "KactusUtils.h"

#include <KactusAPI/include/KactusColors.h>

#include <QPainter>
#include <QApplication>
#include <QStyleHints>
#include <QPalette>
#include <QStyle>

static QPixmap getRecoloredPixmap(const QPixmap& src, const QColor& color)
{
    // Create an ARGB pixmap to draw on
    QPixmap result(src.size());
    result.fill(Qt::transparent);

    QPainter p(&result);

    // Fill with the target color
    p.fillRect(result.rect(), color);

    // Keep only the alpha from the source (masking)
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawPixmap(0, 0, src); // the src alpha is applied
    p.end();

    return result;
}

QIcon KactusUtils::getIconStyledToTheme(QString const& srcPath)
{
    if (darkThemeEnabled())
    {
        QPixmap base(srcPath);
        QIcon icon;
        auto pal = QApplication::palette();
        auto normalCol = pal.color(QPalette::ButtonText);
        
        icon.addPixmap(getRecoloredPixmap(base, normalCol), QIcon::Normal, QIcon::Off);
        return icon;
    }

    return QIcon(srcPath);
}

bool KactusUtils::darkThemeEnabled()
{
    static const bool isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    return isDarkMode;
}

void KactusUtils::applyThemeToPalette()
{
    // Get current theme
    auto appStyle = QApplication::style()->name();

    // Dark mode is not enabled for windows vista style
    if (darkThemeEnabled() && appStyle.compare("windowsvista") != 0)
    {
        // Set main window colors
        auto palette = QGuiApplication::palette();
        auto windowBG = palette.color(QPalette::ColorRole::Window);

        // Set ribbon colors to lighter variants of window background color
        KactusColors::RibbonTheme::GRADIENTTOP = windowBG;
        KactusColors::RibbonTheme::GRADIENTBOTTOM = palette.color(QPalette::ColorRole::Window).lighter(125);
        KactusColors::RibbonTheme::GROUPTITLEGRADIENTTOP = windowBG;
        KactusColors::RibbonTheme::GROUPTITLEGRADIENTBOTTOM = palette.color(QPalette::ColorRole::Window).lighter(175);
        KactusColors::RibbonTheme::GROUPTITLETEXT = palette.color(QPalette::ColorRole::WindowText);

        // Set slightly muted highlight color (for selections)
        auto currentHighlight = palette.highlight().color();
        palette.setColor(QPalette::ColorRole::Highlight, currentHighlight.darker(150));
        QGuiApplication::setPalette(palette);

        // Set text color
        KactusColors::REGULAR_TEXT = palette.windowText().color();
        KactusColors::REGULAR_MESSAGE = KactusColors::REGULAR_TEXT;

        // Set colors for HW design
        KactusColors::DIAGRAM_GRID = palette.windowText().color().darker(250);
        KactusColors::REGULAR_CONNECTION = QColor(Qt::white).darker(150);
        KactusColors::HW_COMPONENT = KactusColors::HW_COMPONENT.darker(175);
        KactusColors::DIAGRAM_COLUMN_HEADER = KactusColors::DIAGRAM_COLUMN_HEADER.darker(175);
    }
    // KactusColors contains default values for original style if not using darkmode
}
