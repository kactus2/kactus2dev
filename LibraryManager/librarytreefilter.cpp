/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarytreefilter.h"
#include "libraryitem.h"

#include "libraryinterface.h"
#include <LibraryManager/VLNVDialer/vlnvdialer.h>
#include <models/librarycomponent.h>
#include <models/busdefinition.h>

#include <QRegExp>

#include <QDebug>

LibraryTreeFilter::LibraryTreeFilter(LibraryInterface* handler, 
									 VLNVDialer* dialer,
									 QAbstractItemModel* sourceModel,
		QObject *parent):
QSortFilterProxyModel(parent), 
handler_(handler),
dialer_(dialer),
firmness_(),
implementation_(),
hierarchy_(),
vendorValidator_(this),
libraryValidator_(this),
nameValidator_(this),
versionValidator_(this),
vendor_(),
library_(),
name_(),
version_() {

	Q_ASSERT(handler);
	Q_ASSERT(dialer);

	// set settings for filter
	setSortLocaleAware(true);
	setSortCaseSensitivity(Qt::CaseInsensitive);

	QRegExp regExp(QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);
	libraryValidator_.setRegExp(regExp);
	nameValidator_.setRegExp(regExp);
	versionValidator_.setRegExp(regExp);

	setSourceModel(sourceModel);

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

LibraryTreeFilter::~LibraryTreeFilter() {
}

bool LibraryTreeFilter::filterAcceptsRow(int sourceRow,
                const QModelIndex& sourceParent) const {

	QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

	// root item is always ok
	if (!itemIndex.isValid())
		return true;

	LibraryItem* item = static_cast<LibraryItem*>(itemIndex.internalPointer());

	// get all vlnvs for the item
	QList<VLNV> list;
	item->getVLNVs(list);

	// if none of the vlnvs match the search rules
	if (!checkVLNVs(list)) {
		return false;
	}
	
	foreach (VLNV vlnv, list) {

		if (!handler_->contains(vlnv)) {
			continue;
		}

		QSharedPointer<LibraryComponent const> libComb = handler_->getModelReadOnly(vlnv);

		// check the type
		switch (handler_->getDocumentType(vlnv)) {
			case VLNV::COMPONENT: {
				// if components are not be displayed
				if (!type_.components_) {
					continue;
				}
				break;
								  }
			case VLNV::ABSTRACTIONDEFINITION: {
				// if buses are not to be displayed
				if (!type_.buses_) {
					continue;
				}

				// abstraction definitions are always for hw

				// if hw is not shown then continue to check next 
				if (!implementation_.hw_)
					continue;

				else {
					return true;
				}
											  }
			case VLNV::BUSDEFINITION: {

				// if buses are not to be displayed
				if (!type_.buses_) {
					continue;
				}
				QSharedPointer<BusDefinition const> busDef = libComb.staticCast<BusDefinition const>();

				// if this was not supposed to show then check next one
				if  (!implementation_.hw_)
					continue;
				else
					return true;
									  }

            case VLNV::COMDEFINITION:
            case VLNV::APIDEFINITION: {

                // if buses are not to be displayed
                if (!type_.buses_) {
                    continue;
                }

                return true;
                                      }
			// if type is one of the advanced
			default:

				// if other types should be displayed
				if (type_.advanced_) {
					return true;
				}

				// if other types are not to be displayed then check other possible
				// vlnvs
				continue;
		}

		// the vlnv is for component for sure
		QSharedPointer<Component const> component = libComb.staticCast<Component const>();

		// if component does not match the filters
		if (!checkImplementation(component) || !checkHierarchy(component) || !checkFirmness(component)) {
			continue;
		}

		// if all tests passed
		return true;
	}

	return false;
}

void LibraryTreeFilter::onVendorChanged( const QString& vendorText ) {

	// update the reg exp for validator
	QRegExp regExp(vendorText, Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);

	invalidateFilter();
}

void LibraryTreeFilter::onLibraryChanged( const QString& libraryText ) {

	// update the reg exp for validator
	QRegExp regExp(libraryText, Qt::CaseInsensitive, QRegExp::Wildcard);
	libraryValidator_.setRegExp(regExp);

	invalidateFilter();
}

void LibraryTreeFilter::onNameChanged( const QString& nameText ) {

	// update the reg exp for validator
	QRegExp regExp(nameText, Qt::CaseInsensitive, QRegExp::Wildcard);
	nameValidator_.setRegExp(regExp);

	invalidateFilter();
}

void LibraryTreeFilter::onVersionChanged( const QString& versionText ) {

	// update the reg exp for validator
	QRegExp regExp(versionText, Qt::CaseInsensitive, QRegExp::Wildcard);
	versionValidator_.setRegExp(regExp);

	invalidateFilter();
}

void LibraryTreeFilter::onFirmnessChanged( const Utils::FirmnessOptions& options ) {
	firmness_ = options;
	invalidateFilter();
}

void LibraryTreeFilter::onImplementationChanged( const Utils::ImplementationOptions& options ) {
	implementation_ = options;
	invalidateFilter();
}

void LibraryTreeFilter::onTypeChanged( const Utils::TypeOptions& options ) {
	type_ = options;
	invalidateFilter();
}

void LibraryTreeFilter::onHierarchyChanged( const Utils::HierarchyOptions& options ) {
	hierarchy_ = options;
	invalidateFilter();
}

bool LibraryTreeFilter::checkFirmness( QSharedPointer<Component const> component ) const {
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

bool LibraryTreeFilter::checkImplementation( QSharedPointer<Component const> component ) const {
	Q_ASSERT(component);

	switch (component->getComponentImplementation()) {
		case KactusAttribute::KTS_HW: {
			// if the HW component contains system views then it should be considered also as system
			if (component->hasSystemViews()) {
				return implementation_.hw_ || implementation_.system_;
			}

			// if the component is pure HW 
			return implementation_.hw_;
												}
		case KactusAttribute::KTS_SW: {
			return implementation_.sw_;
												}
		case KactusAttribute::KTS_SYS: {
			return implementation_.system_;
												 }
		default:
			return false;
	}
}

bool LibraryTreeFilter::checkHierarchy( QSharedPointer<Component const> component ) const {
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

bool LibraryTreeFilter::checkVLNVs( const QList<VLNV>& list ) const {
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
