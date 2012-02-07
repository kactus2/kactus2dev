/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditor.cpp
 */

#include "filesetrefeditor.h"

#include "filesetrefeditordelegate.h"
#include "filesetrefmodel.h"

#include <models/component.h>
#include <common/dialogs/comboSelector/comboselector.h>

#include <QHBoxLayout>
#include <QLayout>

FileSetRefEditor::FileSetRefEditor(QSharedPointer<Component> component,
								   const QString title /*= tr("List")*/, 
								   QWidget *parent /*= 0*/):
ListManager(title, parent),
component_(component) {

	Q_ASSERT(component_);
}

FileSetRefEditor::~FileSetRefEditor() {
}

void FileSetRefEditor::initialize( const QStringList& items /*= QStringList()*/ ) {
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
	model_ = new FileSetRefModel(this, component_, items);

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

	view_->setItemDelegate(new FileSetRefEditorDelegate(this, component_));
}

