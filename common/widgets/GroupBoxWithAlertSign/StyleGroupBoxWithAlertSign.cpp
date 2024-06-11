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

#include<common/widgets/GroupBoxWithAlertSign/StyleOptionGroupBoxWithAlertSign.h>
#include<common/widgets/GroupBoxWithAlertSign/GroupBoxWithAlertSign.h>
#include<common/widgets/GroupBoxWithAlertSign/StyleGroupBoxWithAlertSign.h>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: StyleGroupBoxWithAlertSign::StyleGroupBoxWithAlertSign()
//-----------------------------------------------------------------------------
StyleGroupBoxWithAlertSign::StyleGroupBoxWithAlertSign() :
    QCommonStyle()
{

}

//-----------------------------------------------------------------------------
// Function: StyleGroupBoxWithAlertSign::drawComplexControl()
//-----------------------------------------------------------------------------
void StyleGroupBoxWithAlertSign::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const {
    if (const StyleOptionGroupBoxWithAlertSign* groupBox = qstyleoption_cast<const StyleOptionGroupBoxWithAlertSign*>(opt)) {
        QRect textRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel, widget);
        QRect checkBoxRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox, widget);
        int decorationOffset = 0;
        int pixmapRectWidth = 0;
        int pixmapHeight = 0;
        int textOffset = 0;

        if (!groupBox->getPixmap().isNull()) {
            decorationOffset = groupBox->getOffset();
            pixmapRectWidth = groupBox->getLeftSpace()
                + groupBox->getPixmap().width()
                + groupBox->getRightSpace();
            pixmapHeight = groupBox->getPixmap().height();
            textOffset = decorationOffset + pixmapRectWidth;
        }

        textRect.adjust(textOffset, 0, textOffset, 0);


        if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
            QStyleOptionFrame frame;
            frame.QStyleOption::operator=(*groupBox);
            frame.features = groupBox->features;
            frame.lineWidth = groupBox->lineWidth;
            frame.midLineWidth = groupBox->midLineWidth;
            frame.rect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxFrame, widget);
            p->save();
            QRegion region(groupBox->rect);
            if (!groupBox->text.isEmpty()) {
                bool ltr = groupBox->direction == Qt::LeftToRight;
                QRect finalRect = groupBox->subControls & QStyle::SC_GroupBoxCheckBox ?
                    checkBoxRect.united(textRect) :
                    textRect;
                finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
                region -= finalRect;
            }
            p->setClipRegion(region);
            proxy()->drawPrimitive(PE_FrameGroupBox, &frame, p, widget);
            p->restore();
        }


        if (!groupBox->getPixmap().isNull()) {
            const GroupBoxWithAlertSign* groupBoxWidget = qobject_cast<const GroupBoxWithAlertSign*>(widget);
            int verticalOffset = groupBoxWidget ? groupBoxWidget->getVerticalIconOffset() : 0;
            p->fillRect(decorationOffset, verticalOffset, pixmapRectWidth, pixmapHeight, opt->palette.window().color());
            proxy()->drawItemPixmap(p, QRect(decorationOffset, verticalOffset, pixmapRectWidth, pixmapHeight),
                Qt::AlignCenter, groupBox->getPixmap());
        }


        if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
            QColor textColor = groupBox->textColor;
            if (textColor.isValid())
                p->setPen(textColor);
            int alignment = int(groupBox->textAlignment);
            if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;
            proxy()->drawItemText(p, textRect, Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                groupBox->palette, groupBox->state & State_Enabled, groupBox->text,
                textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
            if (groupBox->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*groupBox);
                fropt.rect = textRect;
                proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
            }
        }
    }
    else {
        QCommonStyle::drawComplexControl(cc, opt, p, widget);
    }
}