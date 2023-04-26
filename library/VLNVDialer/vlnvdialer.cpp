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
// Function: VLNVDialer::setFilters()
//-----------------------------------------------------------------------------
void VLNVDialer::setFilters(Utils::FilterOptions options)
{
    filters_.setFilters(options);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::getFilters()
//-----------------------------------------------------------------------------
Utils::FilterOptions VLNVDialer::getFilters() const
{
    return filters_.getFilters();
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
// Function: VLNVDialer::closeEvent()
//-----------------------------------------------------------------------------
void VLNVDialer::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("FilterWidget/Hidden", hideFilters_);
    QWidget::closeEvent(event);
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
