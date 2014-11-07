//-----------------------------------------------------------------------------
// File: NamelabelWidth.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 5.11.2014
//
// Description:
// Label width calculator for ConnectionEndPoints derived classes.
//-----------------------------------------------------------------------------

#include "NamelabelWidth.h"

//-----------------------------------------------------------------------------
// Function: NamelabelWidth::NamelabelWidth
//-----------------------------------------------------------------------------
NamelabelWidth::NamelabelWidth()
{
}

//-----------------------------------------------------------------------------
// Function: NamelabelWidth::~NamelabelWidth
//-----------------------------------------------------------------------------
NamelabelWidth::~NamelabelWidth()
{
}

//-----------------------------------------------------------------------------
// Function: NamelabelWidth::setLabelLimit
//-----------------------------------------------------------------------------
qreal NamelabelWidth::setLabelLimit(qreal labelWidth)
{
	// If the label width is larger, reduce the size of the label
	if ((labelWidth + ( middleColumn / 2 )) > (ComponentItem::COMPONENTWIDTH / 2))
	{
		labelWidth = ComponentItem::COMPONENTWIDTH / 2 - middleColumn / 2;
	}

	return labelWidth;
}

//-----------------------------------------------------------------------------
// Function: NamelabelWidth::setLabelText
//-----------------------------------------------------------------------------
QString NamelabelWidth::setLabelText( QString labelName, QFont font )
{
	QFontMetrics fontMetrics (font);
	qreal shortWidth = fontMetrics.width(labelName);

	qreal givenSpace = setLabelLimit(shortWidth);

	// If the width of the label is greater than the given space, the label is shortened.
	if ( shortWidth > givenSpace)
	{
		QString shortlabel = "";

		for (int i = 1; i < labelName.size(); ++i)
		{
			shortlabel = labelName.left(labelName.size() - i);

			shortWidth = fontMetrics.width(shortlabel);

			if ( shortWidth < givenSpace)
			{
				// Replace the last letters with ...
				return shortlabel.left(shortlabel.size() - 2) + "...";
			}
		}
	}
	
	return labelName;
}