//-----------------------------------------------------------------------------
// File: vlnvdialer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// VLNVDialer is used to set options that specify what objects to display in library.
//-----------------------------------------------------------------------------

#include "vlnvdialer.h"

#include <QLabel>
#include <QGridLayout>
#include <QIcon>
#include <QSettings>

#include <common/widgets/tagEditor/TagSelectorContainer.h>

//-----------------------------------------------------------------------------
// Function: VLNVDialer::VLNVDialer()
//-----------------------------------------------------------------------------
VLNVDialer::VLNVDialer(QWidget *parent):
    QWidget(parent),
    hideButton_(QIcon(":/icons/common/graphics/filter.png"), QString(), this),
    selectAllButton_(QIcon(":/icons/common/graphics/check-all.png"), QString(), this),
    filters_(this),
    dialer_(this),
    tagGroup_(tr("Tags"), this),
    tagFilter_(new TagSelectorContainer(this))
{
    hideButton_.setToolTip(tr("Show/hide Filters"));
    hideButton_.setFlat(true);

    selectAllButton_.setToolTip(tr("Select all"));
    selectAllButton_.setFlat(true);

    tagGroup_.setFlat(true);

	// check if the filters were visible or not previously
	QSettings settings;
	hideFilters_ = !settings.value("FilterWidget/Hidden", false).toBool();
	onHideShowClick();

    connect(&hideButton_, SIGNAL(clicked(bool)), this, SLOT(onHideShowClick()), Qt::UniqueConnection);
    connect(&selectAllButton_, SIGNAL(clicked(bool)), this, SLOT(onSelectAll()), Qt::UniqueConnection);

	connect(&dialer_, SIGNAL(vendorChanged(const QString&)),
		this, SIGNAL(vendorChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(libraryChanged(const QString&)),
		this, SIGNAL(libraryChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(versionChanged(const QString&)),
		this, SIGNAL(versionChanged(const QString&)), Qt::UniqueConnection);

	connect(&filters_, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
		this, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
		this, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(typeChanged(const Utils::TypeOptions&)),
		this, SIGNAL(typeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
		this, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
    connect(&filters_, SIGNAL(validityChanged(Utils::ValidityOptions const&)),
            this, SIGNAL(validityChanged(Utils::ValidityOptions const&)), Qt::UniqueConnection);
    connect(&filters_, SIGNAL(optionsChanged(Utils::FilterOptions const&)),
        this, SIGNAL(filtersChanged(Utils::FilterOptions const&)), Qt::UniqueConnection);
    connect(tagFilter_, SIGNAL(contentChanged()), this, SLOT(onHandleTagFilterChange()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::setRootItem()
//-----------------------------------------------------------------------------
void VLNVDialer::setRootItem(const LibraryItem* rootItem)
{
    dialer_.setRootItem(rootItem);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::loadFilterSettings()
//-----------------------------------------------------------------------------
void VLNVDialer::loadFilterSettings(QSettings& settings)
{
    Utils::FilterOptions filters;
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    filters.type.components_ = settings.value("ShowComponents", true).toBool();
    filters.type.buses_ = settings.value("ShowBuses", true).toBool();
    filters.type.catalogs_ = settings.value("ShowCatalogs", true).toBool();
    filters.type.apis_ = settings.value("ShowApis", true).toBool();
    filters.type.advanced_ = settings.value("ShowAdvanced", false).toBool();
    settings.endGroup();
    settings.beginGroup("Implementation");
    filters.implementation.hw_ = settings.value("ShowHW", true).toBool();
    filters.implementation.sw_ = settings.value("ShowSW", true).toBool();
    filters.implementation.system_ = settings.value("ShowSystem", true).toBool();
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    filters.hierarchy.flat_ = settings.value("ShowGlobal", true).toBool();
    filters.hierarchy.product_ = settings.value("ShowProduct", true).toBool();
    filters.hierarchy.board_ = settings.value("ShowBoard", true).toBool();
    filters.hierarchy.chip_ = settings.value("ShowChip", true).toBool();
    filters.hierarchy.soc_ = settings.value("ShowSoC", true).toBool();
    filters.hierarchy.ip_ = settings.value("ShowIP", true).toBool();
    settings.endGroup();
    settings.beginGroup("Firmness");
    filters.firmness.templates_ = settings.value("ShowTemplates", true).toBool();
    filters.firmness.mutable_ = settings.value("ShowMutable", true).toBool();
    filters.firmness.fixed_ = settings.value("ShowFixed", true).toBool();
    settings.endGroup();
    settings.beginGroup("Validity");
    filters.validity.valid_ = settings.value("ShowValid", true).toBool();
    filters.validity.invalid_ = settings.value("ShowErrors", true).toBool();
    settings.endGroup();

    filters_.setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::saveFilterSettings()
//-----------------------------------------------------------------------------
void VLNVDialer::saveFilterSettings(QSettings& settings) const
{
    Utils::FilterOptions filters = filters_.getFilters();

    // Save filters.
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    settings.setValue("ShowComponents", filters.type.components_);
    settings.setValue("ShowBuses", filters.type.buses_);
    settings.setValue("ShowCatalogs", filters.type.catalogs_);
    settings.setValue("ShowApis", filters.type.apis_);
    settings.setValue("ShowAdvanced", filters.type.advanced_);
    settings.endGroup();
    settings.beginGroup("Implementation");
    settings.setValue("ShowHW", filters.implementation.hw_);
    settings.setValue("ShowSW", filters.implementation.sw_);
    settings.setValue("ShowSystem", filters.implementation.system_);
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    settings.setValue("ShowGlobal", filters.hierarchy.flat_);
    settings.setValue("ShowProduct", filters.hierarchy.product_);
    settings.setValue("ShowBoard", filters.hierarchy.board_);
    settings.setValue("ShowChip", filters.hierarchy.chip_);
    settings.setValue("ShowSoC", filters.hierarchy.soc_);
    settings.setValue("ShowIP", filters.hierarchy.ip_);
    settings.endGroup();
    settings.beginGroup("Firmness");
    settings.setValue("ShowTemplates", filters.firmness.templates_);
    settings.setValue("ShowMutable", filters.firmness.mutable_);
    settings.setValue("ShowFixed", filters.firmness.fixed_);
    settings.endGroup(); // Firmness
    settings.beginGroup("Validity");
    settings.setValue("ShowValid", filters.validity.valid_);
    settings.setValue("ShowErrors", filters.validity.invalid_);
    settings.endGroup(); // Validity
    settings.endGroup(); // LibraryFilters
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::refreshLibrary()
//-----------------------------------------------------------------------------
void VLNVDialer::refreshLibrary()
{
    dialer_.refreshVendors();
}

//-----------------------------------------------------------------------------
// Function: vlnvdialer::onHandleTagFilterChange()
//-----------------------------------------------------------------------------
void VLNVDialer::onHandleTagFilterChange()
{
    QVector<TagData> filteredTags = tagFilter_->getTags();
    emit tagFiltersChanged(filteredTags);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::onHideShowClick()
//-----------------------------------------------------------------------------
void VLNVDialer::onHideShowClick()
{
	hideFilters_ = !hideFilters_;

    filters_.setHidden(hideFilters_);
    dialer_.setHidden(hideFilters_);
    tagGroup_.setHidden(hideFilters_);

    if (hideFilters_)
    {
        hideButton_.setToolTip(tr("Show filters"));
    }
    else
    {
        hideButton_.setToolTip(tr("Hide filters"));
    }

    QSettings settings;
    settings.setValue("FilterWidget/Hidden", hideFilters_);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::onSelectAll()
//-----------------------------------------------------------------------------
void VLNVDialer::onSelectAll()
{
    allSelected_ = !allSelected_;
    if (allSelected_)
    {
        selectAllButton_.setToolTip("Clear all");
        selectAllButton_.setIcon(QIcon(":/icons/common/graphics/uncheck-all.png"));
    }
    else
    {
        selectAllButton_.setToolTip("Select all");
        selectAllButton_.setIcon(QIcon(":/icons/common/graphics/check-all.png"));
    }

    filters_.selectAll(allSelected_);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::setupLayout()
//-----------------------------------------------------------------------------
void VLNVDialer::setupLayout()
{
    QVBoxLayout* tagGroupLayout(new QVBoxLayout());
    tagGroupLayout->addWidget(tagFilter_);

    tagGroup_.setLayout(tagGroupLayout);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Library Filters"), this), 0, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(&hideButton_, 0, 1, 1, 1, Qt::AlignRight);
    layout->addWidget(&selectAllButton_, 0, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(&dialer_, 1, 0, 1, 3);
    layout->addWidget(&filters_, 2, 0, 1, 3);
    layout->addWidget(&tagGroup_, 3, 0, 1, 3);

    layout->setColumnStretch(0, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 4, 4, 4);
}
