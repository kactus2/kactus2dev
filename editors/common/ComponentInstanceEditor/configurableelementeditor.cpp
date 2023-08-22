//-----------------------------------------------------------------------------
// File: configurableelementeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#include "configurableelementeditor.h"

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsFilter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> configurableElementFormatter, QAbstractItemModel* completionModel,
    QWidget *parent):
QGroupBox(tr("Configurable element values"), parent),
view_(this),
delegate_(nullptr),
filterSelection_(new QCheckBox(tr("Show immediate values"), this))
{
	// set options for the view
	view_.setSortingEnabled(true);
    view_.setSelectionBehavior(QAbstractItemView::SelectItems);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);

    delegate_ = new ConfigurableElementDelegate(completionModel, parameterFinder, configurableElementFormatter, this);
    view_.setItemDelegate(delegate_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);
    topLayout->addWidget(filterSelection_);

    connect(&view_, SIGNAL(removeItem(QModelIndex const&)),
        delegate_, SLOT(onCreateRemoveElementCommands(QModelIndex const&)), Qt::UniqueConnection);

    connect(delegate_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createElementChangeCommand(
        QString const&, QString const&, QModelIndex const&, QAbstractItemModel*)),
        delegate_, SLOT(createElementChangeCommand(
        QString const&, QString const&, QModelIndex const&, QAbstractItemModel*)), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: configurableelementeditor::setModel()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setModel(QAbstractItemModel* newModel, QSortFilterProxyModel* newFilter)
{
    newFilter->setSourceModel(newModel);
    newFilter->setSortCaseSensitivity(Qt::CaseInsensitive);
    view_.setModel(newFilter);
}

//-----------------------------------------------------------------------------
// Function: configurableelementeditor::hideUnnecessaryColumns()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::hideUnnecessaryColumns()
{
    view_.setAlternatingRowColors(false);
    view_.setColumnHidden(ConfigurableElementsColumns::CHOICE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_LEFT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_RIGHT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::TYPE, true);
}
