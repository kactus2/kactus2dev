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

#include<common/widgets/GroupBoxWithAlertSign/GroupBoxWithAlertSign.h>
#include<common/widgets/GroupBoxWithAlertSign/StyleGroupBoxWithAlertSign.h>
#include<common/widgets/GroupBoxWithAlertSign/StyleOptionGroupBoxWithAlertSign.h>
#include <QVBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QPainter>


//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::GroupBoxWithAlertSign()
//-----------------------------------------------------------------------------
GroupBoxWithAlertSign::GroupBoxWithAlertSign(QString title, QWidget* parent) :
	QGroupBox(title, parent),
	offset_(5),
	leftSpace_(3),
	rightSpace_(3),
	verticalIconOffset_(-5)
{

}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::getPixmap()
//-----------------------------------------------------------------------------
QPixmap GroupBoxWithAlertSign::getPixmap() const
{
	return pixmap_;
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setPixmap()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setPixmap(const QPixmap& pixmap)
{
	pixmap_ = pixmap;
	update();
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::getOffset()
//-----------------------------------------------------------------------------
int GroupBoxWithAlertSign::getOffset() const
{
	return offset_;
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setOffset()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setOffset(int offset)
{
	offset_ = offset;
	update();
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::getLeftSpace()
//-----------------------------------------------------------------------------
int GroupBoxWithAlertSign::getLeftSpace() const
{
	return leftSpace_;
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setLeftSpace()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setLeftSpace(int leftSpace)
{
	leftSpace_ = leftSpace;
	update();
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::getRightSpace()
//-----------------------------------------------------------------------------
int GroupBoxWithAlertSign::getRightSpace() const
{
	return rightSpace_;
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setRightSpace()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setRightSpace(int rightSpace)
{
	rightSpace_ = rightSpace;
	update();
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setSpaces()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setSpaces(int leftSpace, int rightSpace)
{
	setLeftSpace(leftSpace);
	setRightSpace(rightSpace);
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::paintEvent()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::paintEvent(QPaintEvent* /*event*/)
{
	StyleGroupBoxWithAlertSign style;
	QPainter painter(this);
	StyleOptionGroupBoxWithAlertSign option;

	initStyleOption(&option);
	option.setPixmap(pixmap_);
	option.setOffset(offset_);
	option.setLeftSpace(leftSpace_);
	option.setRightSpace(rightSpace_);

	style.drawComplexControl(QStyle::CC_GroupBox, &option, &painter, this);
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setAlertIcon()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setAlertIcon()
{
	setPixmap(QIcon(":/icons/common/graphics/exclamation--frame.png").pixmap(QSize(22, 22)));
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::removeAlertIcon()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::removeAlertIcon()
{
	setPixmap(QPixmap());
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::getVerticalIconOffset()
//-----------------------------------------------------------------------------
int GroupBoxWithAlertSign::getVerticalIconOffset() const {
	return verticalIconOffset_;
}

//-----------------------------------------------------------------------------
// Function: GroupBoxWithAlertSign::setVerticalIconOffset()
//-----------------------------------------------------------------------------
void GroupBoxWithAlertSign::setVerticalIconOffset(int verticalOffset) {
	verticalIconOffset_ = verticalOffset;
	update();
}