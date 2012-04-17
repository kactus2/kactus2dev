/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyitem.cpp
 *		Project: Kactus 2
 */

#include "hierarchyitem.h"

#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>
#include <models/librarycomponent.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <QDebug>

HierarchyItem::HierarchyItem(LibraryInterface* handler,
							 HierarchyItem* parent, 
							 VLNV* vlnvP ):
QObject(parent),
component_(),
busDef_(),
absDef_(),
comDef_(),
apiDef_(),
handler_(handler),
childItems_(),
parentItem_(parent),
isValid_(true),
isDuplicate_(false),
type_(HierarchyItem::ROOT) {

	Q_ASSERT_X(handler, "HierarchyItem constructor",
		"Null LibraryInterface pointer given as parameter");
	Q_ASSERT_X(vlnvP, "HierarchyItem constructor",
		"Invalid VLNV given as parameter");
	Q_ASSERT_X(parent, "HierarchyItem constructor",
		"Null parent pointer given as parameter");

	VLNV vlnv = *vlnvP;

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	Q_ASSERT(handler->contains(vlnv));

	if (handler_->getDocumentType(vlnv) == VLNV::COMPONENT) {
		parseComponent(vlnv);
	}
	else if (handler_->getDocumentType(vlnv) == VLNV::BUSDEFINITION) {
		parseBusDefinition(vlnv);
	}
	else if (handler_->getDocumentType(vlnv) == VLNV::ABSTRACTIONDEFINITION) {
		parseAbsDefinition(vlnv);
	}
    else if (handler_->getDocumentType(vlnv) == VLNV::COMDEFINITION) {
        parseComDefinition(vlnv);
    }
    else if (handler_->getDocumentType(vlnv) == VLNV::APIDEFINITION) {
        parseApiDefinition(vlnv);
    }
	else {
		emit errorMessage(tr("VLNV %1:%2:%3:%4 was not for supported item").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		isValid_ = false;
		return;
	}

	
}

HierarchyItem::HierarchyItem( LibraryInterface* handler, QObject* parent ):
QObject(parent),
component_(),
handler_(handler),
childItems_(),
parentItem_(NULL),
isValid_(true),
isDuplicate_(false),
type_(HierarchyItem::ROOT) {
}

void HierarchyItem::parseComponent( const VLNV& vlnv ) {

	type_ = HierarchyItem::COMPONENT;

	// parse the component model
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	Q_ASSERT(libComp);
	component_ = libComp.staticCast<Component>();
	Q_ASSERT(component_);

	QStringList errors;
	isValid_ = component_->isValid(errors);

	// ask the hierarchical references from the component
	QList<VLNV> refs = component_->getHierRefs();
	foreach (VLNV ref, refs) {

		VLNV reference = ref;
		reference.setType(handler_->getDocumentType(ref));

		// make sure the library contains the vlnv
		if (!handler_->contains(reference)) {
			emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
				reference.getVendor()).arg(
				reference.getLibrary()).arg(
				reference.getName()).arg(
				reference.getVersion()));
			isValid_ = false;
			continue;
		}

		QSharedPointer<Design> design;

		// parse the library item
		QSharedPointer<LibraryComponent> temp = handler_->getModel(reference);

		// if the reference was for design configuration
		if (handler_->getDocumentType(reference) == VLNV::DESIGNCONFIGURATION) {

			QSharedPointer<DesignConfiguration> desConf = temp.dynamicCast<DesignConfiguration>();

			VLNV designVLNV = desConf->getDesignRef();
			designVLNV.setType(VLNV::DESIGN);

			if (!handler_->contains(designVLNV) || 
				handler_->getDocumentType(designVLNV) != VLNV::DESIGN) {
					emit errorMessage(tr("Design %1:%2:%3:%4 was not found in the library").arg(
						reference.getVendor()).arg(
						reference.getLibrary()).arg(
						reference.getName()).arg(
						reference.getVersion()));
					isValid_ = false;
					continue;
			}

			QSharedPointer<LibraryComponent> desTemp = handler_->getModel(designVLNV);
			design = desTemp.dynamicCast<Design>();
		}

		// if hierarchy reference was directly for design
		else if (handler_->getDocumentType(reference) == VLNV::DESIGN) {

			QSharedPointer<LibraryComponent> desTemp = handler_->getModel(reference);
			design = desTemp.dynamicCast<Design>();
		}

		// if the hierarhy reference was not for design or design configuration
		else {
			emit errorMessage(tr("VLNV %1:%2:%3:%4 was not valid hierarchy reference").arg(
				reference.getVendor()).arg(
				reference.getLibrary()).arg(
				reference.getName()).arg(
				reference.getVersion()));
			isValid_ = false;
			continue;
		}

		Q_ASSERT_X(design, "HierarchyItem constructor",
			"Invalid design pointer");

		// take all components referenced by the design
		QList<VLNV> components = design->getComponents();
		foreach (VLNV compVLNV, components) {

			if (!handler_->contains(compVLNV)) {
				emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
					compVLNV.getVendor()).arg(
					compVLNV.getLibrary()).arg(
					compVLNV.getName()).arg(
					compVLNV.getVersion()));

				// mark this object as invalid because not all items were found
				isValid_ = false;
				continue;
			}

			else if (handler_->getDocumentType(compVLNV) != VLNV::COMPONENT) {
				emit errorMessage(tr("VLNV %1:%2:%3:%4 was not for component").arg(
					compVLNV.getVendor()).arg(
					compVLNV.getLibrary()).arg(
					compVLNV.getName()).arg(
					compVLNV.getVersion()));

				// mark this object as invalid because not all items were found
				isValid_ = false;
				continue;
			}

			else if (getVLNV() == compVLNV || hasParent(compVLNV)) {
				emit errorMessage(tr("Cyclic instantiation for vlnv %1:%2:%3:%4 was found").arg(
					compVLNV.getVendor()).arg(
					compVLNV.getLibrary()).arg(
					compVLNV.getName()).arg(
					compVLNV.getVersion()));
				isValid_ = false;
				continue;
			}

			// if item already has a child with given VLNV then don't create duplicate.
			if (hasChild(compVLNV))
				continue;

			VLNV* temp = handler_->getOriginalPointer(compVLNV);

			// create a child item that matches the referenced component
			HierarchyItem* item = new HierarchyItem(handler_, this, temp);
			connect(item, SIGNAL(errorMessage(const QString&)),
				this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
			connect(item, SIGNAL(noticeMessage(const QString&)),
				this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
			childItems_.append(item);
		}
	}
}

void HierarchyItem::parseBusDefinition( const VLNV& vlnv ) {
	type_ = HierarchyItem::BUSDEFINITION;

	// parse the bus definition model
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	busDef_ = libComp.staticCast<BusDefinition>();

	QStringList errors;
	isValid_ = busDef_->isValid(errors);
}

void HierarchyItem::parseAbsDefinition( const VLNV& vlnv ) {
	type_ = HierarchyItem::ABSDEFINITION;

	// parse the abstraction definition model
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	absDef_ = libComp.staticCast<AbstractionDefinition>();

	QStringList errors;
	isValid_ = absDef_->isValid(errors);
}

void HierarchyItem::parseComDefinition( const VLNV& vlnv ) {
    type_ = HierarchyItem::COMDEFINITION;

    // parse the COM definition model
    QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
    comDef_ = libComp.staticCast<ComDefinition>();

    QStringList errors;
    isValid_ = comDef_->isValid(errors);
}

void HierarchyItem::parseApiDefinition( const VLNV& vlnv ) {
    type_ = HierarchyItem::APIDEFINITION;

    // parse the API definition model
    QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
    apiDef_ = libComp.staticCast<ApiDefinition>();

    QStringList errors;
    isValid_ = apiDef_->isValid(errors);
}

void HierarchyItem::createChild(VLNV* vlnv ) {

	// if item already has a child for given vlnv
	if (hasChild(*vlnv))
		return;
	// if the child does not exist in library
	else if (!handler_->contains(*vlnv)) {
		emit errorMessage(tr("The vlnv %1 was not found in the library.").arg(vlnv->toString()));
		return;
	}

	HierarchyItem* item = new HierarchyItem(handler_, this, vlnv);
	childItems_.append(item);
}

HierarchyItem::~HierarchyItem() {
}

VLNV HierarchyItem::getVLNV() const {
	if (type_ == HierarchyItem::COMPONENT && component_)
		return *component_->getVlnv();
	else if (type_ == HierarchyItem::BUSDEFINITION && busDef_)
		return *busDef_->getVlnv();
	else if (type_ == HierarchyItem::ABSDEFINITION && absDef_)
		return *absDef_->getVlnv();
    else if (type_ == HierarchyItem::COMDEFINITION && comDef_)
        return *comDef_->getVlnv();
    else if (type_ == HierarchyItem::APIDEFINITION && apiDef_)
        return *apiDef_->getVlnv();
	return VLNV();
}

HierarchyItem* HierarchyItem::child( int index ) const {
	if (index >= 0 && index < childItems_.size())
		return childItems_[index];
	
	return 0;
}

int HierarchyItem::row() {
	if (parentItem_)
		return parentItem_->childItems_.indexOf(this);

	return -1;
}

int HierarchyItem::getNumberOfChildren() const {
	return childItems_.size();
}

int HierarchyItem::getIndexOf( HierarchyItem* item ) {
	return childItems_.indexOf(item);
}

bool HierarchyItem::hasChildren() const {
	return !childItems_.isEmpty();
}

bool HierarchyItem::isValid() const {
	return isValid_;
}

void HierarchyItem::setValidity( bool valid ) {
	isValid_ = valid;
}

HierarchyItem* HierarchyItem::parent() const {
	return parentItem_;
}

bool HierarchyItem::hasChild( const VLNV& vlnv ) {

	foreach (HierarchyItem* item, childItems_) {
		if (item->getVLNV() == vlnv)
			return true;
	}
	return false;
}

bool HierarchyItem::contains( const VLNV& vlnv ) const {
	// if this component matches the vlnv
	if (component_ && *component_->getVlnv() == vlnv)
		return true;

	// if bus definition matches the vlnv
	else if (busDef_ && *busDef_->getVlnv() == vlnv)
		return true;

	// if abstraction definition matches the vlnv
	else if (absDef_ && *absDef_->getVlnv() == vlnv)
		return true;

    // if COM definition matches the vlnv
    else if (comDef_ && *comDef_->getVlnv() == vlnv)
        return true;

    // if API definition matches the vlnv
    else if (apiDef_ && *apiDef_->getVlnv() == vlnv)
        return true;

	// ask the children if they have the vlnv
	foreach (HierarchyItem* item, childItems_) {
		if (item->contains(vlnv))
			return true;
	}
	return false;
}

void HierarchyItem::cleanUp() {

	// make sure this is called only for root item
	Q_ASSERT_X(!component_, "HierarchyItem::cleanUp",
		"Function was called for non-root item");

	QList<HierarchyItem*> itemsToRemove;

	// check the vlnv of each item
	foreach (HierarchyItem* item, childItems_) {
		// the vlnv to search
		VLNV vlnv = item->getVLNV();

		foreach (HierarchyItem* item2, childItems_) {
			
			// if item is not the same but contains the other one and the other
			// is not yet on the list to be removed
			if (item != item2 && item2->contains(vlnv) && !itemsToRemove.contains(item2)) {
				
				item->setDuplicate(true);

				break;
			}
		}
	}
}

QSharedPointer<Component> HierarchyItem::component() const {
	return component_;
}

VLNV* HierarchyItem::getVLNVPointer() const {
	return handler_->getOriginalPointer(getVLNV());
}

int HierarchyItem::referenceCount(const VLNV& vlnv) const {
	int count = 0;

	// add the reference count of all child items.
	foreach (HierarchyItem* item, childItems_) {
		
		// if item itself matches the vlnv and this item is not the root.
		// root must be excluded because it has all items as it's children so 
		// the real count does not include direct children of root
		if (parentItem_ && item->getVLNV() == vlnv)
			++count;
		
		// tell item to search it's children
		else
			count += item->referenceCount(vlnv);
	}
	return count;
}

int HierarchyItem::getOwners( QList<VLNV>& list, const VLNV& vlnvToSearch ) const {

	foreach (HierarchyItem* item, childItems_) {
		
		// if item itself matches the vlnv and this item is not the root.
		// root must be excluded because it has all items as it's children so 
		// the real count does not include direct children of root
		// also make sure the vlnvs are appended only once
		if (parentItem_ && item->getVLNV() == vlnvToSearch && !list.contains(getVLNV()))
			list.append(getVLNV());

		// tell item to search it's children
		else
			item->getOwners(list, vlnvToSearch);
	}
	return list.size();
}

int HierarchyItem::removeItems( const VLNV& vlnv ) {
	
	QList<HierarchyItem*> childrenToRemove;

	// check each child
	foreach (HierarchyItem* item, childItems_) {
		
		// if item has the vlnv then it should be removed
		if (item->getVLNV() == vlnv) 
			childrenToRemove.append(item);

		// otherwise tell item to check it's own children
		else
			item->removeItems(vlnv);
	}

	int count = childrenToRemove.size();

	// remove the children that were marked to be removed.
	foreach (HierarchyItem* item, childrenToRemove) {

		int index = childItems_.indexOf(item);
		delete item;
		childItems_.removeAt(index);

		// if at least one item is removed then item is no longer valid 
		// because the design contains references to item that are not in the
		// library. If type is not component then this of course remains valid.
		if (type_ == HierarchyItem::COMPONENT)
			isValid_ = false;
	}

	return count;
}

QList<HierarchyItem*> HierarchyItem::findItems( const VLNV& vlnv ) {
	QList<HierarchyItem*> items;

	if (getVLNV() == vlnv)
		items.append(this);

	foreach (HierarchyItem* item, childItems_) {
		items += item->findItems(vlnv);
	}
	return items;
}

void HierarchyItem::updateItems( const VLNV& vlnv ) {
	// remove direct children with given vlnv
	int count = removeItems(vlnv);

	// tell remaining children to update their children if needed
	foreach (HierarchyItem* item, childItems_) {
		item->updateItems(vlnv);
	}

	// if at least one child item was removed then create it again.
	if (count > 0) {

		VLNV* vlnvP = handler_->getOriginalPointer(vlnv);

		// if a child was removed then create a new child for the component
		HierarchyItem* item = new HierarchyItem(handler_, this, vlnvP);
		childItems_.append(item);
	}
}

bool HierarchyItem::hasParent( const VLNV& vlnv ) {
	
	// if this is not root
	if (parentItem_) {

		// if item itself has the vlnv
		if (parentItem_->getVLNV() == vlnv)
			return true;

		return parentItem_->hasParent(vlnv);
	}
	// if root item and match was not found.
	else
		return false;
}

QList<VLNV> HierarchyItem::getVLNVs() const {
	QList<VLNV> list;
	
	// if this is not root item
	if (parentItem_) {

		// add this vlnv to the list
		list.append(getVLNV());

		// append parent item's list of vlnvs
		list += parentItem_->getVLNVs();
		return list;
	}
	// if was root item this does not contain VLNVs
	else 
		return list;
}

bool HierarchyItem::isRoot() const {
	if (!parentItem_)
		return true;
	else
		return false;
}

bool HierarchyItem::parentIsRoot() const {
	if (!parentItem_)
		return false;
	return parentItem_->isRoot();
}

bool HierarchyItem::isDuplicate() const {
	return isDuplicate_;
}

void HierarchyItem::setDuplicate( bool isDuplicate ) {
	isDuplicate_ = isDuplicate;
}

void HierarchyItem::clear() {
	qDeleteAll(childItems_);
	childItems_.clear();
}

bool HierarchyItem::isHierarchical() const {
	Q_ASSERT(component_);
	return component_->isHierarchical();
}

HierarchyItem::ObjectType HierarchyItem::type() const {
	return type_;
}

void HierarchyItem::getChildren( QList<VLNV>& childList, const VLNV& owner ) {

	// make sure this is called only for root item
	Q_ASSERT_X(!component_, "HierarchyItem::cleanUp",
		"Function was called for non-root item");

	// search each child item
	foreach (HierarchyItem* item, childItems_) {

		// if child matches the owner
		if (item->getVLNV() == owner) {

			// ask child to append it's children
			item->getChildItems(childList);
			break;
		}
	}
}

void HierarchyItem::getChildItems( QList<VLNV>& itemList ) {

	// go through each child item
	foreach (HierarchyItem* item, childItems_) {

		// if child's vlnv is not yet on the list
		if (!itemList.contains(item->getVLNV()))
			itemList.append(item->getVLNV());

		// ask child to add its' children
		item->getChildItems(itemList);
	}
}

KactusAttribute::Implementation HierarchyItem::getImplementation() const {
	Q_ASSERT(component_);
	return component_->getComponentImplementation();
}

KactusAttribute::SWType HierarchyItem::getSoftwareType() const {
	// make sure item is software item
	Q_ASSERT(getImplementation() == KactusAttribute::KTS_SW);
	return component_->getComponentSWType();
}


KactusAttribute::BusDefType HierarchyItem::getBusDefType() {
	// make sure item is for bus def or abs def
	Q_ASSERT(busDef_ || absDef_);

	if (busDef_) {
		return busDef_->getType();
	}
	else if (absDef_) {
		VLNV busVLNV = absDef_->getBusType();
		
		// if the vlnv is not found
		if (!handler_->contains(busVLNV)) {
			emit errorMessage(tr("VLNV %1 referenced in abstraction definition %2"
				" was not found in library.").arg(busVLNV.toString()).arg(getVLNV().toString()));
			return KactusAttribute::KTS_BUSDEF_HW;
		}

		// make sure the vlnv is for bus definition
		if (handler_->getDocumentType(busVLNV) == VLNV::BUSDEFINITION) {
			QSharedPointer<LibraryComponent> libComp = handler_->getModel(busVLNV);
			QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();

			return busDef->getType();
		}
		// if vlnv was not for bus definition
		else {
			emit errorMessage(tr("VLNV %1 referenced in abstraction definition %2"
				" was not for bus definition").arg(busVLNV.toString()).arg(getVLNV().toString()));
			return KactusAttribute::KTS_BUSDEF_HW;
		}
	}

	Q_ASSERT(false);
	return KactusAttribute::KTS_BUSDEF_HW;
}
