/* 
 *
 *  Created on: 25.5.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyrefwidget.cpp
 */

#include "hierarchyrefwidget.h"

#include <models/view.h>
#include <models/component.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QFormLayout>

HierarchyRefWidget::HierarchyRefWidget(View* view, 
									   QSharedPointer<Component> component,
										LibraryInterface* libHandler,
									   QWidget *parent): 
QWidget(parent),
view_(view),
component_(component),
hierarchyRef_(VLNV::DESIGNCONFIGURATION, libHandler, this, this),
topLevelRef_(this) {

	hierarchyRef_.setTitle(tr("Hierarchy reference"));
	hierarchyRef_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	QFormLayout* extensionLayout = new QFormLayout();
	extensionLayout->addRow(tr("VendorExtension: Reference to a top-level "
		"implementation view"), &topLevelRef_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&hierarchyRef_);
	layout->addLayout(extensionLayout);
	layout->addStretch();

	connect(&hierarchyRef_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&topLevelRef_, SIGNAL(currentIndexChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

HierarchyRefWidget::~HierarchyRefWidget() {
}

bool HierarchyRefWidget::isValid() const {
	return hierarchyRef_.isValid();
}

void HierarchyRefWidget::restoreChanges() {
	refresh();

	// ask the vlnv from the model
	hierarchyRef_.setVLNV(view_->getHierarchyRef());

	// find the text in the combo box and select it
	QString topRef = view_->getTopLevelView();
	int index = topLevelRef_.findText(topRef);
	topLevelRef_.setCurrentIndex(index);
}

void HierarchyRefWidget::applyChanges() {

	view_->setHierarchyRef(hierarchyRef_.getVLNV());
	view_->setTopLevelView(topLevelRef_.currentText());
}

void HierarchyRefWidget::refresh() {
	
	// remove the previous values from the combo box
	topLevelRef_.clear();

	// ask the available views from the component
	QStringList list;
	// if user does not want to reference any view
	list.append(QString(""));

	list.append(component_->getViewNames());

	topLevelRef_.addItems(list);

	// find the text in the combo box and select it
	QString topRef = view_->getTopLevelView();
	int index = topLevelRef_.findText(topRef);
	topLevelRef_.setCurrentIndex(index);
}
