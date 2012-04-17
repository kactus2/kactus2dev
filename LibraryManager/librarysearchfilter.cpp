/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarysearchfilter.h"
#include "vlnv.h"

#include <QDebug>

LibrarySearchFilter::LibrarySearchFilter(QObject *parent) :
    QSortFilterProxyModel(parent), showHidden_(false), searchWord_(),
    typeSettings_(), vlnvSettings_() {
}

LibrarySearchFilter::~LibrarySearchFilter() {
}

bool LibrarySearchFilter::filterAcceptsRow(int sourceRow,
		const QModelIndex& sourceParent) const {

	// this is table model so parent can not be valid
	if (sourceParent.isValid()) {
		return false;
	}

	// if the source row is invalid
	else if (sourceRow < 0 || sourceRow >= sourceModel()->rowCount()) {
		return false;
	}

	QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, QModelIndex());

	VLNV* vlnv = static_cast<VLNV*>(itemIndex.internalPointer());

	Q_ASSERT_X(vlnv, "LibrarySearchFilter::filterAcceptsRow",
			"Invalid item pointer after conversion");

	// if item is hidden and hidden items are not the be shown
	if (vlnv->isHidden() && !showHidden_) {
		return false;
	}

	return searchWordMatch(vlnv);
}

void LibrarySearchFilter::onShowStatusChange(int state) {

	// if show hidden items was unchecked
	if (state == Qt::Unchecked) {
		showHidden_ = false;
	}

	// if show hidden items was checked
	else {
		showHidden_ = true;
	}

	// the filter must be invalidated to update the model
	invalidateFilter();
}

void LibrarySearchFilter::onSearchWordChanged(const QString& newWord) {

	// set the new search word and update the model
	searchWord_ = newWord;
	invalidateFilter();
}

bool LibrarySearchFilter::searchWordMatch(VLNV* vlnv) const {

	Q_ASSERT_X(vlnv, "LibrarySearchFilter::searchWordMatch",
			"Invalid item pointer given as parameter");

	// if vendors are searched and match is found
	if (vlnvSettings_.vendor_ &&
			checkTypeMatch(vlnv) &&
			vlnv->getVendor().contains(
			searchWord_, Qt::CaseInsensitive)) {
		return true;
	}

	// if libraries are searched and match is found
	else if (vlnvSettings_.library_ &&
			checkTypeMatch(vlnv) &&
			vlnv->getLibrary().contains(
			searchWord_, Qt::CaseInsensitive)) {
		return true;
	}

	// if names are searched and match is found
	else if (vlnvSettings_.name_ &&
			checkTypeMatch(vlnv) &&
			vlnv->getName().contains(
			searchWord_, Qt::CaseInsensitive)) {
		return true;
	}

	// if versions are searched and match is found
	else if (vlnvSettings_.version_ &&
			checkTypeMatch(vlnv) &&
			vlnv->getVersion().contains(
			searchWord_, Qt::CaseInsensitive)) {
		return true;
	}

	// if none matched
	return false;
}

bool LibrarySearchFilter::checkTypeMatch(VLNV* vlnv) const {

	Q_ASSERT_X(vlnv, "LibrarySearchFilter::checkTypeMatch",
			"Invalid item pointer given as parameter");

	// check the type of the item's vlnv
	switch (vlnv->getType()) {

	// if abstractors are included in search
	case VLNV::ABSTRACTOR: {
		return typeSettings_.abstractors_;
	}
	// if abstractionDefinitions are included in search
	case VLNV::ABSTRACTIONDEFINITION: {
		return typeSettings_.abstractionDefs_;
	}
	// if busDefinitions are included in search
	case VLNV::BUSDEFINITION: {
		return typeSettings_.busDefs_;
	}
    // if comDefinitions are included in search
    case VLNV::COMDEFINITION: {
        return typeSettings_.busDefs_; // TODO: New flag?
    }
    // if apiDefinitions are included in search
    case VLNV::APIDEFINITION: {
        return typeSettings_.busDefs_; // TODO: New flag?
    }
	// if Components are included in search
	case VLNV::COMPONENT: {
		return typeSettings_.components_;
	}
	// if Designs are included in search
	case VLNV::DESIGN: {
		return typeSettings_.designs_;
	}
	// if designConfigurations are included in search
	case VLNV::DESIGNCONFIGURATION: {
		return typeSettings_.designConfs_;
	}
	// if generatorChains are included in search
	case VLNV::GENERATORCHAIN: {
		return typeSettings_.generatorChains_;
	}
	// if the type is invalid
	default: {
		return false;
	}
	}
}

void LibrarySearchFilter::onTypeStateChanged(
		const TypeBox::TypeSearchSettings* newSettings) {

	// update the settings and update the model
	typeSettings_ = *newSettings;
	invalidateFilter();

}

void LibrarySearchFilter::onVLNVStateChanged(
		const VLNVBox::VLNVSearchSettings* newSettings) {

	// update the settings and update the model
	vlnvSettings_ = *newSettings;
	invalidateFilter();
}

/*
QModelIndex LibrarySearchFilter::mapFromSource(const QModelIndex& sourceIndex ) const {

	return createIndex(sourceIndex.row(), sourceIndex.column(), 
		sourceIndex.internalPointer());
}
*/

/*
QModelIndex LibrarySearchFilter::mapToSource( const QModelIndex& proxyIndex ) const {
	
	return createIndex(proxyIndex.row(), proxyIndex.column(),
		proxyIndex.internalPointer());
}
*/