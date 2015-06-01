//-----------------------------------------------------------------------------
// File: HierarchicalSaveBuildStrategy.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.05.2015
//
// Description:
// Builds a tree model for saving documents hierarchically.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALSAVEBUILDSTRATEGY_H
#define HIERARCHICALSAVEBUILDSTRATEGY_H

#include <QTreeWidgetItem>

#include <QObject>

class LibraryInterface;

class Component;
class Design;
class LibraryComponent;
class LibraryInterface;
class LibrarySelectorWidget;
class VLNV;

//-----------------------------------------------------------------------------
//! Builds a tree model for saving documents hierarchically.
//-----------------------------------------------------------------------------
class HierarchicalSaveBuildStrategy : public QObject
{
    Q_OBJECT
public:

    enum SaveMode
    {
        CURRENT_DIRECTORY = 0,
        COMMON_ROOT_DIRECTORY,
        SINGLE_DIRECTORY
    };

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library   The document library in use.
	 */
	HierarchicalSaveBuildStrategy(LibraryInterface* library, QObject* parent = 0);

	//! The destructor.
	~HierarchicalSaveBuildStrategy();

    /*!
     *  Creates a tree model from the given object tree.
     *
     *      @param [in] rootObject   The root object.
     *
     *      @return Root item of the document tree.
     */
    QTreeWidgetItem* build(QObject* rootObject) const;

    /*!
     *  Saves the documents in the tree and updates all references.
     *
     *      @param [in] item   The root item to start the save from.
     */
    void saveItem(QTreeWidgetItem* item) const;

    /*!
     *  Sets the saving mode specifying the save location.
     *
     *      @param [in] mode   The selected mode.
     */
    void setSaveMode(HierarchicalSaveBuildStrategy::SaveMode mode);

    /*!
     *  Sets the save path.
     *
     *      @param [in] path   The path to set.
     */
    void setSavePath(QString const& path);

signals:

    //! Emitted when an item has been saved in the library.
    void itemSaved() const;

private:

	// Disable copying.
	HierarchicalSaveBuildStrategy(HierarchicalSaveBuildStrategy const& rhs);
	HierarchicalSaveBuildStrategy& operator=(HierarchicalSaveBuildStrategy const& rhs);
   
    /*!
     *  Creates an item and its children.
     *
     *      @param [in] rootObject   The object in the document tree to represent in the item.
     *
     *      @return The created tree item.
     */
    QTreeWidgetItem* createItemAndItsChildren(QObject* rootObject) const;

    /*!
     *  Creates an item.
     *
     *      @param [in] object   The object in the document tree to represent in the item.
     *
     *      @return The created tree item.
     */
    QTreeWidgetItem* createItem(QObject* object) const;

    /*!
     *  Sets an item to be invalid.
     *
     *      @param [in] item   The item to set invalid.
     */
    void setInvalid(QTreeWidgetItem* item) const;

    /*!
     *  Sets the appropriate icon for the item.
     *
     *      @param [in] object      The object in the document tree representing the item.
     *      @param [in] item        The item whose icon to set.
     */
    void setIcon(QObject* object, QTreeWidgetItem* item) const;

    /*!
     *  Generates an unused VLVN.
     *
     *      @param [in] previousVLNV   The base VLNV for generation.
     *
     *      @return The generated unique VLNV.
     */
    QString generateUnusedVLNV(QString const& previousVLNV) const;

    /*!
     *  Creates child items for an item.
     *
     *      @param [in] object       The object in the document tree representing the item.
     *      @param [in] treeItem     The item whose children to create.
     */
    void createChildItems(QObject* object, QTreeWidgetItem* treeItem) const;

    /*!
     *  Checks if an item has no children with the given VLNV.
     *
     *      @param [in] item   The item to check.
     *      @param [in] vlnv   The VLNV to search for.
     *
     *      @return True, if no child has the given VLNV, otherwise false.
     */
    bool hasNoChildWithVLNV(QTreeWidgetItem* item, QString const& vlnv) const;

    /*!
     *  Checks if the given item should be saved.
     *
     *      @param [in] item   The item to check.
     *
     *      @return True, if the item is to be saved, otherwise false.
     */
    bool shouldSaveItem(QTreeWidgetItem* item) const;

    /*!
     *  Updates all references from the given document to the given child item.
     *
     *      @param [in] model       The document model to update.
     *      @param [in] childItem   The child item to update the references to.
     */
    void updateReferencesFromTo(QSharedPointer<LibraryComponent> model, QTreeWidgetItem* childItem) const;

    /*!
     *  Updates all references in a component to another.
     *
     *      @param [in] component           The component whose references to update.
     *      @param [in] reference           The reference to update.
     *      @param [in] updatedReference    The new reference.
     */
    void updateComponentReferences(QSharedPointer<Component> component, VLNV const& reference, VLNV const& updatedReference) const;

    /*!
     *  Updates all references in a design to another.
     *
     *      @param [in] design              The design whose references to update.
     *      @param [in] childVLNV           The reference to update.
     *      @param [in] newChildVLNV        The new reference.
     */
    void updateDesignReferences(QSharedPointer<Design> design, VLNV childVLNV, VLNV newChildVLNV) const;

    /*!
     *  Saves a document model to the library.
     *
     *      @param [in] previousReference   The earlier VLNV of the document model.
     *      @param [in] model               The document model to save.
     */
    void saveToLibrary(VLNV const& previousReference, QSharedPointer<LibraryComponent> model) const;
 
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used document library.
    LibraryInterface* library_;

    //! The selected save mode.
    SaveMode saveMode_;

    //! The selected path to save to.
    QString savePath_;
};

#endif // HIERARCHICALSAVEBUILDSTRATEGY_H
