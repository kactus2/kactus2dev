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

#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <designEditors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsFilter.h>

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
delegate_(0),
filterSelection_(new QCheckBox(tr("Show immediate values"), this))
{
    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(completionModel);

	// set options for the view
	view_.setSortingEnabled(true);
    view_.setSelectionBehavior(QAbstractItemView::SelectItems);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);

    delegate_ = new ConfigurableElementDelegate(parameterCompleter, parameterFinder, configurableElementFormatter, this);
    view_.setItemDelegate(delegate_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);
    topLayout->addWidget(filterSelection_);

    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        delegate_, SLOT(onCreateRemoveElementCommand(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeAllSubItems(QModelIndex const&)),
        delegate_, SLOT(onCreateMultipleElementRemoveCommands(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::~ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::~ConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: configurableelementeditor::expandView()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::expandView()
{
    view_.expandAll();
}

//-----------------------------------------------------------------------------
// Function: configurableelementeditor::setFirstParentColumnsSpannable()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setFirstParentColumnsSpannable()
{
    int rowCount = view_.model()->rowCount();
    for (int i = 0; i < rowCount; ++i)
    {
        view_.setFirstColumnSpanned(i, QModelIndex(), true);
    }
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
// Function: configurableelementeditor::getDelegate()
//-----------------------------------------------------------------------------
ConfigurableElementDelegate* ConfigurableElementEditor::getDelegate() const
{
    return delegate_;
}

//-----------------------------------------------------------------------------
// Function: configurableelementeditor::getFilterSelectionBox()
//-----------------------------------------------------------------------------
QCheckBox* ConfigurableElementEditor::getFilterSelectionBox() const
{
    return filterSelection_;
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
