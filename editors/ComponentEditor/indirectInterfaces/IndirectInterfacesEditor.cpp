//-----------------------------------------------------------------------------
// File: IndirectInterfacesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Editor for component Indirect Interfaces.
//-----------------------------------------------------------------------------

#include "IndirectInterfacesEditor.h"

#include "IndirectInterfacesDelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesEditor::IndirectInterfacesEditor()
//-----------------------------------------------------------------------------
IndirectInterfacesEditor::IndirectInterfacesEditor(LibraryInterface* handler,
    QSharedPointer<Component> component,
    //QSharedPointer<IndirectInterfaceValidator> validator,
    QSharedPointer<ParameterFinder> parameterFinder,
    QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component, parameterFinder, this)
{
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Indirect interfaces summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/indirectIfListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

    view_.setItemDelegate(new IndirectInterfacesDelegate(component, this));    
    
	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(interfaceAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(interfaceRemoved(int)),	this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesEditor::~IndirectInterfacesEditor()
//-----------------------------------------------------------------------------
IndirectInterfacesEditor::~IndirectInterfacesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesEditor::refresh()
//-----------------------------------------------------------------------------
void IndirectInterfacesEditor::refresh()
{
	view_.setModel(&model_);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesEditor::showEvent()
//-----------------------------------------------------------------------------
void IndirectInterfacesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/indirectInterfaces.html");
}
