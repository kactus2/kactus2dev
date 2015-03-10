/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyfilter.cpp
 *		Project: Kactus 2
 */

#include "hierarchyfilter.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <library/LibraryManager/VLNVDialer/vlnvdialer.h>
#include "hierarchyitem.h"

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::HierarchyFilter()
//-----------------------------------------------------------------------------
HierarchyFilter::HierarchyFilter(VLNVDialer* dialer,
								 QObject *parent):
QSortFilterProxyModel(parent),
dialer_(dialer),
firmness_(),
implementation_(),
type_(),
hierarchy_(),
vendorValidator_(this),
libraryValidator_(this),
nameValidator_(this),
versionValidator_(this),
vendor_(),
library_(),
name_(),
version_() {

	Q_ASSERT(dialer);

	// set settings for filter
	setSortLocaleAware(true);
	setSortCaseSensitivity(Qt::CaseInsensitive);

	QRegExp regExp(QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);
	libraryValidator_.setRegExp(regExp);
	nameValidator_.setRegExp(regExp);
	versionValidator_.setRegExp(regExp);

	connect(dialer, SIGNAL(vendorChanged(const QString&)),
		this, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(libraryChanged(const QString&)),
		this, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(nameChanged(const QString&)), 
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(versionChanged(const QString&)),
		this, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
		this, SLOT(onFirmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
		this, SLOT(onImplementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(typeChanged(const Utils::TypeOptions&)),
		this, SLOT(onTypeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
	connect(dialer, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
		this, SLOT(onHierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::~HierarchyFilter()
//-----------------------------------------------------------------------------
HierarchyFilter::~HierarchyFilter() {
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onVendorChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onVendorChanged(QString const& vendorText)
{
	vendor_ = vendorText;

	// update the reg exp for validator
	QRegExp regExp(vendorText, Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onLibraryChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onLibraryChanged(QString const& libraryText)
{
	library_ = libraryText;

	// update the reg exp for validator
	QRegExp regExp(libraryText, Qt::CaseInsensitive, QRegExp::Wildcard);
	libraryValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onNameChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onNameChanged(QString const& nameText) 
{
	name_ = nameText;

	// update the reg exp for validator
	QRegExp regExp(nameText, Qt::CaseInsensitive, QRegExp::Wildcard);
	nameValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onVersionChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onVersionChanged(QString const& versionText)
{
	version_ = versionText;

	// update the reg exp for validator
	QRegExp regExp(versionText, Qt::CaseInsensitive, QRegExp::Wildcard);
	versionValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onFirmnessChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onFirmnessChanged(Utils::FirmnessOptions const& options)
{
	firmness_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onImplementationChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onImplementationChanged(Utils::ImplementationOptions const& options)
{
	implementation_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onTypeChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onTypeChanged(Utils::TypeOptions const& options)
{
	type_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::onHierarchyChanged()
//-----------------------------------------------------------------------------
void HierarchyFilter::onHierarchyChanged(Utils::HierarchyOptions const& options)
{
	hierarchy_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool HierarchyFilter::filterAcceptsRow(int source_row, QModelIndex const& source_parent) const
{
	// find the hierarchy item
	QModelIndex itemIndex = sourceModel()->index(source_row, 0, source_parent);

	// root item is always ok
	if (!itemIndex.isValid())
		return true;

	HierarchyItem* item = static_cast<HierarchyItem*>(itemIndex.internalPointer());

	// if no search rules for vlnv are defined then display only non-duplicate
	// children of root
	if (vendor_.size() <= 1 && library_.size() <= 1  && name_.size() <= 1 &&
		version_.size() <= 1 && item->parentIsRoot() && item->isDuplicate()) {
			return false;
	}

	// check the type
	switch (item->type()) {
		case HierarchyItem::COMPONENT: {
			if (!type_.components_) {
				return false;
			}
			break;
									   }

        case HierarchyItem::APIDEFINITION:
        case HierarchyItem::COMDEFINITION:
            {
                if (!type_.buses_)
                    return false;

                // if SW should not be displayed then this is not shown
                if (!implementation_.sw_)
                    return false;

                break;
            }

		case HierarchyItem::BUSDEFINITION:
		case HierarchyItem::ABSDEFINITION: {
			if (!type_.buses_)
				return false;

            // if hw should not be displayed then this is not shown
            if (!implementation_.hw_)
                return false;
										   }

        default:
            break;
	}

	QSharedPointer<Component const> component = item->component();

	QList<VLNV> list = item->getVLNVs();

	if (component)
    {
		// check the filters
		if (!checkFirmness(component))
			return false;
		else if (!checkImplementation(component))
			return false;
		else if (!checkHierarchy(component))
			return false;
	}

	if (!checkVLNVs(list))
		return false;
	else 
		return true;
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::checkVLNVs()
//-----------------------------------------------------------------------------
bool HierarchyFilter::checkVLNVs( const QList<VLNV>& list ) const
{
    foreach (VLNV vlnv, list) 
    {
        int pos = 0;

        // QT library Forced to use temp variables
        QString vendor = vlnv.getVendor();
        QString library =  vlnv.getLibrary();
        QString name =  vlnv.getName();
        QString version =  vlnv.getVersion();

        // if the vlnv matches the search rules
        if ((vendorValidator_.validate(vendor, pos) == QValidator::Acceptable) &&
            (libraryValidator_.validate(library, pos) == QValidator::Acceptable) &&
            (nameValidator_.validate(name, pos) == QValidator::Acceptable) &&
            (versionValidator_.validate(version, pos) == QValidator::Acceptable)) 
        {
                return true;

        }
    }

	// if none of the vlnvs matched
	return false;
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::checkFirmness()
//-----------------------------------------------------------------------------
bool HierarchyFilter::checkFirmness(QSharedPointer<Component const> component) const
{
	Q_ASSERT(component);

    KactusAttribute::Firmness componentFirmness = component->getComponentFirmness();
    if (componentFirmness == KactusAttribute::TEMPLATE) 
    {
        return firmness_.templates_;
    }
    else if (componentFirmness == KactusAttribute::MUTABLE)
    {	
        return firmness_.mutable_;
    }
    else if (componentFirmness == KactusAttribute::FIXED)
    { 
        return firmness_.fixed_;
    }
    else if (componentFirmness == KactusAttribute::DEFINITIONS)
    { 
        return firmness_.definitions_;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::checkImplementation()
//-----------------------------------------------------------------------------
bool HierarchyFilter::checkImplementation( QSharedPointer<Component const> component ) const
{
	Q_ASSERT(component);

	switch (component->getComponentImplementation()) {
		case KactusAttribute::HW: {

			// if the HW component contains system views then it should be considered also as system
			if (component->hasSystemViews()) {
				return implementation_.hw_ || implementation_.system_;
			}

			// if the component is pure HW 
			return implementation_.hw_;
												}
		case KactusAttribute::SW: {
			return implementation_.sw_;
												}
		case KactusAttribute::SYSTEM: {
			return implementation_.system_;
												 }
		default:
			return false;
	} 
}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::checkHierarchy()
//-----------------------------------------------------------------------------
bool HierarchyFilter::checkHierarchy(QSharedPointer<Component const> component) const
{
	Q_ASSERT(component);

	switch (component->getComponentHierarchy()) {
		case KactusAttribute::FLAT:
			return hierarchy_.flat_;

		case KactusAttribute::PRODUCT:
			return hierarchy_.product_;

		case KactusAttribute::BOARD:
			return hierarchy_.board_;

		case KactusAttribute::CHIP:
			return hierarchy_.chip_;

		case KactusAttribute::SOC:
			return hierarchy_.soc_;

		case KactusAttribute::IP:
			return hierarchy_.ip_;

		default:
			return false;
	}
}
