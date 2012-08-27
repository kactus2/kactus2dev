/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyfilter.cpp
 *		Project: Kactus 2
 */

#include "hierarchyfilter.h"

#include <common/KactusAttribute.h>
#include <LibraryManager/VLNVDialer/vlnvdialer.h>
#include "hierarchyitem.h"

#include <QRegExp>

#include <QDebug>

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

HierarchyFilter::~HierarchyFilter() {
}

void HierarchyFilter::onVendorChanged( const QString& vendorText ) {
	vendor_ = vendorText;

	// update the reg exp for validator
	QRegExp regExp(vendorText, Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);

	invalidateFilter();
}

void HierarchyFilter::onLibraryChanged( const QString& libraryText ) {
	library_ = libraryText;

	// update the reg exp for validator
	QRegExp regExp(libraryText, Qt::CaseInsensitive, QRegExp::Wildcard);
	libraryValidator_.setRegExp(regExp);

	invalidateFilter();
}

void HierarchyFilter::onNameChanged( const QString& nameText ) {
	name_ = nameText;

	// update the reg exp for validator
	QRegExp regExp(nameText, Qt::CaseInsensitive, QRegExp::Wildcard);
	nameValidator_.setRegExp(regExp);

	invalidateFilter();
}

void HierarchyFilter::onVersionChanged( const QString& versionText ) {
	version_ = versionText;

	// update the reg exp for validator
	QRegExp regExp(versionText, Qt::CaseInsensitive, QRegExp::Wildcard);
	versionValidator_.setRegExp(regExp);

	invalidateFilter();
}

void HierarchyFilter::onFirmnessChanged( const Utils::FirmnessOptions& options ) {
	firmness_ = options;
	invalidateFilter();
}

void HierarchyFilter::onImplementationChanged( const Utils::ImplementationOptions& options ) {
	implementation_ = options;
	invalidateFilter();
}

void HierarchyFilter::onTypeChanged( const Utils::TypeOptions& options ) {
	type_ = options;
	invalidateFilter();
}

void HierarchyFilter::onHierarchyChanged( const Utils::HierarchyOptions& options ) {
	hierarchy_ = options;
	invalidateFilter();
}

bool HierarchyFilter::filterAcceptsRow( int source_row,
									   const QModelIndex& source_parent ) const {

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
			if (!type_.components_)
				return false;
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
	}

	QSharedPointer<Component> component = item->component();

	QList<VLNV> list = item->getVLNVs();

	if (component) {
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

bool HierarchyFilter::checkVLNVs( const QList<VLNV>& list ) const {

	foreach (VLNV vlnv, list) {

		int pos = 0;

                // QT library Forced to use temp variables
                QString vendor = vlnv.getVendor();
                QString library =  vlnv.getLibrary();
                QString name =  vlnv.getName();
                QString version =  vlnv.getVersion();


                // if the vlnv matches the search rules
                if ((vendorValidator_.validate(vendor, pos) ==
                        QValidator::Acceptable) &&
                        (libraryValidator_.validate(library, pos) ==
                        QValidator::Acceptable) &&
                        (nameValidator_.validate(name, pos) ==
                        QValidator::Acceptable) &&
                        (versionValidator_.validate(version, pos) ==
                        QValidator::Acceptable)) {
				return true;
			
		}

	}

	// if none of the vlnvs matched
	return false;
}

bool HierarchyFilter::checkFirmness(QSharedPointer<Component> component) const {
	Q_ASSERT(component);

	switch (component->getComponentFirmness()) {
		case KactusAttribute::KTS_TEMPLATE: 
			return firmness_.templates_;
											
		case KactusAttribute::KTS_MUTABLE: 
			return firmness_.mutable_;

		case KactusAttribute::KTS_PARAMETERIZABLE: 
			return firmness_.parameterizable_;

		case KactusAttribute::KTS_FIXED: 
			return firmness_.fixed_;

		default:
			return false;
	}
}

bool HierarchyFilter::checkImplementation(QSharedPointer<Component> component) const {
	Q_ASSERT(component);

	switch (component->getComponentImplementation()) {
		case KactusAttribute::KTS_HW:
			return implementation_.hw_;

		case KactusAttribute::KTS_SW:
			return implementation_.sw_;

		case KactusAttribute::KTS_SYS:
			return implementation_.system_;

		default:
			return false;
	} 
}

bool HierarchyFilter::checkHierarchy(QSharedPointer<Component> component) const {
	Q_ASSERT(component);

	switch (component->getComponentHierarchy()) {
		case KactusAttribute::KTS_GLOBAL:
			return hierarchy_.global_;

		case KactusAttribute::KTS_PRODUCT:
			return hierarchy_.product_;

		case KactusAttribute::KTS_BOARD:
			return hierarchy_.board_;

		case KactusAttribute::KTS_CHIP:
			return hierarchy_.chip_;

		case KactusAttribute::KTS_SOC:
			return hierarchy_.soc_;

		case KactusAttribute::KTS_IP:
			return hierarchy_.ip_;

		default:
			return false;
	}
}
