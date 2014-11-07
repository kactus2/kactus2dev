//-----------------------------------------------------------------------------
// File: NamelabelWidth.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 5.11.2014
//
// Description:
// Label width calculator for ConnectionEndPoints derived classes.
//-----------------------------------------------------------------------------

#ifndef NAMELABELWIDTH_H
#define NAMELABELWIDTH_H

#include <QGraphicsTextItem>
#include <common/graphicsItems/ComponentItem.h>

#include <QFontMetrics>
#include <QFont>

//-----------------------------------------------------------------------------
//! NamelabelWidth class.
//-----------------------------------------------------------------------------
class NamelabelWidth
{
public:
	/*!
	 *  Constructor.
	 */
	NamelabelWidth();

	/*!
	 *  Destructor.
	 */
	~NamelabelWidth();

	/*!
	 *  Returns the limit for the name label.
	 */
	static qreal setLabelLimit( qreal labelWidth );

	/*!
	 *  Returns the corrected name label.
	 */
	static QString setLabelText ( QString labelName, QFont font );

private:
	//! The size of the middle section of the component, with padding of labels taken into account.
	static const int middleColumn = 20+20;
};

#endif // NAMELABELWIDTH_H
