/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: summarylabel.cpp
 *		Project: Kactus 2
 */

#include "summarylabel.h"

#include <QFont>


 //-----------------------------------------------------------------------------
 // Function: SummaryLabel::SummaryLabel()
 //-----------------------------------------------------------------------------
SummaryLabel::SummaryLabel( const QString& text, QWidget *parent, bool showExtensionIcon):
QLabel(text, parent) {

	QFont usedFont = font();
	usedFont.setWeight(QFont::Bold);
	usedFont.setPointSize(usedFont.pointSize() + SummaryLabel::POINTSIZE_INCREMENT);
	setFont(usedFont);

    if (showExtensionIcon)
    {
        auto labelText = text;
        auto size = fontMetrics().height();
        setText("<html>" + labelText + "<img  src=':icons/common/graphics/appicon.png'"
            "width='" + QString::number(size) + "' height='" + QString::number(size) + "' ></html>");
        setToolTip(tr("This is Kactus2 extension"));
    }
}
