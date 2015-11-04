//-----------------------------------------------------------------------------
// File: hierarchyitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// Represents a single component in the library in hierarchy view.
//-----------------------------------------------------------------------------

#include "hierarchyitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: HierarchyItem::HierarchyItem()
//-----------------------------------------------------------------------------
HierarchyItem::HierarchyItem(LibraryInterface* handler, HierarchyItem* parent, 
    VLNV const& vlnv, KactusAttribute::Implementation implementation, QString const& viewName):
QObject(parent),
    component_(),
    busDef_(),
    absDef_(),
    comDef_(),
    apiDef_(),
    design_(),
    handler_(handler),
    childItems_(),
    parentItem_(parent),
    isValid_(true),
    isDuplicate_(false),
    type_(HierarchyItem::ROOT),
    instanceCount_(),
    viewName_()
{
    Q_ASSERT_X(handler, "HierarchyItem constructor", "Null LibraryInterface pointer given as parameter");
    Q_ASSERT_X(parent, "HierarchyItem constructor", "Null parent pointer given as parameter");

    connect(this, SIGNAL(errorMessage(QString const&)),
        parent, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(this, SIGNAL(noticeMessage(QString const&)),
        parent, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    Q_ASSERT(handler->contains(vlnv));

    VLNV::IPXactType documentType = handler_->getDocumentType(vlnv);
    if (documentType == VLNV::COMPONENT)
    {
        parseComponent(vlnv);
    }
    else if (documentType == VLNV::BUSDEFINITION)
    {
        parseBusDefinition(vlnv);
    }
    else if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        parseAbsDefinition(vlnv);
    }
    else if (documentType == VLNV::COMDEFINITION)
    {
        parseComDefinition(vlnv);
    }
    else if (documentType == VLNV::APIDEFINITION)
    {
        parseApiDefinition(vlnv);
    }
    else if (documentType == VLNV::DESIGN)
    {
        parseDesign(vlnv, implementation, viewName);
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not for supported item").arg(vlnv.toString()));
        isValid_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::HierarchyItem()
//-----------------------------------------------------------------------------
HierarchyItem::HierarchyItem(LibraryInterface* handler, QObject* parent ):
QObject(parent),
    component_(),
    busDef_(),
    absDef_(),
    comDef_(),
    apiDef_(),
    design_(),
    handler_(handler),
    childItems_(),
    parentItem_(NULL),
    isValid_(true),
    isDuplicate_(false),
    type_(HierarchyItem::ROOT),
    instanceCount_(),
    viewName_()
{
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::~HierarchyItem()
//-----------------------------------------------------------------------------
HierarchyItem::~HierarchyItem()'' {
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseComponent()
//-----------------------------------------------------------------------------
void HierarchyItem::parseComponent(VLNV const& vlnv)
{
	type_ = HierarchyItem::COMPONENT;

	QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
	Q_ASSERT(libComp);
	component_ = libComp.staticCast<Component const>();
	Q_ASSERT(component_);

	isValid_ = handler_->isValid(vlnv);

	// ask the hierarchical references from the component
	foreach (VLNV const& ref, component_->getHierRefs())
    {
		VLNV reference = ref;
		reference.setType(handler_->getDocumentType(ref));

		// make sure the library contains the vlnv
		if (!handler_->contains(reference))
        {
			emit errorMessage(tr("VLNV %1 was not found in the library").arg(reference.toString()));
			isValid_ = false;
			continue;
		}

		// the vlnv for the design
		VLNV designVLNV;

		// if the reference was for design configuration
		if (handler_->getDocumentType(reference) == VLNV::DESIGNCONFIGURATION)
        {
			QSharedPointer<Document const> temp = handler_->getModelReadOnly(reference);
			QSharedPointer<DesignConfiguration const> desConf = temp.dynamicCast<DesignConfiguration const>();
			designVLNV = desConf->getDesignRef();
			designVLNV.setType(VLNV::DESIGN);
		}

		// if hierarchy reference was directly for design
		else if (handler_->getDocumentType(reference) == VLNV::DESIGN)
        {
			designVLNV = reference;
		}
		// if the reference is not design configuration or design
		else
        {
            emit errorMessage(tr("VLNV %1 was not valid hierarchy reference.").arg(reference.toString()));
            isValid_ = false;
            continue;
        }

        // if the design was not found
        if (!handler_->contains(designVLNV))
        {
            emit errorMessage(tr("Design reference %1 was not found in library.").arg(designVLNV.toString()));
            isValid_ = false;
            continue;
        }
        // if the reference was for some other kind of object
        else if (handler_->getDocumentType(designVLNV) != VLNV::DESIGN)
        {
            emit errorMessage(tr("Design reference %1 was for wrong type of object.").arg(designVLNV.toString()));
            isValid_ = false;
            continue;
        }
        // if the component already has this the design under it
        else if (hasChild(designVLNV)) 
        {
            continue;
        }

		KactusAttribute::Implementation implementation = component_->getViewType(reference);
		QString viewName = component_->getViewName(reference);

		// create a child item that matches the referenced design
		HierarchyItem* item = new HierarchyItem(handler_, this, designVLNV, implementation, viewName);
		connect(item, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
		connect(item, SIGNAL(noticeMessage(QString const&)),
            this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

		childItems_.append(item);
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseDesign()
//-----------------------------------------------------------------------------
void HierarchyItem::parseDesign(VLNV const& vlnv, KactusAttribute::Implementation implementation,
    QString const& viewName)
{
	viewName_ = viewName;

    if (implementation == KactusAttribute::HW)
    {
        type_ = HierarchyItem::HW_DESIGN;
    }
    else if (implementation == KactusAttribute::SW)
    {
        type_ = HierarchyItem::SW_DESIGN;
    }
    else if (implementation == KactusAttribute::SYSTEM)
    {
        type_ = HierarchyItem::SYS_DESIGN;
    }
    else
    {
        Q_ASSERT(false);
    }

	// parse the design
	QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
	design_ = libComp.staticCast<Design const>();

	isValid_ = handler_->isValid(vlnv);

	// take all components referenced by the design
	foreach (VLNV const& compVLNV, design_->getComponents())
    {
		if (!handler_->contains(compVLNV))
        {
			emit errorMessage(tr("VLNV %1 was not found in the library").arg(compVLNV.toString()));

			// mark this object as invalid because not all items were found
			isValid_ = false;
			continue;
		}

		else if (handler_->getDocumentType(compVLNV) != VLNV::COMPONENT)
        {
			emit errorMessage(tr("VLNV %1 was not for component").arg(compVLNV.toString()));

			// mark this object as invalid because not all items were found
			isValid_ = false;
			continue;
		}

		else if (getVLNV() == compVLNV || hasParent(compVLNV))
        {
			emit errorMessage(tr("Cyclic instantiation for vlnv %1 was found").arg(compVLNV.toString()));
			isValid_ = false;
			continue;
		}

		// if item already has a child with given VLNV then don't create duplicate.
		if (hasChild(compVLNV))
        {
			int count = instanceCount_.value(compVLNV);
			count++;
			instanceCount_[compVLNV] = count;
			continue;
		}

		// create a child item that matches the referenced component
		HierarchyItem* item = new HierarchyItem(handler_, this, compVLNV);
		connect(item, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
		connect(item, SIGNAL(noticeMessage(QString const&)),
			this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);
		childItems_.append(item);
		instanceCount_[compVLNV] = 1;
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseBusDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseBusDefinition(VLNV const& vlnv)
{
	type_ = HierarchyItem::BUSDEFINITION;

	// parse the bus definition model
	QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
	busDef_ = libComp.staticCast<BusDefinition const>();

	isValid_ = handler_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseAbsDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseAbsDefinition(VLNV const& vlnv)
{
	type_ = HierarchyItem::ABSDEFINITION;

	// parse the abstraction definition model
	QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
	absDef_ = libComp.staticCast<AbstractionDefinition const>();

	isValid_ = handler_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseComDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseComDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::COMDEFINITION;

    // parse the COM definition model
    QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
    comDef_ = libComp.staticCast<ComDefinition const>();

    isValid_ = handler_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseApiDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseApiDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::APIDEFINITION;

    // parse the API definition model
    QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
    apiDef_ = libComp.staticCast<ApiDefinition const>();

    isValid_ = handler_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::createChild()
//-----------------------------------------------------------------------------
void HierarchyItem::createChild(VLNV const& vlnv)
{
	// if item already has a child for given vlnv
	if (hasChild(vlnv))
    {
		return;
	}

	// if the child does not exist in library
	if (!handler_->contains(vlnv))
    {
		emit errorMessage(tr("The vlnv %1 was not found in the library.").arg(vlnv.toString()));
		return;
	}

	HierarchyItem* item = new HierarchyItem(handler_, this, vlnv);
	childItems_.append(item);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getVLNV()
//-----------------------------------------------------------------------------
VLNV HierarchyItem::getVLNV() const
{
	if (type_ == HierarchyItem::COMPONENT && component_)
    {
		return component_->getVlnv();
	}
	else if (type_ == HierarchyItem::BUSDEFINITION && busDef_)
    {
		return busDef_->getVlnv();
	}
	else if (type_ == HierarchyItem::ABSDEFINITION && absDef_)
    {
		return absDef_->getVlnv();
	}
	else if (type_ == HierarchyItem::COMDEFINITION && comDef_)
    {
        return comDef_->getVlnv();
	}
	else if (type_ == HierarchyItem::APIDEFINITION && apiDef_)
    {
        return apiDef_->getVlnv();
	}
	else if ((type_ == HierarchyItem::HW_DESIGN ||
		type_ == HierarchyItem::SW_DESIGN ||
		type_ == HierarchyItem::SYS_DESIGN) && design_)
    {
		return design_->getVlnv();
	}

	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::child()
//-----------------------------------------------------------------------------
HierarchyItem* HierarchyItem::child(int index) const
{
	if (0 <= index && index < childItems_.size())
    {
		return childItems_[index];
    }

	return 0;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::row()
//-----------------------------------------------------------------------------
int HierarchyItem::row()
{
	if (parentItem_)
    {
		return parentItem_->childItems_.indexOf(this);
    }

	return -1;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getNumberOfChildren()
//-----------------------------------------------------------------------------
int HierarchyItem::getNumberOfChildren() const
{
	return childItems_.size();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getIndexOf()
//-----------------------------------------------------------------------------
int HierarchyItem::getIndexOf(HierarchyItem* item)
{
	return childItems_.indexOf(item);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::hasChildren()
//-----------------------------------------------------------------------------
bool HierarchyItem::hasChildren() const
{
	return !childItems_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::isValid()
//-----------------------------------------------------------------------------
bool HierarchyItem::isValid() const
{
	return isValid_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::setValidity()
//-----------------------------------------------------------------------------
void HierarchyItem::setValidity(bool valid)
{
	isValid_ = valid;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parent()
//-----------------------------------------------------------------------------
HierarchyItem* HierarchyItem::parent() const
{
	return parentItem_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::hasChild()
//-----------------------------------------------------------------------------
bool HierarchyItem::hasChild(VLNV const& vlnv)
{
	foreach (HierarchyItem* item, childItems_)
    {
		if (item->getVLNV() == vlnv)
        {
			return true;
        }
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::contains()
//-----------------------------------------------------------------------------
bool HierarchyItem::contains(VLNV const& vlnv) const
{
	if (component_ && component_->getVlnv() == vlnv)
    {
		return true;
    }

	else if (busDef_ && busDef_->getVlnv() == vlnv)
    {
		return true;
    }
	
	else if (absDef_ && absDef_->getVlnv() == vlnv)
    {
		return true;
    }
    
    else if (comDef_ && comDef_->getVlnv() == vlnv)
    {
        return true;
    }

    else if (apiDef_ && apiDef_->getVlnv() == vlnv)
    {
        return true;
    }

	foreach (HierarchyItem* item, childItems_)
    {
		if (item->contains(vlnv))
        {
			return true;
        }
	}
	return false;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::cleanUp()
//-----------------------------------------------------------------------------
void HierarchyItem::cleanUp()
{
	// make sure this is called only for root item
	Q_ASSERT_X(!component_, "HierarchyItem::cleanUp", "Function was called for non-root item");

	// check the vlnv of each item
	foreach (HierarchyItem* item, childItems_)
    {
		// the vlnv to search
		VLNV vlnv = item->getVLNV();

		foreach (HierarchyItem* item2, childItems_)
        {			
			// if item is not the same but contains the other one and the other
			// is not yet on the list to be removed
			if (item != item2 && item2->contains(vlnv))
            {				
				item->setDuplicate(true);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::component()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> HierarchyItem::component() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::referenceCount()
//-----------------------------------------------------------------------------
int HierarchyItem::referenceCount(VLNV const& vlnv) const
{
	int count = 0;

	// add the reference count of all child items.
	foreach (HierarchyItem* item, childItems_)
    {		
		// if item itself matches the vlnv and this item is not the root.
		// root must be excluded because it has all items as it's children so 
		// the real count does not include direct children of root
		if (parentItem_ && item->getVLNV() == vlnv)
        {
			count++;
        }
		else
        {
			count += item->referenceCount(vlnv);
        }
	}

	return count;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getOwners()
//-----------------------------------------------------------------------------
int HierarchyItem::getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const
{
	foreach (HierarchyItem* item, childItems_)
    {	
		// if item itself matches the vlnv and this item is not the root.
		// root must be excluded because it has all items as it's children so 
		// the real count does not include direct children of root
		// also make sure the vlnvs are appended only once
		if (parentItem_ && item->getVLNV() == vlnvToSearch && !list.contains(getVLNV()))
        {
			list.append(getVLNV());
        }
		// tell item to search it's children
		else
        {
			item->getOwners(list, vlnvToSearch);
        }
	}

	return list.size();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::removeItems()
//-----------------------------------------------------------------------------
int HierarchyItem::removeItems(VLNV const& vlnv)
{
	QList<HierarchyItem*> childrenToRemove;

	// check each child
	foreach (HierarchyItem* item, childItems_)
    {
		// if item has the vlnv then it should be removed
		if (item->getVLNV() == vlnv) 
        {
			childrenToRemove.append(item);
        }
		// otherwise tell item to check it's own children
		else
        {
			item->removeItems(vlnv);
        }
	}

	int count = childrenToRemove.size();

	foreach (HierarchyItem* item, childrenToRemove)
    {
		int index = childItems_.indexOf(item);
		childItems_.removeAt(index);
        delete item;

		// if at least one item is removed then item is no longer valid 
		// because the design contains references to item that are not in the
		// library. If type is not component then this of course remains valid.
		if (type_ == HierarchyItem::COMPONENT)
        {
			isValid_ = false;
        }
	}

	return count;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::findItems()
//-----------------------------------------------------------------------------
QList<HierarchyItem*> HierarchyItem::findItems(VLNV const& vlnv)
{
	QList<HierarchyItem*> items;

	if (getVLNV() == vlnv)
    {
		items.append(this);
    }

	foreach (HierarchyItem* item, childItems_)
    {
		items += item->findItems(vlnv);
	}

	return items;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::updateItems()
//-----------------------------------------------------------------------------
void HierarchyItem::updateItems(VLNV const& vlnv)
{
	// remove direct children with given vlnv
	int count = removeItems(vlnv);

	// tell remaining children to update their children if needed
	foreach (HierarchyItem* item, childItems_) {
		item->updateItems(vlnv);
	}

	// if at least one child item was removed then create it again.
	if (count > 0) {

		// if a child was removed then create a new child for the component
		HierarchyItem* item = new HierarchyItem(handler_, this, vlnv, KactusAttribute::HW);
		childItems_.append(item);
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::hasParent()
//-----------------------------------------------------------------------------
bool HierarchyItem::hasParent(VLNV const& vlnv)
{
	// if this is not root
	if (parentItem_)
    {
		// if item itself has the vlnv
		if (parentItem_->getVLNV() == vlnv)
        {
			return true;
        }

		return parentItem_->hasParent(vlnv);
	}
	// if root item and match was not found.
	else
    {
		return false;
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> HierarchyItem::getVLNVs() const
{
	// if this is not root item
	if (parentItem_)
    {
        QList<VLNV> list;

		// add this vlnv to the list
		list.append(getVLNV());

		// append parent item's list of vlnvs
		list += parentItem_->getVLNVs();
		return list;
	}
	// if was root item this does not contain VLNVs
	else 
    {
		return QList<VLNV>();
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::isRoot()
//-----------------------------------------------------------------------------
bool HierarchyItem::isRoot() const
{
	return !parentItem_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parentIsRoot()
//-----------------------------------------------------------------------------
bool HierarchyItem::parentIsRoot() const 
{
	if (!parentItem_)
    {
		return false;
    }

	return parentItem_->isRoot();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::isDuplicate()
//-----------------------------------------------------------------------------
bool HierarchyItem::isDuplicate() const
{
	return isDuplicate_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::setDuplicate()
//-----------------------------------------------------------------------------
void HierarchyItem::setDuplicate(bool isDuplicate)
{
	isDuplicate_ = isDuplicate;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::clear()
//-----------------------------------------------------------------------------
void HierarchyItem::clear()
{
	qDeleteAll(childItems_);
	childItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::isHierarchical()
//-----------------------------------------------------------------------------
bool HierarchyItem::isHierarchical() const
{
	Q_ASSERT(component_);

    if (component_->getImplementation() == KactusAttribute::SW)
    {
        return component_->hasSWViews();
    }
    else
    {
	    return component_->isHierarchical();
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::type()
//-----------------------------------------------------------------------------
HierarchyItem::ObjectType HierarchyItem::type() const
{
	return type_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getChildren()
//-----------------------------------------------------------------------------
void HierarchyItem::getChildren(QList<VLNV>& childList, VLNV const& owner)
{
	// make sure this is called only for root item
	Q_ASSERT_X(!component_, "HierarchyItem::getChildren", "Function was called for non-root item");

	// search each child item
	foreach (HierarchyItem* item, childItems_)
    {
		// if child matches the owner
		if (item->getVLNV() == owner)
        {
			// ask child to append it's children
			item->getChildItems(childList);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getChildItems()
//-----------------------------------------------------------------------------
void HierarchyItem::getChildItems(QList<VLNV>& itemList)
{
	// go through each child item
	foreach (HierarchyItem* item, childItems_)
    {
		// if child's vlnv is not yet on the list
		if (!itemList.contains(item->getVLNV()))
        {
			itemList.append(item->getVLNV());
        }

		// ask child to add its' children
		item->getChildItems(itemList);
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation HierarchyItem::getImplementation() const
{
	Q_ASSERT(component_);
	return component_->getImplementation();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::instanceCount()
//-----------------------------------------------------------------------------
int HierarchyItem::instanceCount() const
{
	if (type_ == HierarchyItem::COMPONENT)
    {
		return parentItem_->countInstances(getVLNV());
	}
	else
    {
		return -1;
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::countInstances()
//-----------------------------------------------------------------------------
int HierarchyItem::countInstances(VLNV const& componentVLNV)
{
	if (!instanceCount_.contains(componentVLNV))
    {
		return -1;
	}
	else
    {
		return instanceCount_.value(componentVLNV);
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getViewName()
//-----------------------------------------------------------------------------
QString HierarchyItem::getViewName() const
{
	return viewName_;
}
