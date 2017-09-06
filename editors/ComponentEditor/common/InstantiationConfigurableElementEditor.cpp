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

#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor()
//-----------------------------------------------------------------------------
InstantiationConfigurableElementEditor::InstantiationConfigurableElementEditor(
    QSharedPointer<ConfigurableElementFinder> elementFinder, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> elementFormatter, QSharedPointer<ExpressionFormatter> parameterFormatter,
    QSharedPointer<ExpressionParser> elementParser, QSharedPointer<ExpressionParser> parameterParser,
    QAbstractItemModel* completionModel, QWidget* parent):
ConfigurableElementEditor(parameterFinder, elementFormatter, completionModel, parent),
model_(new ConfigurableElementsModel(parameterFinder, elementFormatter, parameterFormatter, elementParser,
    parameterParser, this)),
elementFinder_(elementFinder),
filter_(new ConfigurableElementsFilter(this))
{
    setModel(model_, filter_);
    hideUnnecessaryColumns();

    connect(getFilterSelectionBox(), SIGNAL(clicked(bool)),
        filter_, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);

    connect(getDelegate(), SIGNAL(removeConfigurableElement(QString const&, QString const&, int)),
        this, SLOT(sendSignalForElementRemoval(QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(getDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(getDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::~InstantiationConfigurableElementEditor()
//-----------------------------------------------------------------------------
InstantiationConfigurableElementEditor::~InstantiationConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::setParameters()
//-----------------------------------------------------------------------------
void InstantiationConfigurableElementEditor::setParameters(QString const& containerName,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedElements)
{
    model_->setParameters(
        containerName, parameters, QSharedPointer<QList<QSharedPointer<Choice> > >(), storedElements);

    elementFinder_->setConfigurableElementList(model_->getConfigurableElements());

    expandView();
}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void InstantiationConfigurableElementEditor::clear() 
{
	model_->clear();
}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::sendSignalForElementRemoval()
//-----------------------------------------------------------------------------
void InstantiationConfigurableElementEditor::sendSignalForElementRemoval(QString const& elementID,
    QString const& parentName, int elementRow)
{
    if (parentName.isEmpty())
    {
        model_->onRemoveItem(elementID, elementRow);
        filter_->invalidate();
    }
}
