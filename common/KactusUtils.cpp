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
#include <QtGlobal>
#include <QStringBuilder>

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

QPixmap KactusUtils::getPixmapStyledToTheme(const QString& srcPath, QColor* colorOverride /*= nullptr*/)
{
    if (darkThemeEnabled())
    {
        QPixmap base(srcPath);
        auto pal = QApplication::palette();

        QColor newColor = pal.color(QPalette::ButtonText);
        if (colorOverride)
        {
            newColor = *colorOverride;
        }

        return getRecoloredPixmap(srcPath, newColor);
    }

    return QPixmap(srcPath);
}

QIcon KactusUtils::getIconStyledToTheme(QString const& srcPath, QColor* colorOverride /*= nullptr*/)
{
    if (darkThemeEnabled())
    {
        QIcon icon;
        icon.addPixmap(getPixmapStyledToTheme(srcPath, colorOverride));
        return icon;
    }

    return QIcon(srcPath);
}

bool KactusUtils::darkThemeEnabled()
{
// Dark mode support for kactus2 requires at least Qt 6.5
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    static const bool isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    return isDarkMode;
#endif
    return false;
}

void KactusUtils::applyThemeToPalette()
{
    // Get current theme
    auto appStyle = QApplication::style()->name();
    auto palette = QGuiApplication::palette();

    // Set main window colors
    auto windowBG = palette.color(QPalette::ColorRole::Window);
    KactusColors::DEFAULT_WINDOW_BG = windowBG;

    // Dark mode is not enabled for windows vista style
    if (darkThemeEnabled() && appStyle.compare("windowsvista") != 0)
    {

        // Set ribbon colors to lighter variants of window background color
        KactusColors::RIBBON_GRADIENT_TOP = windowBG;
        KactusColors::RIBBON_GRADIENT_BOTTOM = palette.color(QPalette::ColorRole::Window).lighter(125);
        KactusColors::RIBBONGROUP_TITLE_GRADIENT_TOP = windowBG;
        KactusColors::RIBBONGROUP_TITLE_GRADIENT_BOTTOM = palette.color(QPalette::ColorRole::Window).lighter(175);
        KactusColors::RIBBONGROUP_TITLE_TEXT = palette.color(QPalette::ColorRole::WindowText);

        // Set slightly muted highlight color (for selections)
        auto currentHighlight = palette.highlight().color();
        palette.setColor(QPalette::ColorRole::Highlight, currentHighlight.darker(150));
        QGuiApplication::setPalette(palette);

        // Set text color
        KactusColors::REGULAR_TEXT = palette.windowText().color();
        KactusColors::REGULAR_MESSAGE = KactusColors::REGULAR_TEXT;

        KactusColors::ERROR_COLOR = KactusColors::ERROR_COLOR.lighter(120);

        // Field colors
        KactusColors::MANDATORY_FIELD = KactusColors::MANDATORY_FIELD.darker(140);
        KactusColors::DISABLED_FIELD = KactusColors::DISABLED_FIELD.darker(300);
        KactusColors::REGULAR_FIELD = KactusColors::DEFAULT_WINDOW_BG;
        KactusColors::LOGICAL_PORT_FIELD = KactusColors::LOGICAL_PORT_FIELD.darker(200);
        KactusColors::STRONG_FIELD = KactusColors::STRONG_FIELD.darker(200);

        // Set colors for HW design
        KactusColors::DIAGRAM_GRID = palette.windowText().color().darker(250);
        KactusColors::REGULAR_CONNECTION = QColor(Qt::white).darker(150);
        KactusColors::BROKEN_CONNECTION = KactusColors::BROKEN_CONNECTION.darker(140);
        KactusColors::ADHOC_PORT = KactusColors::REGULAR_CONNECTION;
        KactusColors::HW_COMPONENT = KactusColors::HW_COMPONENT.darker(175);
        KactusColors::HW_BUS_COMPONENT = KactusColors::HW_BUS_COMPONENT.darker(125);
        KactusColors::HW_BUS_COMPONENT = KactusColors::HW_BUS_COMPONENT.darker(125);

        KactusColors::MASTER_INTERFACE = KactusColors::MASTER_INTERFACE.darker(120);
        KactusColors::MIRROREDMASTER_INTERFACE = KactusColors::MIRROREDMASTER_INTERFACE.darker(120);
        KactusColors::MIRROREDSLAVE_INTERFACE = KactusColors::MIRROREDSLAVE_INTERFACE.darker(130);

        KactusColors::DIAGRAM_COLUMN_HEADER = KactusColors::DIAGRAM_COLUMN_HEADER.darker(175);
        KactusColors::CONNECTION_UNDERCROSSING = KactusColors::CONNECTION_UNDERCROSSING.darker(140);

        KactusColors::ASSOCIATION_LINE = KactusColors::REGULAR_CONNECTION;

        KactusColors::DOCK_WIDGET_TITLE_BG = KactusColors::DEFAULT_WINDOW_BG.lighter(150);
        KactusColors::TABLE_GRIDLINE = KactusColors::DOCK_WIDGET_TITLE_BG;
    }

    // KactusColors contains default values for most original style if not using darkmode
}

QString KactusUtils::colorToRgbString(QColor const& color)
{
    return QStringLiteral("rgb(") % QString::number(color.red()) % QLatin1Char(',') %
        QString::number(color.green()) % QLatin1Char(',') % QString::number(color.blue()) %
        QLatin1Char(')');
}
