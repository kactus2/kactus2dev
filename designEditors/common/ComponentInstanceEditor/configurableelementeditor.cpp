/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.cpp
 *		Project: Kactus 2
 */

#include "configurableelementeditor.h"

#include "configurableelementdelegate.h"
#include <IPXACTmodels/component.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSharedPointer>
#include <QAbstractItemDelegate>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(QWidget *parent,
													 const QString& title):
QGroupBox(title, parent),
component_(0),
view_(this),
filter_(this),
model_(this)
{
	filter_.setSourceModel(&model_);
	view_.setModel(&filter_);

	// set options for the view
	
	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	// the delegate for editing
	view_.setItemDelegate(new ConfigurableElementDelegate(QSharedPointer<Component>(), this));

    view_.setAlternatingRowColors(false);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::~ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::~ConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setComponent( ComponentItem* component ) 
{
	// set the component for the item delegate.
	QAbstractItemDelegate* absDelegate = view_.itemDelegate();
	ConfigurableElementDelegate* delegate = qobject_cast<ConfigurableElementDelegate*>(absDelegate);
	Q_ASSERT(delegate);
	delegate->setComponent(component->componentModel());

	component_ = component;
	model_.setComponent(component);

    model_.setExpressionParser(
        QSharedPointer <IPXactSystemVerilogParser> (new IPXactSystemVerilogParser(component->componentModel())));
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::clear() 
{
	component_ = 0;
	model_.clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::onRemoveClick()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::onRemoveClick() 
{
	QModelIndex index = view_.currentIndex();
	QModelIndex sourceIndex = filter_.mapToSource(index);
	if (sourceIndex.isValid())
		model_.onRemove(sourceIndex);
}
