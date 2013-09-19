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
	layout->addWidget(&type_);
	layout->addWidget(&implementation_);
	layout->addWidget(&hierarchy_);
	layout->addWidget(&firmness_);
	layout->addStretch();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

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

//-----------------------------------------------------------------------------
// Function: FilterWidget::setFilters()
//-----------------------------------------------------------------------------
void FilterWidget::setFilters(FilterOptions options)
{
    type_.setTypes(options.type);  
    implementation_.setImplementation(options.implementation);  
    hierarchy_.setHierarchy(options.hierarchy);  
    firmness_.setFirmness(options.firmness);    
}

//-----------------------------------------------------------------------------
// Function: FilterWidget::getFilters()
//-----------------------------------------------------------------------------
FilterWidget::FilterOptions FilterWidget::getFilters() const
{
    FilterOptions options;
    options.type = type_.getTypes();
    options.implementation = implementation_.getImplementation();
    options.hierarchy = hierarchy_.getHierarchy();
    options.firmness = firmness_.getFirmness();
    return options;
}

