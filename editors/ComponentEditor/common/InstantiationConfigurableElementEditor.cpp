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

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>

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

    connect(filterSelection_, SIGNAL(clicked(bool)),
        filter_, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);

    connect(delegate_, SIGNAL(removeConfigurableElement(QString const&, int)),
        this, SLOT(sendSignalForElementRemoval(QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(dataChangedInID(QString const&, QString const&)),
        model_, SLOT(emitDataChangeForID(QString const&, QString const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
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
     int elementRow)
{
	model_->onRemoveItem(elementID, elementRow);
	filter_->invalidate();
}
