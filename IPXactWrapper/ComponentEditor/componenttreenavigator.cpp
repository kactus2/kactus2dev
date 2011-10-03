/* 
 *
 *  Created on: 1.2.2011
 *      Author: Antti Kamppi
 * 		filename: componenttreenavigator.cpp
 */

#include "componenttreenavigator.h"
#include "componenttreeitem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QString>

#include <QDebug>

ComponentTreeNavigator::ComponentTreeNavigator(QSharedPointer<Component> component, 
											   LibraryInterface* handler,
											   QWidget *parent): 
QGroupBox(parent), 
view_(this), 
model_(component, handler, this), 
addButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeButton_(QIcon(":/icons/graphics/remove.png"), QString(), this) {

	// connect the model to the view
	view_.setModel(&model_);
	view_.expandToDepth(1);

	// the layout to manage the two buttons
	QVBoxLayout* buttonLayout = new QVBoxLayout();

	// connect the add button
	connect(&addButton_, SIGNAL(clicked(bool)),
		this, SLOT(onAdd()), Qt::UniqueConnection);
	buttonLayout->addWidget(&addButton_);

	// connect the remove button
	connect(&removeButton_, SIGNAL(clicked()),
		this, SLOT(onRemove()), Qt::UniqueConnection);
	buttonLayout->addWidget(&removeButton_);
	buttonLayout->addStretch();

	// at start the buttons are disabled because nothing has been selected
	removeButton_.setDisabled(true);
	addButton_.setDisabled(true);

	// create the top layout that manages the view and the buttonLayout
	QHBoxLayout* topLayout = new QHBoxLayout(this);

	topLayout->addWidget(&view_);
	topLayout->addLayout(buttonLayout);

	// connect the signal from the view that informs when user selects one item
	// in the view
	connect(&view_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemSelected(const QModelIndex&)), Qt::UniqueConnection);
	
	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		&model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(itemMoved()),
		this, SIGNAL(itemMoved()), Qt::UniqueConnection);
}

ComponentTreeNavigator::~ComponentTreeNavigator() {

}

void ComponentTreeNavigator::onAdd() {
	
	// get currently selected index
	QModelIndex parentIndex = view_.currentIndex();
	if (!parentIndex.isValid()) {
		return;
	}

	// create the child item and get it's model index
	QModelIndex childIndex = model_.createChild(parentIndex);

	// if the child was not created an invalid model index is returned
	if (!childIndex.isValid()) {
		// return because nothing was created
		return;
	}

	// select the recently created index
	view_.setCurrentIndex(childIndex);
}

void ComponentTreeNavigator::onRemove() {

	// get currently selected index
	QModelIndex index = view_.currentIndex();
	if (!index.isValid()) {
		return;
	}
	
	// inform that all the editors for these items should be removed.
	emit itemRemoved(index);

	// tell model to remove the item.
	model_.removeItem(index);

	// deselect all items
	view_.setCurrentIndex(QModelIndex());
}

void ComponentTreeNavigator::updateItemName(const QString newName, const QModelIndex& index ) 
{
	model_.setItemName(newName, index);
}

QSize ComponentTreeNavigator::sizeHint() const {
	return QSize(300, 600);
}

void ComponentTreeNavigator::updateItem( const QModelIndex& index ) {
	model_.updateItem(index);
}

void ComponentTreeNavigator::onItemSelected( const QModelIndex& index ) {
	
	if (!index.isValid()) {
		return;
	}

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		index.internalPointer());

	// if editor is locked then don't enabled state of buttons
	if (!locked_) {
		// if the item can have children then enable the add button, otherwise 
		// disable it. If item can be removed then enable remove button.
		addButton_.setEnabled(item->canHaveChildren());
		removeButton_.setEnabled(item->canBeRemoved());
	}

	// pass the info on item selection forward
	emit itemSelected(index);
}

void ComponentTreeNavigator::clearSelection() {
	/*view_.clearSelection();
	view_.setCurrentIndex(QModelIndex());*/
}

void ComponentTreeNavigator::setLocked( bool locked ) {
	locked_ = locked;
	addButton_.setDisabled(locked);
	removeButton_.setDisabled(locked);
	view_.setLocked(locked);
}

void ComponentTreeNavigator::selectItem( const QModelIndex& index ) {
	disconnect(&view_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemSelected(const QModelIndex&)));
	//view_.clearSelection();
	view_.selectItem(index);
	connect(&view_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemSelected(const QModelIndex&)), Qt::UniqueConnection);
}
