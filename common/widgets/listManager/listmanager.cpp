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
QGroupBox(title, parent), model_(this, items), view_(this) {
	
	view_.setMovement(QListView::Free);

	// create the layout for the right side buttons
	QVBoxLayout* buttonLayout = new QVBoxLayout();

	// create the buttons
	QPushButton* addButton = new QPushButton(tr("Add new"), this);
	buttonLayout->addWidget(addButton);
	connect(addButton, SIGNAL(clicked(bool)),
		this, SLOT(onAdd()), Qt::UniqueConnection);

	QPushButton* editButton = new QPushButton(tr("Edit"), this);
	buttonLayout->addWidget(editButton);
	connect(editButton, SIGNAL(clicked(bool)),
		this, SLOT(onEdit()), Qt::UniqueConnection);

	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	buttonLayout->addWidget(removeButton);
	connect(removeButton, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);

	QPushButton* moveUpButton = new QPushButton(tr("Move up"), this);
	buttonLayout->addWidget(moveUpButton);
	connect(moveUpButton, SIGNAL(clicked(bool)),
		this, SLOT(onMoveUp()), Qt::UniqueConnection);

	QPushButton* moveDownButton = new QPushButton(tr("Move Down"), this);
	buttonLayout->addWidget(moveDownButton);
	connect(moveDownButton, SIGNAL(clicked(bool)),
		this, SLOT(onMoveDown()), Qt::UniqueConnection);

	buttonLayout->addStretch();

	// the top layout of the widget
	QHBoxLayout* topLayout = new QHBoxLayout(this);

	// add the view on the left side
	topLayout->addWidget(&view_);

	// and all the buttons on the right side
	topLayout->addLayout(buttonLayout);

	// connect the model to the view
	view_.setModel(&model_);

}

ListManager::~ListManager() {

}

void ListManager::onAdd() {
	QString item = QInputDialog::getText(this, tr("Add a new item to list"), 
		tr("Input the text for the item to add"));

	// if user wrote an empty string or if clicked cancel
	if (item.isEmpty()) {
		return;
	}

	model_.appendItem(item);

	// inform that content of the widget has changed
	emit contentChanged();
	return;
}

void ListManager::onEdit() {
	
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldText = model_.data(index, Qt::DisplayRole).toString();

	// ask the user to set the new text
	QString newText = QInputDialog::getText(this, tr("Edit an item in the list"),
		tr("Input the new value for the item"), QLineEdit::Normal, 
		oldText);

	// if empty string was given or cancel was pressed
	if (newText.isEmpty()) {
		return;
	}

	// if user changed the text
	else if (oldText != newText) {
		model_.replace(index, newText);

		// inform that content of the widget has changed
		emit contentChanged();
	}
}

void ListManager::onRemove() {
	QModelIndex index = view_.currentIndex();
	model_.remove(index);

	// inform that content of the widget has changed
	emit contentChanged();
}

void ListManager::onMoveUp() {
	QModelIndex index = view_.currentIndex();
	model_.moveUp(index);

	// inform that content of the widget has changed
	emit contentChanged();
}

void ListManager::onMoveDown() {
	QModelIndex index = view_.currentIndex();
	model_.moveDown(index);

	// inform that content of the widget has changed
	emit contentChanged();
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
