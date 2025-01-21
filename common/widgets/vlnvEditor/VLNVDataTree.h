//-----------------------------------------------------------------------------
// File: VLNVDataNode.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 5.1.2012
//
// Description:
// Optimized VLNV data tree for VLNV editor data searches.
//-----------------------------------------------------------------------------

#ifndef VLNVDATATREE_H
#define VLNVDATATREE_H

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/Document.h>

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


    /*!
     *  Constructor.
     */
    VLNVDataNode(QString const& name = QString());

    /*!
     *  Destructor.
     */
    virtual ~VLNVDataNode() = default;
    
    // Disable copying.
    VLNVDataNode(VLNVDataNode const& rhs) = delete;
    VLNVDataNode& operator=(VLNVDataNode const& rhs) = delete;

    /*!
     *  Returns the name of the node.
     */
    QString const& name() const;

    /*!
     *  Returns the VLNV of the node (valid only if the node level is version).
     */
    VLNV const& getVLNV() const;

    /*!
     *  Returns the child with the given name or null if not found.
     *
     *    @param [in] name The name of the child to search for.
     */
    VLNVDataNode* findChild(QString const& name) const;

    /*!
     *  Returns the node's children.
     */
    QList< QSharedPointer<VLNVDataNode> > const& getChildren() const;

    
    /*!
     * Get the number of children the node has.
     *
     *    @return The number of children.
     */
     int getChildCount() const;

    /*!
     *  Sets the VLNV for the node.
     *
     *    @param [in] vlnv The vlnv to set.
     */
    void setVLNV(VLNV const& vlnv);

    /*!
     *  Adds a child to the node.
     *
     *    @param [in] name The name of the child.
     *
     *    @return The created child node.
     */
    VLNVDataNode* addChild(QString const& name);

    /*!
     *  Removes the child with the given name.
     *
     *    @param [in] name The name of the child.
     */
    void removeChild(QString const& name);

    /*!
     *  Removes all child nodes.
     */
    void removeChildren();
    

private:

    //! The node name.
    QString name_;

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
    virtual ~VLNVDataTree() = default;
    
    // Disable copying.
    VLNVDataTree(VLNVDataTree const& rhs) = delete;
    VLNVDataTree& operator=(VLNVDataTree const& rhs) = delete;

    /*! 
     *  Updates the tree based on the filters and the given library interface.
     *
     *    @param [in] lh     The library interface.
     *    @param [in] type   The allowed document types.
     */
    void parse(LibraryInterface* lh, QList<VLNV::IPXactType> const& types);

    /*!
     *  Clears the tree so that it contains only the root node.
     */
    void clear();

    /*!
     *  Sets the firmness filter on/off.
     *
     *    @param [in] on        If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] firmness  The allowed firmness.
     */
    void setFirmnessFilter(bool on, KactusAttribute::Firmness firmness = KactusAttribute::TEMPLATE);

    /*!
     *  Sets the product hierarchy filter on/off.
     *
     *    @param [in] on           If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] productHier  The allowed product hierarchy.
     */
    void setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier = KactusAttribute::IP);

    /*!
     *  Sets the implementation filter on/off.
     *
     *    @param [in] on              If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] implementation  The allowed implementation.
     */
    void setImplementationFilter(bool on, KactusAttribute::Implementation implementation = KactusAttribute::HW);

    /*!
     *  Sets the IP-XACT revision filter on/off.
     *
     *    @param [in] on              If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] revision        The allowed IP-XACT standard revision.
     */
    void setRevisionFilter(bool on, Document::Revision revision = Document::Revision::Std22);

    /*!
     *  Adds an extension to be filtered out from the content search results.
     *
     *    @param [in] extension The extension to filter out.
     */
    void addExtensionFilter(QString const& extension);

private:

    /*!
     *  Parses the library subtree indicated by the given library item and adds matching names
     *  to the given data node.
     *
     *    @param [in] libItem  The library item containing the subtree.
     *    @param [in] node     The target data node.
     */
    void parseSubtree(LibraryInterface* lh, LibraryItem const* libItem, VLNVDataNode& node,
                      QList<VLNV::IPXactType> const& types);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Firmness filter data.
    bool firmnessFilterEnabled_ = false;
    KactusAttribute::Firmness firmnessFilter_ = KactusAttribute::TEMPLATE;

    //! Product hierarchy filter data.
    bool hierarchyFilterEnabled_ = false;
    KactusAttribute::ProductHierarchy hierarchyFilter_ = KactusAttribute::IP;

    //! Implementation filter data.
    bool implementationFilterEnabled_ = false;
    KactusAttribute::Implementation implementationFilter_ = KactusAttribute::HW;

    //! IP-XACT standard revision filter data.
    bool revisionFilterEnabled_ = false;
    Document::Revision revisionFilter_ = Document::Revision::Unknown;

    //! The extensions to filter.
    QStringList extensions_;
};

//-----------------------------------------------------------------------------

#endif // VLNVDATATREE_H
