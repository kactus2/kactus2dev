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

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor()
//-----------------------------------------------------------------------------
InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor(
     QSharedPointer<ConfigurableElementFinder> elementFinder, 
    ExpressionSet elementExpressions,
    ExpressionSet parameterExpressions,
//     QSharedPointer<ParameterFinder> parameterFinder,
//     QSharedPointer<ExpressionFormatter> elementFormatter,
//     QSharedPointer<ExpressionFormatter> parameterFormatter,
//     QSharedPointer<ExpressionParser> elementParser,
//     QSharedPointer<ExpressionParser> parameterParser,
    QAbstractItemModel* completionModel, QWidget* parent):
    ConfigurableElementEditor(elementExpressions, parameterExpressions,
        completionModel, parent),
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

    elementFinder_->setConfigurableElementList(model_->getConfigurableElements());
}
