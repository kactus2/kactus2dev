//-----------------------------------------------------------------------------
// File: VLNVDataTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 5.1.2012
//
// Description:
// Optimized VLNV data tree for VLNV editor data searches.
//-----------------------------------------------------------------------------

#include "VLNVDataTree.h"

#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include//LibraryItem.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: VLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode::VLNVDataNode(QString const& name)
    : name_(name),
      vlnv_()
{
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString const& VLNVDataNode::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: getVLNV()
//-----------------------------------------------------------------------------
VLNV const& VLNVDataNode::getVLNV() const
{
    return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: findChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::findChild(QString const& name) const
{
    for (QSharedPointer<VLNVDataNode> node : children_)
    {
        if (node->name() == name)
        {
            return node.data();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: getChildren()
//-----------------------------------------------------------------------------
QList< QSharedPointer<VLNVDataNode> > const& VLNVDataNode::getChildren() const
{
    return children_;
}

//-----------------------------------------------------------------------------
// Function: VLNVDataNode::getChildren()
//-----------------------------------------------------------------------------
int VLNVDataNode::getChildCount() const
{
    return children_.count();
}

//-----------------------------------------------------------------------------
// Function: addChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::addChild(QString const& name)
{
    QSharedPointer<VLNVDataNode> child(new VLNVDataNode(name));
    children_.append(child);
    return child.data();
}

//-----------------------------------------------------------------------------
// Function: removeChild()
//-----------------------------------------------------------------------------
void VLNVDataNode::removeChild(QString const& name)
{
    for (QSharedPointer<VLNVDataNode> node : children_)
    {
        if (node->name() == name)
        {
            children_.removeOne(node);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: removeChildren()
//-----------------------------------------------------------------------------
void VLNVDataNode::removeChildren()
{
    children_.clear();
}

//-----------------------------------------------------------------------------
// Function: setVLNV()
//-----------------------------------------------------------------------------
void VLNVDataNode::setVLNV(VLNV const& vlnv)
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: VLNVDataTree()
//-----------------------------------------------------------------------------
VLNVDataTree::VLNVDataTree(): VLNVDataNode(),
    firmnessFilterEnabled_(false),
    firmnessFilter_(KactusAttribute::TEMPLATE),
    hierarchyFilterEnabled_(false),
    hierarchyFilter_(KactusAttribute::IP),
    implementationFilterEnabled_(false),
    implementationFilter_(KactusAttribute::HW),
    extensions_()
{
}

//-----------------------------------------------------------------------------
// Function: parse()
//-----------------------------------------------------------------------------
void VLNVDataTree::parse(LibraryInterface* lh, QList<VLNV::IPXactType> const& types)
{
    // Parse all library objects.
    parseSubtree(lh, lh->getTreeRoot(), *this, types);
}

//-----------------------------------------------------------------------------
// Function: clear()
//-----------------------------------------------------------------------------
void VLNVDataTree::clear()
{
    removeChildren();
}

//-----------------------------------------------------------------------------
// Function: setFirmnessFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setFirmnessFilter(bool on, KactusAttribute::Firmness firmness)
{
    firmnessFilterEnabled_ = on;
    firmnessFilter_ = firmness;
}

//-----------------------------------------------------------------------------
// Function: setHierarchyFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier)
{
    hierarchyFilterEnabled_ = on;
    hierarchyFilter_ = productHier;
}

//-----------------------------------------------------------------------------
// Function: setImplementationFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setImplementationFilter(bool on, KactusAttribute::Implementation implementation)
{
    implementationFilterEnabled_ = on;
    implementationFilter_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: parseVendor()
//-----------------------------------------------------------------------------
void VLNVDataTree::parseSubtree(LibraryInterface* lh, LibraryItem const* libItem, VLNVDataNode& node,
                                QList<VLNV::IPXactType> const& types)
{
    const int childCount = libItem->getNumberOfChildren();
    for (int i = 0; i < childCount; ++i)
    {
        LibraryItem const* item = libItem->child(i);

        // Check if this is a leaf item.
        if (item->getLevel() == LibraryItem::Level::VERSION)
        {
            // Check if the tree already contains an node with the same name.
            // Check that the type is valid.
            if (node.findChild(item->name()) || !types.contains(item->getVLNV().getType()))
            {
                continue;
            }

            VLNV const vlnv = item->getVLNV();

            bool useFilters = (vlnv.getType() == VLNV::COMPONENT);
            KactusAttribute::Firmness firmness = KactusAttribute::KTS_REUSE_LEVEL_COUNT;
            KactusAttribute::ProductHierarchy hierarchy = KactusAttribute::KTS_PRODHIER_COUNT;
            KactusAttribute::Implementation implementation = KactusAttribute::KTS_IMPLEMENTATION_COUNT;

            if (useFilters)
            {
                // Retrieve the library component for filtering. Filtering is possible only if the
                // library component is an IP-XACT component.        
                QSharedPointer<Component const> component = lh->getModelReadOnly<Component>(vlnv);
                firmness = component->getFirmness();
                hierarchy = component->getHierarchy();
                implementation = component->getImplementation();
            }

            // If filtering is off, just accept the item.
            if (useFilters == false ||
                ((!firmnessFilterEnabled_ || firmnessFilter_ == firmness) &&
                (!hierarchyFilterEnabled_ || hierarchyFilter_ == hierarchy) &&
                (!implementationFilterEnabled_ || implementationFilter_ == implementation)))
            {
                VLNVDataNode* childNode = node.addChild(item->name());
                childNode->setVLNV(vlnv);
            }
        }
        else
        {
            QString name = item->name();

            if (item->getLevel() == LibraryItem::Level::NAME)
            {
                // Filter out extensions.
                for (QString const& ext : extensions_)
                {
                    if (name.endsWith(ext))
                    {
                        name = name.left(name.length() - ext.length());
                    }
                }
            }

            // Otherwise parse the subtree recursively.
            VLNVDataNode* childNode = node.findChild(name);

            if (childNode == 0)
            {
                childNode = node.addChild(name);
            }

            parseSubtree(lh, item, *childNode, types);

            if (childNode->getChildCount() == 0)
            {
                node.removeChild(name);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VLNVDataTree::addExtensionFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::addExtensionFilter(QString const& extension)
{
    extensions_.append(extension);
}
