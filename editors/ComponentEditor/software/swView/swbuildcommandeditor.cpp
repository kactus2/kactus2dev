//-----------------------------------------------------------------------------
// File: swbuildcommandeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// Editor to set the SW build commands for SW view of a component.
//-----------------------------------------------------------------------------

#include "swbuildcommandeditor.h"

#include "swbuildcommandmodel.h"
#include "swbuilddelegate.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/fileBuilders/FileBuilderColumns.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SWBuildCommandEditor::SWBuildCommandEditor()
//-----------------------------------------------------------------------------
SWBuildCommandEditor::SWBuildCommandEditor(QSharedPointer<Component> component,
	QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands,
	QWidget *parent):
QGroupBox(tr("SW build commands"), parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new SWBuildCommandModel(component, swBuildCommands, this)),
component_(component)
{
	view_->setSortingEnabled(true);
	view_->setItemsDraggable(false);

	proxy_->setSourceModel(model_);

	view_->setItemDelegate(new SWBuildDelegate(this));
	view_->setModel(proxy_);

	// sort the view
	view_->sortByColumn(0, Qt::AscendingOrder);

	// Software components do not contain the command
	view_->setColumnHidden(FileBuilderColumns::COMMAND_COLUMN, 
        component->getImplementation() == KactusAttribute::SW);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandEditor::~SWBuildCommandEditor()
//-----------------------------------------------------------------------------
SWBuildCommandEditor::~SWBuildCommandEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandEditor::isValid()
//-----------------------------------------------------------------------------
bool SWBuildCommandEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandEditor::refresh()
//-----------------------------------------------------------------------------
void SWBuildCommandEditor::refresh()
{
	view_->setColumnHidden(FileBuilderColumns::COMMAND_COLUMN, 
        component_->getImplementation() == KactusAttribute::SW);
	view_->update();
}
