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
	 *  Return the length of the given text.
	 *      @param [in] text   The text used.
	 *      @param [in] font   The font used in the text.
	 */
	static qreal getTextLength (QString text, QFont font );

	/*!
	 *  Shortens the given label name according to the given width.
	 *
	 *      @param [in] labelName   The text in the label.
	 *      @param [in] font        The font used in the label.
	 *      @param [in] width       The width to which the label will be shortened.
	 */
	static QString setNameLabel (QString labelName, QFont font, qreal width);

private:
	//! The size of the middle section of the component, with padding of labels taken into account.
	static const int middleColumn = 20+20;

	static const int MIDDLE = 10;
	static const int PADDING = 10;
};

#endif // NAMELABELWIDTH_H
