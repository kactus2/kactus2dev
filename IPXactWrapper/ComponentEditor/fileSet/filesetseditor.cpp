/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.cpp
 *		Project: Kactus 2
 */

#include "filesetseditor.h"
#include "filesetsdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

FileSetsEditor::FileSetsEditor( QSharedPointer<Component> component,
	LibraryInterface* handler):
ItemEditor(component, handler),
view_(this),
model_(component, this),
proxy_(this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("File sets summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);

	view_.setAllowImportExport(true);
	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/fileSetList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);

	view_.setModel(&proxy_);
	view_.setItemDelegate(new FileSetsDelegate(this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileSetAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileSetRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

FileSetsEditor::~FileSetsEditor() {

}

bool FileSetsEditor::isValid() const {
	return model_.isValid();
}

void FileSetsEditor::makeChanges() {
	// TODO remove this in final
}

void FileSetsEditor::refresh() {
	view_.update();
}

void FileSetsEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/filesets.html");
}
