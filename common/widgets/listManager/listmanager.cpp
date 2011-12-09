/* 
 *
 *  Created on: 28.1.2011
 *      Author: Antti Kamppi
 * 		filename: listmanager.cpp
 */

#include "listmanager.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QModelIndex>

#include <QDebug>

ListManager::ListManager(const QString title, QWidget *parent,
						 const QStringList& items): 
QGroupBox(title, parent), 
model_(this, items), 
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

	// the top layout of the widget
	QHBoxLayout* topLayout = new QHBoxLayout(this);

	// add the view on the left side
	topLayout->addWidget(&view_);

	// connect the model to the view
	view_.setModel(&model_);

}

ListManager::~ListManager() {

}

const QStringList& ListManager::items() const {
	return model_.items();
}

void ListManager::setItems( const QStringList& items ) {
	model_.setItems(items);
}

int ListManager::size() const {
	return model_.rowCount(QModelIndex());
}
