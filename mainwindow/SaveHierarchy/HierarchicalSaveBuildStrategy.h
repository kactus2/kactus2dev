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
class HierarchicalSaveBuildStrategy 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library   The document library in use.
	 */
	HierarchicalSaveBuildStrategy(LibraryInterface* library);

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

private:

	// Disable copying.
	HierarchicalSaveBuildStrategy(HierarchicalSaveBuildStrategy const& rhs);
	HierarchicalSaveBuildStrategy& operator=(HierarchicalSaveBuildStrategy const& rhs);
   
    QTreeWidgetItem* createComponentItem(QObject* rootObject) const;

    QTreeWidgetItem* createItem(QObject* object) const;

    void setInvalid(QTreeWidgetItem* item) const;

    QString generateUnusedVLNV(QString const& previousVLNV) const;

    void createChildItems(QObject* object, QTreeWidgetItem* treeItem) const;

    bool hasNoChildWithVLNV(QTreeWidgetItem* item, QString const& vlnv) const;

    bool shouldSaveItem(QTreeWidgetItem* item) const;

    void updateReferencesFromTo(QSharedPointer<LibraryComponent> model, QTreeWidgetItem* childItem) const;

    void updateComponentReferences(QSharedPointer<Component> component, VLNV const& reference, VLNV const& updatedReference) const;

    void updateDesignReferences(QSharedPointer<Design> design, VLNV childVLNV, VLNV newChildVLNV) const;

    void saveToLibrary(VLNV const& previousReference, QSharedPointer<LibraryComponent> model) const;

    LibraryInterface* library_;
};

#endif // HIERARCHICALSAVEBUILDSTRATEGY_H
