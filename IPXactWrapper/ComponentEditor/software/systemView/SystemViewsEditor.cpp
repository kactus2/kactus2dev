//-----------------------------------------------------------------------------
// File: SystemViewsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The editor to add/remove/edit the system views of a component.
//-----------------------------------------------------------------------------

#include "SystemViewsEditor.h"
#include "SystemViewsDelegate.h"
#include <LibraryManager/libraryinterface.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>

SystemViewsEditor::SystemViewsEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler, 
							 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("System views"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/sysViewListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new SystemViewsDelegate(this));

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

SystemViewsEditor::~SystemViewsEditor() {
}

bool SystemViewsEditor::isValid() const {
	return model_.isValid();
}

void SystemViewsEditor::refresh() {
	view_.update();
}

void SystemViewsEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/systemviews.html");
}
