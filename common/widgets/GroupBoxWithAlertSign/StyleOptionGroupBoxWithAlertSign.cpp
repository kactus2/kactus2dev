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
//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::StyleOptionGroupBoxWithAlertSign()
//-----------------------------------------------------------------------------
StyleOptionGroupBoxWithAlertSign::StyleOptionGroupBoxWithAlertSign() :
    QStyleOptionGroupBox(),
    offset_(0),
    leftSpace_(0),
    rightSpace_(0)
{
    // Constructor
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::getPixmap()
//-----------------------------------------------------------------------------
QPixmap StyleOptionGroupBoxWithAlertSign::getPixmap() const {
    return pixmap_;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::setPixmap()
//-----------------------------------------------------------------------------
void StyleOptionGroupBoxWithAlertSign::setPixmap(QPixmap pixmap) {
    pixmap_ = pixmap;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::getOffset()
//-----------------------------------------------------------------------------
int StyleOptionGroupBoxWithAlertSign::getOffset() const {
    return offset_;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::setOffset()
//-----------------------------------------------------------------------------
void StyleOptionGroupBoxWithAlertSign::setOffset(int offset) {
    offset_ = offset;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::getLeftSpace()
//-----------------------------------------------------------------------------
int StyleOptionGroupBoxWithAlertSign::getLeftSpace() const {
    return leftSpace_;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::setLeftSpace()
//-----------------------------------------------------------------------------
void StyleOptionGroupBoxWithAlertSign::setLeftSpace(int leftSpace) {
    leftSpace_ = leftSpace;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::getRightSpace()
//-----------------------------------------------------------------------------
int StyleOptionGroupBoxWithAlertSign::getRightSpace() const {
    return rightSpace_;
}

//-----------------------------------------------------------------------------
// Function: StyleOptionGroupBoxWithAlertSign::setRightSpace()
//-----------------------------------------------------------------------------
void StyleOptionGroupBoxWithAlertSign::setRightSpace(int rightSpace) {
    rightSpace_ = rightSpace;
}