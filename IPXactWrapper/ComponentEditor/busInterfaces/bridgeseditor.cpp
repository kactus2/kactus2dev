/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgeseditor.cpp
 *		Project: Kactus 2
 */

#include "bridgeseditor.h"
#include "bridgesdelegate.h"

#include <QVBoxLayout>

BridgesEditor::BridgesEditor(QSharedPointer<SlaveInterface> slave, 
							 QSharedPointer<Component> component,
							 QWidget *parent):
QGroupBox(tr("Bridges"), parent),
component_(component),
view_(this),
proxy_(this),
model_(slave, this) {

	view_.setItemDelegate(new BridgesDelegate(component_, this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	// set default width for master column
	view_.setColumnWidth(BridgesDelegate::MASTER_COLUMN, 200);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

BridgesEditor::~BridgesEditor() {
}

bool BridgesEditor::isValid() const {
	return model_.isValid();
}

void BridgesEditor::refresh( QSharedPointer<SlaveInterface> slave ) {
	model_.refresh(slave);
}
