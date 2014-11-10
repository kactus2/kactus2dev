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
// Function: NamelabelWidth::getTextLength
//-----------------------------------------------------------------------------
qreal NamelabelWidth::getTextLength( QString text, QFont font )
{
	QFontMetrics fontMetrics(font);

	return fontMetrics.width( text );
}

//-----------------------------------------------------------------------------
// Function: NamelabelWidth::setNameLabel
//-----------------------------------------------------------------------------
QString NamelabelWidth::setNameLabel( QString labelName, QFont font, qreal width )
{
	QFontMetrics fontMetrics(font);
	qreal shortWidth = fontMetrics.width(labelName);
	QString shortLabel = labelName;

	if ( shortWidth + MIDDLE + PADDING > width )
	{
	    for (int i = 1; i < labelName.size(); ++i)
    	{
	    	shortLabel = labelName.left(labelName.size() - i);
		    shortWidth = fontMetrics.width(shortLabel);

    		if ( shortWidth + MIDDLE + PADDING < width  )
	    	{
		    	// Replace the last letters with ...
			    return shortLabel.left(shortLabel.size() - 2) + "...";
    		}
	    }
	}

	return shortLabel;
}