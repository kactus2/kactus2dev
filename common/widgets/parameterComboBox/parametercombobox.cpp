/* 
 *	Created on: 1.3.2013
 *	Author:		Antti Kamppi
 * 	File name:	parametercombobox.cpp
 * 	Project:	Kactus 2
*/

#include "parametercombobox.h"

ParameterComboBox::ParameterComboBox( QSharedPointer<Component> component,
	QWidget *parent,
	bool editable /*= true*/ ):
QComboBox(parent),
component_(component) {

	Q_ASSERT(component_);

	setEditable(editable);

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}

ParameterComboBox::~ParameterComboBox() {
}

void ParameterComboBox::refresh() {
	
	// the items to be contained in the combo box
	QStringList items = component_->getModelParameterNames();
	items.append(component_->getParameterNames());

	// the items are added without emitting signals
	disconnect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)));

	// remove previous items
	clear();

	// add the parameter/model parameter names
	addItems(items);

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}

void ParameterComboBox::onEditChanged( const QString& text ) {
	emit textChanged(text);
}

void ParameterComboBox::setRemapText( const QString& text ) {

	// the text is set without emitting signals
	disconnect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)));

	setCurrentText(text);

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}
