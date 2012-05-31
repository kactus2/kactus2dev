/* 
 *  	Created on: 30.5.2012
 *      Author: Antti Kamppi
 * 		filename: listeditor.cpp
 *		Project: Kactus 2
 */

#include "listeditor.h"

#include <QHBoxLayout>

ListEditor::ListEditor(QWidget *parent):
QWidget(parent),
model_(this),
view_(this) {

	// the signals from the view
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		&model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	// the signals from the model
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addWidget(&view_);

	// connect the model to the view
	view_.setModel(&model_);
}

ListEditor::~ListEditor() {
}

const QStringList& ListEditor::items() const {
	return model_.items();
}

void ListEditor::setItems( const QStringList& items ) {
	model_.setItems(items);
}

int ListEditor::size() const {
	return model_.rowCount();
}

void ListEditor::setItemDelegate( QStyledItemDelegate* delegate ) {
	view_.setItemDelegate(delegate);
}
