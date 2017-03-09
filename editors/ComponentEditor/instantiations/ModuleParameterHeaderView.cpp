//-----------------------------------------------------------------------------
// File: ModuleParameterHeaderView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.12.2014
//
// Description:
// Header view for module parameter editor.
//-----------------------------------------------------------------------------

#include "ModuleParameterHeaderView.h"

#include "ModuleParameterColumns.h"

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::ModelParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ModuleParameterHeaderView::ModuleParameterHeaderView(Qt::Orientation orientation, QWidget* parent /* = 0 */):
ParameterEditorHeaderView(orientation, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::~ModelParameterEditorHeaderView()
//-----------------------------------------------------------------------------
ModuleParameterHeaderView::~ModuleParameterHeaderView()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::descriptionColumn()
//-----------------------------------------------------------------------------
int ModuleParameterHeaderView::descriptionColumn() const
{
    return ModuleParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::valueColumn()
//-----------------------------------------------------------------------------
int ModuleParameterHeaderView::valueColumn() const
{
    return ModuleParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterEditorHeaderView::maximumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterHeaderView::maximumColumn() const
{
    return ModuleParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditorHeaderView::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterHeaderView::arrayRightColumn() const
{
    return ModuleParameterColumns::ARRAY_RIGHT;
}
