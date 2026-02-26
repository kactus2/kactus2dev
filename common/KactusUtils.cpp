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

#include <QPainter>
#include <QApplication>
#include <QPalette>

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
    QPixmap base(srcPath);
    QIcon icon;
    auto pal = QApplication::palette();
    auto normalCol = pal.color(QPalette::ButtonText);

    icon.addPixmap(getRecoloredPixmap(base, normalCol), QIcon::Normal, QIcon::Off);
    return icon;
}
