/* 
 *  	Created on: 8.8.2011
 *      Author: Antti Kamppi
 * 		filename: objectremovedialog.cpp
 *		Project: Kactus 2
 */

#include "objectremovedialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QHeaderView>

#include <QDebug>

ObjectRemoveDialog::ObjectRemoveDialog(QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
okButton_(tr("OK"), this),
cancelButton_(tr("Cancel"), this),
selectionBox_(tr("Select/deselect all"), this),
view_(this),
filter_(this),
model_(this) {

	setWindowTitle(tr("Select items to be removed"));

	filter_.setSourceModel(&model_);
	view_.setModel(&filter_);

	selectionBox_.setChecked(true);

	// set view to be sortable
	view_.setSortingEnabled(true);
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setAlternatingRowColors(true);
	view_.verticalHeader()->hide();
	//view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	view_.setWordWrap(true);
	view_.horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	view_.setColumnWidth(0, 500);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&selectionBox_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();
	buttonLayout->addWidget(&okButton_, 0, Qt::AlignRight);
	buttonLayout->addWidget(&cancelButton_, 0, Qt::AlignRight);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_, 1);
	layout->addLayout(buttonLayout, 0);

	connect(&cancelButton_, SIGNAL(clicked(bool)),
		this, SLOT(reject()), Qt::UniqueConnection);
	connect(&okButton_, SIGNAL(clicked(bool)),
		this, SLOT(accept()), Qt::UniqueConnection);
	connect(&selectionBox_, SIGNAL(stateChanged(int)),
		&model_, SLOT(onSelectionChange(int)), Qt::UniqueConnection);

}

ObjectRemoveDialog::~ObjectRemoveDialog() {
}

void ObjectRemoveDialog::addItem( const ObjectRemoveModel::Item& item ) {
	model_.addItem(item);
}

QSize ObjectRemoveDialog::sizeHint() const {
	return QSize(700, 700);
}

QList<ObjectRemoveModel::Item> ObjectRemoveDialog::getItemsToRemove() const {
	return model_.getItemsToRemove();
}

void ObjectRemoveDialog::createItem( const VLNV& vlnv, bool locked ) {
	model_.createItem(vlnv, locked);
}

void ObjectRemoveDialog::createItem( const QString& filePath ) {
	model_.createItem(filePath);
}
