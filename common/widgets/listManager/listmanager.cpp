/* 
 *
 *  Created on: 28.1.2011
 *      Author: Antti Kamppi
 * 		filename: listmanager.cpp
 */

#include "listmanager.h"

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QModelIndex>

#include <QDebug>

ListManager::ListManager(const QString title, QWidget *parent): 
QGroupBox(title, parent), 
model_(NULL), 
view_(NULL) {
}

ListManager::~ListManager() {

}

const QStringList& ListManager::items() const {
	Q_ASSERT(model_);
	if (model_) {
		return model_->items();
	}
	return QStringList();
}

void ListManager::setItems( const QStringList& items ) {
	Q_ASSERT(model_);
	if (model_) {
		model_->setItems(items);
	}
}

int ListManager::size() const {
	Q_ASSERT(model_);
	if (model_) {
		return model_->rowCount(QModelIndex());
	}
	return 0;
}

void ListManager::initialize( const QStringList& items /*= QStringList()*/ ) {

	// remove the previous model and view if there are one
	if (model_) {
		delete model_;
		model_ = 0;
	}
	if (view_) {
		delete view_;
		view_ = 0;
	}

	// create new model and view
	model_ = new ListManagerModel(this, items);

	view_ = new EditableListView(this);

	// the signals from the view
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	// the signals from the model
	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QLayout* topLayout = layout();

	if (!topLayout) {
		// the top layout of the widget
		topLayout = new QHBoxLayout(this);
	}

	// add the view on the left side
	topLayout->addWidget(view_);

	// connect the model to the view
	view_->setModel(model_);
    view_->setItemDelegate(new LineEditDelegate(this));
}
