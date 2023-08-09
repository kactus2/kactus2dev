//-----------------------------------------------------------------------------
// File: Modeseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to add/remove/edit the mode-elements of a component.
//-----------------------------------------------------------------------------

#include "Modeseditor.h"
#include "ModeColumns.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: ModesEditor::ModesEditor()
//-----------------------------------------------------------------------------
ModesEditor::ModesEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
    ExpressionSet expressions,
	QWidget* parent) :
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component, expressions, this)
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Modes summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = handler->getDirectoryPath(component->getVlnv());
	QString defPath = QString("%1/ModesListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);
	view_.setItemsDraggable(false);
	view_.setSortingEnabled(true);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(ModeAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(ModeRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ModesEditor::refresh()
//-----------------------------------------------------------------------------
void ModesEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ModesEditor::showEvent()
//-----------------------------------------------------------------------------
void ModesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	
    emit helpUrlRequested("componenteditor/modes2022.html");
}
