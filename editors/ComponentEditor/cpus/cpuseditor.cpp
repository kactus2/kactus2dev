//-----------------------------------------------------------------------------
// File: cpuseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// Editor to add/remove/edit the cpu-elements of a component.
//-----------------------------------------------------------------------------

#include "cpuseditor.h"
#include "cpusdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: CpusEditor::CpusEditor()
//-----------------------------------------------------------------------------
CpusEditor::CpusEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
     QSharedPointer<CPUValidator> validator, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component, validator, this)
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("CPUs"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = handler->getDirectoryPath(component->getVlnv());
	QString defPath = QString("%1/cpusListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);
	view_.setItemsDraggable(false);
	view_.setItemDelegate(new CpusDelegate(component, this));	

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CpusEditor::~CpusEditor()
//-----------------------------------------------------------------------------
CpusEditor::~CpusEditor()
{
}

//-----------------------------------------------------------------------------
// Function: CpusEditor::refresh()
//-----------------------------------------------------------------------------
void CpusEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: CpusEditor::showEvent()
//-----------------------------------------------------------------------------
void CpusEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/cpus.html");
}
