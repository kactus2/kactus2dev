//-----------------------------------------------------------------------------
// File: VLNVDataTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.1.2012
//
// Description:
// Optimized VLNV data tree for VLNV editor data searches.
//-----------------------------------------------------------------------------

#include "VLNVDataTree.h"

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/libraryitem.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: VLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode::VLNVDataNode(QString const& name, unsigned int level) : name_(name), level_(level), vlnv_()
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode::~VLNVDataNode()
{
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& VLNVDataNode::getName() const
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
// Function: getLevel()
//-----------------------------------------------------------------------------
unsigned int VLNVDataNode::getLevel() const
{
    return level_;
}

//-----------------------------------------------------------------------------
// Function: findChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::findChild(QString const& name)
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
        {
            return node.data();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: findChild()
//-----------------------------------------------------------------------------
VLNVDataNode const* VLNVDataNode::findChild(QString const& name) const
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
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
// Function: addChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::addChild(QString const& name)
{
    QSharedPointer<VLNVDataNode> child(new VLNVDataNode(name, level_ + 1));
    children_.append(child);
    return child.data();
}

//-----------------------------------------------------------------------------
// Function: removeChild()
//-----------------------------------------------------------------------------
void VLNVDataNode::removeChild(QString const& name)
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
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
VLNVDataTree::VLNVDataTree() : firmnessFilterEnabled_(false), firmnessFilter_(KactusAttribute::KTS_TEMPLATE),
                               hierarchyFilterEnabled_(false), hierarchyFilter_(KactusAttribute::KTS_IP),
                               implementationFilterEnabled_(false), implementationFilter_(KactusAttribute::KTS_HW)
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVDataTree()
//-----------------------------------------------------------------------------
VLNVDataTree::~VLNVDataTree()
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
void VLNVDataTree::setFirmnessFilter(bool on, KactusAttribute::Firmness firmness /*= KactusAttribute::KTS_TEMPLATE*/)
{
    firmnessFilterEnabled_ = on;
    firmnessFilter_ = firmness;
}

//-----------------------------------------------------------------------------
// Function: setHierarchyFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier /*= KactusAttribute::KTS_IP*/)
{
    hierarchyFilterEnabled_ = on;
    hierarchyFilter_ = productHier;
}

//-----------------------------------------------------------------------------
// Function: setImplementationFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setImplementationFilter(bool on, KactusAttribute::Implementation implementation /*= KactusAttribute::KTS_HW*/)
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
    for (int i = 0; i < libItem->getNumberOfChildren(); ++i)
    {
        LibraryItem const* item = libItem->child(i);

        // Check if this is a leaf item.
        if (item->getLevel() == LibraryItem::VERSION)
        {
            VLNV const vlnv = item->getVLNV();

            // Check if the tree already contains an node with the same name.
            if (node.findChild(item->getName()))
            {
                continue;
            }

            // Check that the type is valid.
            if (!types.contains(item->getVLNV().getType()))
            {
                continue;
            }

            // If filtering is off, just accept the item.
            if (!firmnessFilterEnabled_ && !hierarchyFilterEnabled_ && !implementationFilterEnabled_)
            {
                VLNVDataNode* childNode = node.addChild(item->getName());
                childNode->setVLNV(vlnv);
                continue;
            }

            // Retrieve the library component for filtering. Filtering is possible only if the
            // library component is an IP-XACT component.
            QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(vlnv);
            QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

            if (component == 0 ||
                ((!firmnessFilterEnabled_ || firmnessFilter_ == component->getComponentFirmness()) &&
                (!hierarchyFilterEnabled_ || hierarchyFilter_ == component->getComponentHierarchy()) &&
                (!implementationFilterEnabled_ || implementationFilter_ == component->getComponentImplementation())))
            {
                VLNVDataNode* childNode = node.addChild(item->getName());
                childNode->setVLNV(vlnv);
            }
        }
        else
        {
            QString name = item->getName();

            if (item->getLevel() == LibraryItem::NAME)
            {
                // TODO: This should be done conditionally.
                name = name.remove(".sysdesigncfg");
                name = name.remove(".sysdesign");
                name = name.remove(".swdesigncfg");
                name = name.remove(".swdesign");
                name = name.remove(".designcfg");
                name = name.remove(".design");
            }

            // Otherwise parse the subtree recursively.
            VLNVDataNode* childNode = node.findChild(name);

            if (childNode == 0)
            {
                childNode = node.addChild(name);
            }

            parseSubtree(lh, item, *childNode, types);

            if (childNode->getChildren().size() == 0)
            {
                node.removeChild(name);
            }
        }
    }
}