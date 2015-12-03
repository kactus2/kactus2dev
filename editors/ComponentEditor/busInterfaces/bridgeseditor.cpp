/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgeseditor.cpp
 *		Project: Kactus 2
 */

#include "bridgeseditor.h"
#include "bridgesdelegate.h"
#include "BridgeColumns.h"

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BridgesEditor::BridgesEditor()
//-----------------------------------------------------------------------------
BridgesEditor::BridgesEditor(QSharedPointer<SlaveInterface> slave, QSharedPointer<Component> component,
    QWidget* parent):
QGroupBox(tr("Transparent bridges"), parent),
    view_(this),
    proxy_(this),
    model_(slave, this)
{
    view_.setItemDelegate(new BridgesDelegate(component, this));

    // items can not be dragged
    view_.setItemsDraggable(false);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	view_.setColumnWidth(BridgeColumns::MASTER_COLUMN, 200);    

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BridgesEditor::~BridgesEditor()
//-----------------------------------------------------------------------------
BridgesEditor::~BridgesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: BridgesEditor::isValid()
//-----------------------------------------------------------------------------
bool BridgesEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: BridgesEditor::refresh()
//-----------------------------------------------------------------------------
void BridgesEditor::refresh(QSharedPointer<SlaveInterface> slave)
{
	model_.refresh(slave);
}
