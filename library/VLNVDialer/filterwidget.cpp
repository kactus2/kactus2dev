//-----------------------------------------------------------------------------
// File: filterwidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the widgets to set the filter options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "filterwidget.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FilterWidget::FilterWidget()
//-----------------------------------------------------------------------------
FilterWidget::FilterWidget(QWidget *parent):
QWidget(parent),
firmness_(this),
implementation_(this),
type_(this),
hierarchy_(this),
validity_(this)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&type_);
	layout->addWidget(&implementation_);
	layout->addWidget(&hierarchy_);
	layout->addWidget(&firmness_);
    layout->addWidget(&validity_);
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
    connect(&validity_, SIGNAL(optionsChanged(const Utils::ValidityOptions&)),
               this, SIGNAL(validityChanged(const Utils::ValidityOptions&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FilterWidget::~FilterWidget()
//-----------------------------------------------------------------------------
FilterWidget::~FilterWidget()
{
}

//-----------------------------------------------------------------------------
// Function: FilterWidget::setFilters()
//-----------------------------------------------------------------------------
void FilterWidget::setFilters(Utils::FilterOptions options)
{
    blockSignals(true);
    type_.setTypes(options.type);  
    implementation_.setImplementation(options.implementation);  
    hierarchy_.setHierarchy(options.hierarchy);  
    firmness_.setFirmness(options.firmness);
    validity_.setValidity(options.validity);
    blockSignals(false);

    emit optionsChanged(options);
}

//-----------------------------------------------------------------------------
// Function: FilterWidget::getFilters()
//-----------------------------------------------------------------------------
Utils::FilterOptions FilterWidget::getFilters() const
{
    Utils::FilterOptions options;
    options.type = type_.getTypes();
    options.implementation = implementation_.getImplementation();
    options.hierarchy = hierarchy_.getHierarchy();
    options.firmness = firmness_.getFirmness();
    options.validity = validity_.getValidity();
    return options;
}

//-----------------------------------------------------------------------------
// Function: FilterWidget::selectAll()
//-----------------------------------------------------------------------------
void FilterWidget::selectAll(bool select)
{
    type_.selectAll(select);
    implementation_.selectAll(select);
    hierarchy_.selectAll(select);
    firmness_.selectAll(select);
    validity_.selectAll(select);
}
