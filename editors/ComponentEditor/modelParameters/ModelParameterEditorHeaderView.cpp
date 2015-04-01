//-----------------------------------------------------------------------------
// File: ModelParameterEditorHeaderView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.12.2014
//
// Description:
// Header view for model parameter editor.
//-----------------------------------------------------------------------------

#include "ModelParameterEditorHeaderView.h"

#include "ModelParameterColumns.h"

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::ModelParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ModelParameterEditorHeaderView::ModelParameterEditorHeaderView(Qt::Orientation orientation, QWidget* parent /* = 0 */):
ParameterEditorHeaderView(orientation, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::~ModelParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ModelParameterEditorHeaderView::~ModelParameterEditorHeaderView()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::descriptionColumn()
//-----------------------------------------------------------------------------
int ModelParameterEditorHeaderView::descriptionColumn() const
{
    return ModelParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::valueColumn()
//-----------------------------------------------------------------------------
int ModelParameterEditorHeaderView::valueColumn() const
{
    return ModelParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::maximumColumn()
//-----------------------------------------------------------------------------
int ModelParameterEditorHeaderView::maximumColumn() const
{
    return ModelParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModelParameterEditorHeaderView::arrayRightColumn() const
{
    return ModelParameterColumns::ARRAY_RIGHT;
}
