/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.cpp
 *		Project: Kactus 2
 */

#include "filesetseditor.h"
#include "filesetsdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>

FileSetsEditor::FileSetsEditor( QSharedPointer<Component> component):
ItemEditor(component),
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

	view_.setModel(&proxy_);
	view_.setItemDelegate(new FileSetsDelegate(this));
	view_.setColumnWidth(0, FileSetsEditor::NAME_COLUMN_WIDTH);
	view_.setColumnWidth(1, FileSetsEditor::DESC_COLUMN_WIDTH);

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

}

void FileSetsEditor::refresh() {
	view_.update();
}
