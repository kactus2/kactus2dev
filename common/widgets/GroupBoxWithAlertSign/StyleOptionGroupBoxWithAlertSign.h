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

#ifndef STYLEOPTIONGROUPBOXWITHALERTSIGN_H
#define STYLEOPTIONGROUPBOXWITHALERTSIGN_H

#include <QStyleOptionGroupBox>
#include <QPixmap>

//-----------------------------------------------------------------------------
//! Style Option for GroupBoxWithAlertSign
//-----------------------------------------------------------------------------
class StyleOptionGroupBoxWithAlertSign : public QStyleOptionGroupBox
{
public:
	/*!
	 *	Constructor.
	 */
	explicit StyleOptionGroupBoxWithAlertSign();

	/*!
	 *	Retrieves the pixmap used for the alert icon.
	 *
	 *		@return The current pixmap of the alert icon.
	 */
	QPixmap getPixmap() const;

	/*!
	 *	Sets the pixmap for the alert icon.
	 *
	 *		@param	[in]	pixmap		The pixmap to set for the alert icon.
	 */
	void setPixmap(QPixmap pixmap);

	/*!
	 *	Retrieves the offset for the alert icon.
	 *
	 *		@return The current offset for the alert icon.
	 */
	int getOffset() const;

	/*!
	 *	Sets the offset for the alert icon.
	 *
	 *		@param	[in]	offset		The new offset to set for the alert icon.
	 */
	void setOffset(int offset);

	/*!
	 *	Retrieves the left spacing for the alert icon.
	 *
	 *		@return The current left spacing for the alert icon.
	 */
	int getLeftSpace() const;

	/*!
	 *	Sets the left spacing for the alert icon.
	 *
	 *		@param	[in]	leftSpace	The new left spacing to set for the alert icon.
	 */
	void setLeftSpace(int leftSpace);

	/*!
	 *	Retrieves the right spacing for the alert icon.
	 *
	 *		@return The current right spacing for the alert icon.
	 */
	int getRightSpace() const;

	/*!
	 *	Sets the right spacing for the alert icon.
	 *
	 *		@param	[in]	rightSpace	The new right spacing to set for the alert icon.
	 */
	void setRightSpace(int rightSpace);

private:
	//! Pixmap of the alert icon.
	QPixmap pixmap_;

	//! Offset for the alert icon.
	int offset_;

	//! Left space for the alert icon.
	int leftSpace_;

	//! Right space for the alert icon.
	int rightSpace_;
};

#endif // STYLEOPTIONGROUPBOXWITHALERTSIGN_H