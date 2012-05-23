//-----------------------------------------------------------------------------
// File: VLNVDataNode.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.1.2012
//
// Description:
// Optimized VLNV data tree for VLNV editor data searches.
//-----------------------------------------------------------------------------

#ifndef VLNVDATATREE_H
#define VLNVDATATREE_H

#include <common/KactusAttribute.h>
#include <LibraryManager/vlnv.h>

#include <QString>
#include <QSharedPointer>

class LibraryInterface;
class LibraryItem;

//-----------------------------------------------------------------------------
//! VLNVDataNode class.
//-----------------------------------------------------------------------------
class VLNVDataNode
{
public:
    // Valid level values.
    enum
    {
        LEVEL_ROOT = 0, //!< Reserved for the tree root node.
        LEVEL_VENDOR,   //!< Node represents a Vendor field.
        LEVEL_LIBRARY,  //!< Node represents a Library field.
        LEVEL_NAME,     //!< Node represents a Name field.
        LEVEL_VERSION   //!< Node represents a Version field.
    };

    /*!
     *  Constructor.
     */
    VLNVDataNode(QString const& name = "", unsigned int level = LEVEL_ROOT);

    /*!
     *  Destructor.
     */
    ~VLNVDataNode();

    /*!
     *  Returns the name of the node.
     */
    QString const& getName() const;

    /*!
     *  Returns the VLNV of the node (valid only if the node level is LEVEL_VERSION).
     */
    VLNV const& getVLNV() const;

    /*!
     *  Returns the node level.
     */
    unsigned int getLevel() const;

    /*!
     *  Returns the child with the given name or null if not found.
     *
     *      @param [in] name The name of the child to search for.
     */
    VLNVDataNode* findChild(QString const& name);

    /*!
     *  Returns the child with the given name or null if not found.
     *
     *      @param [in] name The name of the child to search for.
     */
    VLNVDataNode const* findChild(QString const& name) const;

    /*!
     *  Returns the node's children.
     */
    QList< QSharedPointer<VLNVDataNode> > const& getChildren() const;

    /*!
     *  Sets the VLNV for the node.
     *
     *      @param [in] vlnv The vlnv to set.
     */
    void setVLNV(VLNV const& vlnv);

    /*!
     *  Adds a child to the node.
     *
     *      @param [in] name The name of the child.
     *
     *      @return The created child node.
     */
    VLNVDataNode* addChild(QString const& name);

    /*!
     *  Removes the child with the given name.
     *
     *      @param [in] name The name of the child.
     */
    void removeChild(QString const& name);

    /*!
     *  Removes all child nodes.
     */
    void removeChildren();
    

private:
    // Disable copying.
    VLNVDataNode(VLNVDataNode const& rhs);
    VLNVDataNode& operator=(VLNVDataNode const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The node name.
    QString name_;

    //! The node level.
    unsigned int level_;

    //! The object VLNV (only valid at the leaf nodes).
    VLNV vlnv_;

    //! The list of children.
    QList< QSharedPointer<VLNVDataNode> > children_;
};

//-----------------------------------------------------------------------------
//! VLNVDataTree class.
//-----------------------------------------------------------------------------
class VLNVDataTree : public VLNVDataNode
{
public:
    /*!
     *  Constructor which creates an empty tree.
     */
    VLNVDataTree();

    /*!
     *  Destructor.
     */
    ~VLNVDataTree();

    /*!
     *  Updates the tree based on the filters and the given library interface.
     *
     *      @param [in] lh    The library interface.
     *      @param [in] type  The allowed document type.
     */
    void parse(LibraryInterface* lh, VLNV::IPXactType type);

    /*!
     *  Clears the tree so that it contains only the root node.
     */
    void clear();

    /*!
     *  Sets the firmness filter on/off.
     *
     *      @param [in] on        If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] firmness  The allowed firmness.
     */
    void setFirmnessFilter(bool on, KactusAttribute::Firmness firmness = KactusAttribute::KTS_TEMPLATE);

    /*!
     *  Sets the product hierarchy filter on/off.
     *
     *      @param [in] on           If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] productHier  The allowed product hierarchy.
     */
    void setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier = KactusAttribute::KTS_IP);

    /*!
     *  Sets the implementation filter on/off.
     *
     *      @param [in] on              If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] implementation  The allowed implementation.
     */
    void setImplementationFilter(bool on, KactusAttribute::Implementation implementation = KactusAttribute::KTS_HW);

private:
    // Disable copying.
    VLNVDataTree(VLNVDataTree const& rhs);
    VLNVDataTree& operator=(VLNVDataTree const& rhs);

    /*!
     *  Parses the library subtree indicated by the given library item and adds matching names
     *  to the given data node.
     *
     *      @param [in] libItem  The library item containing the subtree.
     *      @param [in] node     The target data node.
     */
    void parseSubtree(LibraryInterface* lh, LibraryItem const* libItem, VLNVDataNode& node);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Firmness filter data.
    bool firmnessFilterEnabled_;
    KactusAttribute::Firmness firmnessFilter_;

    //! Product hierarchy filter data.
    bool hierarchyFilterEnabled_;
    KactusAttribute::ProductHierarchy hierarchyFilter_;

    //! Implementation filter data.
    bool implementationFilterEnabled_;
    KactusAttribute::Implementation implementationFilter_;
};

//-----------------------------------------------------------------------------

#endif // VLNVDATATREE_H
