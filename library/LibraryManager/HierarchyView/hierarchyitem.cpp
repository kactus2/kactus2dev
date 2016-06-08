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

#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>

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
    library_(handler),
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

    VLNV::IPXactType documentType = library_->getDocumentType(vlnv);
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
    library_(handler),
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
HierarchyItem::~HierarchyItem()
{
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::createChild()
//-----------------------------------------------------------------------------
void HierarchyItem::createChild(VLNV const& vlnv)
{
	if (hasChild(vlnv))
    {
		return;
	}

	// if the child does not exist in library
	if (!library_->contains(vlnv))
    {
		emit errorMessage(tr("The vlnv %1 was not found in the library.").arg(vlnv.toString()));
		return;
	}

	childItems_.append(new HierarchyItem(library_, this, vlnv));
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parent()
//-----------------------------------------------------------------------------
HierarchyItem* HierarchyItem::parent() const
{
    return parentItem_;
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
	else if ((type_ == HierarchyItem::HW_DESIGN || type_ == HierarchyItem::SW_DESIGN ||
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
		return parentItem_->getIndexOf(this);
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

	foreach (HierarchyItem* item, childItems_)
    {
		VLNV vlnv = item->getVLNV();

		foreach (HierarchyItem* item2, childItems_)
        {			
			// if item is not the same but contains the other one and the other is not yet on the list to be removed
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

	foreach (HierarchyItem* item, childrenToRemove)
    {
		childItems_.removeAt(childItems_.indexOf(item));
        delete item;

		// If at least one item is removed then item is no longer valid because the design contains references 
        // to item that are not in the library. If type is not component then this of course remains valid.
		if (type_ == HierarchyItem::COMPONENT)
        {
			isValid_ = false;
        }
	}

	return childrenToRemove.size();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::findItems()
//-----------------------------------------------------------------------------
QVector<HierarchyItem*> HierarchyItem::findItems(const VLNV& vlnv)
{
	QVector<HierarchyItem*> items;

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
	foreach (HierarchyItem* item, childItems_)
    {
		item->updateItems(vlnv);
	}

	// if at least one child item was removed then create it again.
	if (count > 0)
    {
		HierarchyItem* item = new HierarchyItem(library_, this, vlnv, KactusAttribute::HW);
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
// Function: HierarchyItem::isRoot()
//-----------------------------------------------------------------------------
bool HierarchyItem::isRoot() const
{
    return !parentItem_;
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
// Function: HierarchyItem::getVLNVs()
//-----------------------------------------------------------------------------
QVector<VLNV> HierarchyItem::getVLNVs() const
{
    QVector<VLNV> list;

	// If this is not root item.
	if (parentItem_)
    {
		list.append(getVLNV());

		// append parent item's list of vlnvs
		list += parentItem_->getVLNVs();
	}

	return list;
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
// Function: HierarchyItem::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation HierarchyItem::getImplementation() const
{
    Q_ASSERT(component_);
    return component_->getImplementation();
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
// Function: HierarchyItem::getViewName()
//-----------------------------------------------------------------------------
QString HierarchyItem::getViewName() const
{
    return viewName_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::countInstances()
//-----------------------------------------------------------------------------
int HierarchyItem::countInstances(VLNV const& componentVLNV)
{
    return instanceCount_.value(componentVLNV, -1);
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
// Function: HierarchyItem::parseComponent()
//-----------------------------------------------------------------------------
void HierarchyItem::parseComponent(VLNV const& vlnv)
{
    type_ = HierarchyItem::COMPONENT;
    component_ = library_->getModelReadOnly(vlnv).staticCast<Component const>();
    Q_ASSERT(component_);

    isValid_ = library_->isValid(vlnv);

    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        if (view->isHierarchical())
        {
            VLNV designVLNV = findDesignReference(view);
            createChildItemForDesign(designVLNV, KactusAttribute::HW, view->name());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::findDesignReference()
//-----------------------------------------------------------------------------
VLNV HierarchyItem::findDesignReference(QSharedPointer<View> view)
{
    if (!view->getDesignInstantiationRef().isEmpty())
    {
        QString viewDesign = view->getDesignInstantiationRef();

        foreach (QSharedPointer<DesignInstantiation> instantiation, *component_->getDesignInstantiations())
        {
            if (instantiation->name() == viewDesign)
            {
                return *instantiation->getDesignReference();
            }
        }
    }
    else
    {
        QString viewConfiguration = view->getDesignConfigurationInstantiationRef();

        foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation, 
            *component_->getDesignConfigurationInstantiations())
        {
            if (instantiation->name() == viewConfiguration)
            {
                VLNV configurationVLNV = *instantiation->getDesignConfigurationReference();

                if (!library_->contains(configurationVLNV))
                {
                    isValid_ = false;
                    return configurationVLNV;
                }
                else if (library_->getDocumentType(configurationVLNV) != VLNV::DESIGNCONFIGURATION)
                {
                    emit errorMessage(tr("Design Configuration reference %1 was for wrong type of object.").arg(
                        configurationVLNV.toString()));
                    isValid_ = false;
                    return VLNV();
                }
                else
                {
                    QSharedPointer<Document const> document = library_->getModelReadOnly(configurationVLNV);
                    return document.dynamicCast<DesignConfiguration const>()->getDesignRef();
                }
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::createChildItemForDesign()
//-----------------------------------------------------------------------------
void HierarchyItem::createChildItemForDesign(VLNV const& designVLNV, 
    KactusAttribute::Implementation implementation, QString const& viewName)
{
    if (!designVLNV.isEmpty())
    {
        if (!library_->contains(designVLNV))
        {
            emit errorMessage(tr("VLNV %1 was not found in the library").arg(designVLNV.toString()));
            isValid_ = false;
        }                
        else if (library_->getDocumentType(designVLNV) != VLNV::DESIGN)
        {
            emit errorMessage(tr("VLNV %1 was not valid hierarchy reference.").arg(designVLNV.toString()));
            isValid_ = false;
        }
        else if (!hasChild(designVLNV)) 
        {
            HierarchyItem* designItem = new HierarchyItem(library_, this, designVLNV, implementation, viewName);

            connect(designItem, SIGNAL(errorMessage(QString const&)),
                this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
            connect(designItem, SIGNAL(noticeMessage(QString const&)),
                this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

            childItems_.append(designItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseBusDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseBusDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::BUSDEFINITION;
    busDef_ = library_->getModelReadOnly(vlnv).staticCast<BusDefinition const>();

    isValid_ = library_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseAbsDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseAbsDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::ABSDEFINITION;
    absDef_ = library_->getModelReadOnly(vlnv).staticCast<AbstractionDefinition const>();

    isValid_ = library_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseComDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseComDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::COMDEFINITION;
    comDef_ = library_->getModelReadOnly(vlnv).staticCast<ComDefinition const>();

    isValid_ = library_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseApiDefinition()
//-----------------------------------------------------------------------------
void HierarchyItem::parseApiDefinition(VLNV const& vlnv)
{
    type_ = HierarchyItem::APIDEFINITION;
    apiDef_ = library_->getModelReadOnly(vlnv).staticCast<ApiDefinition const>();

    isValid_ = library_->isValid(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::parseDesign()
//-----------------------------------------------------------------------------
void HierarchyItem::parseDesign(VLNV const& vlnv, KactusAttribute::Implementation implementation,
    QString const& viewName)
{
    instanceCount_.clear();

    viewName_ = viewName;
    design_ = library_->getModelReadOnly(vlnv).staticCast<Design const>();
    isValid_ = library_->isValid(vlnv);

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

    // Take all valid components referenced by the design.
    QVector<VLNV> componentReferences = getValidComponentsInDesign();

    foreach (VLNV const& componentVLNV, componentReferences)
    {
        // If item already has a child with given VLNV then don't create duplicate.
        if (!hasChild(componentVLNV))
        {
            HierarchyItem* item = new HierarchyItem(library_, this, componentVLNV);

            connect(item, SIGNAL(errorMessage(QString const&)),
                this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
            connect(item, SIGNAL(noticeMessage(QString const&)),
                this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

            childItems_.append(item);
            instanceCount_[componentVLNV] = componentReferences.count(componentVLNV);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::getValidComponentsInDesign()
//-----------------------------------------------------------------------------
QVector<VLNV> HierarchyItem::getValidComponentsInDesign()
{
    QVector<VLNV> componentReferences;

    foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        if (!instance->isDraft())
        {
            VLNV componentVLNV = *instance->getComponentRef();

            if (isValidComponentInstanceVLNV(componentVLNV))
            {
                componentReferences.append(componentVLNV);
            }
            else
            {
                // Mark this object as invalid because not all items were valid.
                isValid_ = false;
            }
        }
    }

    foreach(QSharedPointer<SWInstance> instance, design_->getSWInstances())
    {
        if (!instance->isDraft())
        {
            VLNV componentVLNV = *instance->getComponentRef();

            if (isValidComponentInstanceVLNV(componentVLNV))
            {
                componentReferences.append(componentVLNV);
            }
            else 
            {
                // Mark this object as invalid because not all items were valid.
                isValid_ = false;
            }
        }
    }

    return componentReferences;
}

//-----------------------------------------------------------------------------
// Function: HierarchyItem::isValidComponentInstanceVLNV()
//-----------------------------------------------------------------------------
bool HierarchyItem::isValidComponentInstanceVLNV(VLNV const& componentVLNV)
{
    if (!library_->contains(componentVLNV))
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(componentVLNV.toString()));
        return false;
    }

    else if (library_->getDocumentType(componentVLNV) != VLNV::COMPONENT)
    {
        emit errorMessage(tr("VLNV %1 was not for component").arg(componentVLNV.toString()));
        return false;
    }

    else if (getVLNV() == componentVLNV || hasParent(componentVLNV))
    {
        emit errorMessage(tr("Cyclic instantiation for vlnv %1 was found").arg(componentVLNV.toString()));
        return false;
    }

    return true;
}
