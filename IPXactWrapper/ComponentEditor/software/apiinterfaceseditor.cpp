/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: apiinterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include "apiinterfaceseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include "apiinterfacesdelegate.h"

ApiInterfacesEditor::ApiInterfacesEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler, 
										 QWidget* parent /*= 0*/):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("API interfaces"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ApiInterfacesDelegate(this));
	view_.setColumnWidth(ApiInterfacesDelegate::NAME_COLUMN, ApiInterfacesEditor::NAME_COLUMN_WIDTH);
	view_.setColumnWidth(ApiInterfacesDelegate::API_DEF_COLUMN, ApiInterfacesEditor::API_DEF_COLUMN_WIDTH);
	view_.setColumnWidth(ApiInterfacesDelegate::DEPENDENCY_COLUMN, ApiInterfacesEditor::DEPENDENCY_COLUMN_WIDTH);
	view_.setColumnWidth(ApiInterfacesDelegate::DISPLAY_NAME_COLUMN, ApiInterfacesEditor::DISPLAY_NAME_COLUMN_WIDTH);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(apiAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(apiRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

ApiInterfacesEditor::~ApiInterfacesEditor() {
}

bool ApiInterfacesEditor::isValid() const {
	return model_.isValid();
}

void ApiInterfacesEditor::refresh() {
	view_.update();
}
