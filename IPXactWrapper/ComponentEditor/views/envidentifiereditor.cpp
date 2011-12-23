/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiereditor.cpp
 */

#include "envidentifiereditor.h"

#include "envidentifiersdelegate.h"

#include <QHeaderView>
#include <QStringList>
#include <QIcon>
#include <QVBoxLayout>

EnvIdentifierEditor::EnvIdentifierEditor(View* view, 
										 QWidget *parent, 
										 const QString title): 
QGroupBox(title, parent),
view_(this), model_(view, this), 
proxy_(this) {

	// set view to be sortable
	view_.setSortingEnabled(true);

	view_.setItemDelegate(new EnvIdentifiersDelegate(this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setProperty("mandatoryField", true);

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	restoreChanges();

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

EnvIdentifierEditor::~EnvIdentifierEditor() {
}

void EnvIdentifierEditor::restoreChanges() {
	model_.restore();
}

void EnvIdentifierEditor::applyChanges() {
	model_.apply();
}

bool EnvIdentifierEditor::isValid() const {
	return model_.isValid();
}
