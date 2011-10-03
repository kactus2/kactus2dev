/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.cpp
 *		Project: Kactus 2
 */

#include "configurableelementeditor.h"

#include "configurableelementdelegate.h"
#include <models/component.h>
#include <designwidget/diagramcomponent.h>

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSharedPointer>
#include <QAbstractItemDelegate>

ConfigurableElementEditor::ConfigurableElementEditor(QWidget *parent,
													 const QString& title):
QGroupBox(title, parent),
component_(0),
view_(this),
filter_(this),
model_(this),
addButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeButton_(QIcon(":/icons/graphics/remove.png"), QString(), this) {

	filter_.setSourceModel(&model_);
	view_.setModel(&filter_);

	// set options for the view
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setResizeMode(QHeaderView::Interactive);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setSelectionBehavior(QAbstractItemView::SelectItems);
	view_.setEditTriggers(QAbstractItemView::DoubleClicked | 
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	view_.setWordWrap(true);
	view_.setSortingEnabled(false);
	view_.verticalHeader()->hide();
	view_.setItemDelegate(new ConfigurableElementDelegate(QSharedPointer<Component>(), this));
	view_.setSortingEnabled(true);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(&addButton_);
	buttonLayout->addWidget(&removeButton_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);
	topLayout->addLayout(buttonLayout);

	connect(&addButton_, SIGNAL(clicked(bool)),
		&model_, SLOT(onAdd()), Qt::UniqueConnection);
	connect(&removeButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemoveClick()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

ConfigurableElementEditor::~ConfigurableElementEditor() {
}

void ConfigurableElementEditor::setComponent( DiagramComponent* component ) {

	// set the component for the item delegate.
	QAbstractItemDelegate* absDelegate = view_.itemDelegate();
	ConfigurableElementDelegate* delegate = qobject_cast<ConfigurableElementDelegate*>(absDelegate);
	Q_ASSERT(delegate);
	delegate->setComponent(component->componentModel());

	component_ = component;
	model_.setComponent(component);
}

void ConfigurableElementEditor::clear() {

	component_ = 0;
	model_.clear();
}

void ConfigurableElementEditor::onRemoveClick() {
	QModelIndex index = view_.currentIndex();
	QModelIndex sourceIndex = filter_.mapToSource(index);
	if (sourceIndex.isValid())
		model_.onRemove(sourceIndex);
}
