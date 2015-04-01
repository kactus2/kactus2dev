//-----------------------------------------------------------------------------
// File: ParameterEditorHeaderView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.12.2014
//
// Description:
// Header view for parameter editor.
//-----------------------------------------------------------------------------

#include "ParameterEditorHeaderView.h"
#include "ParameterColumns.h"

#include <QPainter>

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::ParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ParameterEditorHeaderView::ParameterEditorHeaderView(Qt::Orientation orientation, QWidget* parent /* = 0 */):
QHeaderView(orientation, parent)
{
    
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::~ParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ParameterEditorHeaderView::~ParameterEditorHeaderView()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::paintSection()
//-----------------------------------------------------------------------------
void ParameterEditorHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if ( logicalIndex == maximumColumn() || logicalIndex == valueColumn() || logicalIndex == arrayRightColumn() )
    {
        QPen oldPen = painter->pen();
        QPen newPen(Qt::lightGray);
        newPen.setWidth(2);

        painter->setPen(newPen);
        painter->drawLine(rect.topRight(), rect.bottomRight());
        painter->setPen(oldPen);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::descriptionColumn()
//-----------------------------------------------------------------------------
int ParameterEditorHeaderView::descriptionColumn() const
{
    return ParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::valueColumn()
//-----------------------------------------------------------------------------
int ParameterEditorHeaderView::valueColumn() const
{
    return ParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::maximumColumn()
//-----------------------------------------------------------------------------
int ParameterEditorHeaderView::maximumColumn() const
{
    return ParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::arrayRightColumn()
//-----------------------------------------------------------------------------
int ParameterEditorHeaderView::arrayRightColumn() const
{
    return ParameterColumns::ARRAY_RIGHT;
}
