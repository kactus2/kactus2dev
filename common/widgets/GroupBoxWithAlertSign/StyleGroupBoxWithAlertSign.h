//-----------------------------------------------------------------------------
// File: GroupBoxWithAlertSign.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Vasilii Feshchenko
// Date: 05.06.2024
//
// Description:
// Group box with an alert sign in the title.
// The main parts of the code have been created by Michael Scopchanov
// Git hub: https://github.com/scopchanov/SO-DecoratedGroupBox.git ,
// stackoverflow answer by Michael Scopchanov: https://stackoverflow.com/a/52630998 . 
// Read copyright below. 
//-----------------------------------------------------------------------------

/**
MIT License

Copyright (c) 2018 Michael Scopchanov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef STYLEGROUPBOXWITHALERTSIGN_H
#define STYLEGROUPBOXWITHALERTSIGN_H

#include <QCommonStyle>

//-----------------------------------------------------------------------------
//! Style for GroupBoxWithAlertSign
//-----------------------------------------------------------------------------
class StyleGroupBoxWithAlertSign : public QCommonStyle
{
	Q_OBJECT
public:
    /*!
     *	Constructor.
     */
    explicit StyleGroupBoxWithAlertSign();

    /*!
     *	Draws complex control for the group box with alert sign.
     *
     *    @param [in] cc         The complex control to draw.
     *    @param [in] opt        The style options for drawing.
     *    @param [in] p          The painter object for drawing.
     *    @param [in] widget     The widget to draw on.
     * 
     *      @remarks    The function is slightly altered to incorporate vertical offset.
     *                  To better understand structure of the funcition refer  to the source 
     *                  Git Hub page (check header comment for link).
     *                              
     */
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const override;
};
#endif // STYLEGROUPBOXWITHALERTSIGN_H