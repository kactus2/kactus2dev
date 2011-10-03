/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: filterwidget.cpp
 *		Project: Kactus 2
 */

#include "filterwidget.h"

#include <QVBoxLayout>

FilterWidget::FilterWidget(QWidget *parent):
QWidget(parent),
firmness_(this),
implementation_(this),
type_(this),
hierarchy_(this) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&firmness_);
	layout->addWidget(&implementation_);
	layout->addWidget(&type_);
	layout->addWidget(&hierarchy_);
	layout->addStretch();
	layout->setSpacing(0);

	connect(&firmness_, SIGNAL(optionsChanged(const Utils::FirmnessOptions&)),
		this, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
	connect(&implementation_, SIGNAL(optionsChanged(const Utils::ImplementationOptions&)),
		this, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
	connect(&type_, SIGNAL(optionsChanged(const Utils::TypeOptions&)),
		this, SIGNAL(typeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
	connect(&hierarchy_, SIGNAL(optionsChanged(const Utils::HierarchyOptions&)),
		this, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
}

FilterWidget::~FilterWidget() {
}
