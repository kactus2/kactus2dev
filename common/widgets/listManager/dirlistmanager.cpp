/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanager.cpp
 */

#include "dirlistmanager.h"
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include "dirlistmanagermodel.h"

#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>

DirListManager::DirListManager( const QString title, 
							   LibraryInterface* handler,
							   QSharedPointer<Component> component,
							   QWidget *parent /*= 0*/):
ListManager(title, parent), 
handler_(handler), 
component_(component) {
}

DirListManager::~DirListManager() {
}

void DirListManager::initialize( const QStringList& items /*= QStringList()*/ ) {
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
	model_ = new DirListManagerModel(handler_, component_, this, items);

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

