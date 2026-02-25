//-----------------------------------------------------------------------------
// File: InstantiationConfigurableElementEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2017
//
// Description:
// Editor for configurable elements of a single category.
//-----------------------------------------------------------------------------

#include "InstantiationConfigurableElementEditor.h"

#include <editors/common/ExpressionSet.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>

#include <KactusAPI/include/ParameterConfigurableElementFinder.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor()
//-----------------------------------------------------------------------------
InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor(
    QSharedPointer<ParameterConfigurableElementFinder> elementFinder,
    ExpressionSet elementExpressions,
    ExpressionSet designExpressions,
    QAbstractItemModel* completionModel,
    QWidget* parent):
ConfigurableElementEditor(elementExpressions, designExpressions, completionModel, parent),
elementFinder_(elementFinder)
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::setParameters()
//-----------------------------------------------------------------------------
void InstantiationConfigurableElementEditor::setParameters(QString const& containerName,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, Document::Revision docRevision,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedElements)
{
    ConfigurableElementEditor::setParameters(containerName, parameters, 
        QSharedPointer<QList<QSharedPointer<Choice> > >(), docRevision, storedElements);

    elementFinder_->setCEVList(storedElements);
    elementFinder_->setParameterList(parameters);
}
