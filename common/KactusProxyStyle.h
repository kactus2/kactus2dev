//-----------------------------------------------------------------------------
// File: KactusProxyStyle.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 04.05.2026
//
// Description:
// Overrides the default style (fusion, kde, windows...) with customizations
// for Kactus2.
//-----------------------------------------------------------------------------
#pragma once

#include <QProxyStyle>

class KactusProxyStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    /*!
     *  Override certain icons of default style.
     *
     *    @param[in] standardIcon   The icon to override
     *    @param[in] option         The style option, which contains information for drawing.
     *    @param[in] widget         Widget being drawn.
     *
     *    @return The icon that should be drawn.
     */
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const override;
};
