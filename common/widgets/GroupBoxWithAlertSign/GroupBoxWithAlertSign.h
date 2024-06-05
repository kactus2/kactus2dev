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

#ifndef GROUPBOXWITHALERTSIGN_H
#define GROUPBOXWITHALERTSIGN_H

#include <QGroupBox>
#include <QString>


 //-----------------------------------------------------------------------------
 //! Group box with an alert sign in the title.
 //-----------------------------------------------------------------------------
class GroupBoxWithAlertSign : public QGroupBox
{
	Q_OBJECT

public:
	/*!
	 *	Constructor.
	 *
	 *		@param	[in]	title		The title of the group box.
	 *		@param	[in]	parent		The parent widget of the group box. Defaults to nullptr.
	 */
	explicit GroupBoxWithAlertSign(QString title, QWidget* parent = nullptr);

	/*!
	 *	Retrieves the pixmap used for the alert icon.
	 *
	 *		@return	The pixmap representing the alert icon.
	 */
	QPixmap getPixmap() const;

	/*!
	 *	Sets the pixmap for the alert icon.
	 *
	 *		@param	[in]	pixmap		The new pixmap to use for the alert icon.
	 */
	void setPixmap(const QPixmap& pixmap);

	/*!
	 *	Retrieves the offset for the alert icon from the left boundary.
	 *
	 *		@return	The current offset for the alert icon.
	 */
	int getOffset() const;

	/*!
	 *	Sets the offset for the alert icon from the left boundary.
	 *
	 *		@param	[in]	offset		The new offset to set for the alert icon.
	 */
	void setOffset(int offset);

	/*!
	 *	Retrieves the left spacing for the alert icon.
	 *
	 *		@return	The current left spacing for the icon.
	 */
	int getLeftSpace() const;

	/*!
	 *	Sets the left spacing for the alert icon.
	 *
	 *		@param	[in]	leftSpace	The new left spacing to set for the icon.
	 */
	void setLeftSpace(int leftSpace);

	/*!
	 *	Retrieves the right spacing for the alert icon.
	 *
	 *		@return	The current right spacing for the icon.
	 */
	int getRightSpace() const;

	/*!
	 *	Sets the right spacing for the alert icon.
	 *
	 *		@param	[in]	rightSpace	The new right spacing to set for the icon.
	 */
	void setRightSpace(int rightSpace);

	/*!
	 *	Sets both the left and right spacing for the alert icon simultaneously.
	 *
	 *		@param	[in]	leftSpace	The left spacing to set.
	 *		@param	[in]	rightSpace	The right spacing to set.
	 */
	void setSpaces(int leftSpace, int rightSpace);

	/*!
	 *	Retrieves the vertical offset for the alert icon.
	 *
	 *		@return	The current vertical offset for the alert icon.
	 */
	int getVerticalIconOffset() const;

	/*!
	 *	Sets the vertical offset for the alert icon.
	 *
	 *		@param	[in]	verticalOffset	The new vertical offset to set for the icon.
	 */
	void setVerticalIconOffset(int verticalOffset);

protected:
	/*!
	 *	Custom paint event to handle the drawing of the group box with the alert icon.
	 *
	 *		@param	[in]	event		The paint event.
	 */
	void paintEvent(QPaintEvent* event) override;

	/*!
	 *	Sets the alert icon to a predefined icon. This method configures the pixmap
	 *	used for the alert based on a standard icon.
	 */
	void setAlertIcon();

	/*!
	 *	Removes the alert icon, making it invisible within the group box.
	 */
	void removeAlertIcon();

private:
	//! Pixmap for the alert icon.
	QPixmap pixmap_;

	//! Offset for the icon.
	int offset_;

	//! Left space for the icon.
	int leftSpace_;

	//! Right space for the icon.
	int rightSpace_;

	//! Vertical offset for the icon.
	int verticalIconOffset_;
};

#endif // GROUPBOXWITHALERTSIGN_H