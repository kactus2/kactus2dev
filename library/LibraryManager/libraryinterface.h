//-----------------------------------------------------------------------------
// File: libraryinterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.02.2011
//
// Description:
// LibraryInterface defines an interface to operate the IP-XACT-library.
//-----------------------------------------------------------------------------

#ifndef LIBRARYINTERFACE_H
#define LIBRARYINTERFACE_H

#include <IPXACTmodels/common/VLNV.h>

#include <QObject>
#include <QSharedPointer>

class Design;
class LibraryItem;
class Document;

//-----------------------------------------------------------------------------
//! LibraryInterface defines an interface to operate the IP-XACT-library.
//-----------------------------------------------------------------------------
class LibraryInterface
{

public:

	/*! Get a model that matches given VLNV. Must be used when changes are made to the document.
	 *  The provided document is a copy, meaning that it must be explicitly saved after the changes.
	 * 
	 *      @param [in] vlnv    Identifies the desired document.
	 *
	 *      @return The model that matches the document. The ownership of the object remains on Library Handler.
	*/
	virtual QSharedPointer<Document> getModel(VLNV const& vlnv) = 0;

    /*! Get a model that matches given VLNV for read-only access. May be used when no changes are to be made
	 *  for the document.
	 * 
	 *      @param [in] vlnv    Identifies the desired document.
	 *
	 *      @return The model that matches the document. The ownership of the object remains on Library Handler.
	*/
	virtual QSharedPointer<Document const> getModelReadOnly(VLNV const& vlnv) = 0;

	/*! Checks if the library already contains the specified VLNV.
	 *
	 *      @param [in] vlnv    The VLNV that is searched within the library.
	 *
	 *      @return True if the vlnv was found, otherwise false.
	*/
	virtual bool contains(VLNV const& vlnv) = 0;

	/*! Get a path to the specified IP-XACT document.
	 *
	 *      @param [in] vlnv    Specifies the wanted IP-XACT document.
	 *
	 *      @return The path to the document. If VLNV is not found then empty string is returned.
	*/
	virtual const QString getPath(VLNV const& vlnv) const = 0;

	/*! Get the directory path to the specified IP-XACT document.
	 *
	 *      @param [in] vlnv    Specifies the wanted IP-XACT document.
	 *
	 *      @return The directory path to the document. Does not contain the XML file name.
	*/
	virtual QString getDirectoryPath(VLNV const& vlnv) const = 0;

	/*! Write the model to file system to given file path.
     *
     *      @param [in] path            Directory path that specifies the directory to save to file into.
     *      @param [in] model           The library component that is written.
     * 
     *      @return True if the model was in valid state and was successfully written.
     *
     *      @remark The path must not contain the file name.
     *      @remark If file already exists in file system it is overwritten.
     *      @remark It is not necessary to call onItemSaved() because it is called by this function.
     */
    virtual bool writeModelToFile(QString const& path, QSharedPointer<Document> model) = 0;

	/*! Write the already registered model to file system.
	 * 
	 *      @param [in] modelThe        model to be written.
	 * 
	 *      @return True if the model was in valid state and was successfully written.
     *
     *      @remark The model must be already part of the library and it's file path can be found.
     *      @remark This operation cannot be undone.
	*/
	virtual bool writeModelToFile(QSharedPointer<Document> model) = 0;

	//! Search for IP-XACT files in the file system and add them to library.
	virtual void searchForIPXactFiles() = 0;

	/*! Get list of VLNVs that are needed by given document.
	*
	* This function takes an IP-XACT document and searches it and returns list
	* of all VLNVs that are needed by that document. Function also searches
	* all possible sub VLNVs that need other VLNVs and adds them to the list.
	* The list doesn't contain single VLNV twice.
	*
	*      @param [in] vlnv     The VLNV that is used as starting point for the search.
	*      @param [out] list    The list where all VLNVs are added to.
	*/
	virtual void getNeededVLNVs(VLNV const& vlnv, QList<VLNV>& list) = 0;

	/*! Get list of files that are needed by the given document.
	*
	* This function returns a list of file paths that are converted as
	* absolute file paths to the needed files. This function searches files
	* only from this document, it does not search possible subcomponents.
	*
	*      @param [in] vlnv     The VLNV that is used for the search.
	*      @param [out] list    The files are appended to the list if they are not already on the list.
	*/
	virtual void getDependencyFiles(VLNV const& vlnv, QStringList& list) = 0;

	/*! Get list of the files that are needed by the given document either directly or indirectly.
	  *
	  *      @param [in] vlnv   The VLNV identifying the document.
	  *      @param [out] list   The files are appended to the list if they are not already on the list.
	 */
	 virtual void getHierarchicalDependencyFiles(VLNV const& vlnv, QStringList& list) = 0;

	/*! 
	*      Returns const-pointer to the library tree's root item
	*/
	virtual LibraryItem const* getTreeRoot() const = 0;

	/*! Get the document type of given vlnv.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 *      @param [in] vlnv Specifies the document that's type is wanted.
	 *
	 *      @return The type of the document.
	*/
	virtual VLNV::IPXactType getDocumentType(VLNV const& vlnv) = 0;

	/*! Count how many times the given component is instantiated in the library.
	 *
	 *      @param [in] vlnv Identifies the component that's instances are searched.
	 *
	 *      @return Number of found instances.
	 */
	virtual int referenceCount(VLNV const& vlnv) const = 0;

	/*! Get the components that have instantiated the given vlnv in their design.
	 *
	 *      @param [out] list           The search results.
	 *      @param [in] vlnvToSearch    Identifies the component to search for.
	 * 
	 *      @return Number of owners found.
	*/
	virtual int getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const = 0;

	/*! Get the items that are needed by the specified item.
	 *
	 *      @param [out] list           The search results.
	 *      @param [in] vlnvToSearch    Identifies the item thats child-items are wanted.
	 *
	 *      @return int The number of found children.
	*/
	virtual int getChildren(QList<VLNV>& list, VLNV const& vlnvToSearch) const = 0;

	/*! Get the VLNV of the design for a given hierarchy reference.
	 *
	 * This function can be used by calling it with a hierarchy reference found
	 * in a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design VLNV. If reference is for a
	 * configuration then the design VLNV is searched from the configuration and
	 * design VLNV is returned. If design is not found then invalid VLNV is 
	 * returned.
	 * 
	 *      @param [in] hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 *      @return VLNV The VLNV identifying the design object.
	*/
	virtual VLNV getDesignVLNV(VLNV const& hierarchyRef) = 0;

	/*! Get the design for a given hierarchy reference.
	 * 
	 * This function can be used by calling it with a hierarchy reference found
	 * if a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design pointer. If reference is for a
	 * configuration then the design VLNV is searched from the configuration and
	 * design pointer is returned. If design is not found then null pointer is 
	 * returned.
	 * 
	 *      @param [in] hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 *      @return The design.
	*/
	virtual QSharedPointer<Design> getDesign(VLNV const& hierarchyRef) = 0;

	/*! Check if the identified object is in valid state.
	 *
	 *      @param [in] VLNV Identifies the object to check.
	 *
	 *      @return bool True if the object was valid. False if invalid or object was not found in library.
	*/
	virtual bool isValid(VLNV const& vlnv) = 0;

public slots:

	/*! Check the library items for validity
	 * 
	 *      @remark This function automatically removes the invalid library items.
	 *
	*/
	virtual void onCheckLibraryIntegrity() = 0;

	/*! Selects the given vlnv item in the library
	 *
	 *      @param [in] vlnv Specifies the item in the library to select
	*/
	virtual void onSelectionChanged(VLNV const& vlnv) = 0;

	//! Clear the item selection
	virtual void onClearSelection() = 0;

	/*! Edit an item in the library
	 *
	 *      @param [in] vlnv Reference to the vlnv that identifies the object to edit.
	*/
	virtual void onEditItem(VLNV const& vlnv) = 0;

	/*! Open the specified component design
	 *
	 *      @param [in] vlnv Identifies the component that's design is wanted
	*/
	virtual void onOpenDesign(VLNV const& vlnv) = 0;

	/*! Create a new item for given vlnv.
	 * 
	 * The type of the vlnv defines the type of the object to create.
	 * After creating the object an editor for it is opened.
	 *
	 *      @param [in] vlnv Identifies the object to create
	*/
	virtual void onCreateNewItem(VLNV const& vlnv) = 0;

	/*! Create new design with given vlnv.
	 *
	 *      @param [in] vlnv The vlnv that identifies the design.
	*/
	virtual void onCreateDesign(VLNV const& vlnv) = 0;

	/*! Remove the specified library object from the library and file system.
	 * 
	 * This function does not ask user to verify the deletion, it just removes the object and
	 * it's xml file. If vlnv is for component the component's files are not removed.
	 * 
	 *      @param [in] vlnv Identifies the object.
	*/
	virtual void removeObject(VLNV const& vlnv) = 0;

	/*! Remove the specified library objects from the library and file system.
	 * 
	 * This function opens a dialog that asks user to confirm the deletion of the objects.
	 * 
	 *      @param [in] vlnvList Identifies the objects to remove.
	*/
	virtual void removeObjects(const QList<VLNV>& vlnvList) = 0;

	/*! Call this function before saving several objects to library.
	*
	* When calling this before saving and then using writeModelToFile() the 
	* library is not update between each save operation thus making the program
	* faster.
	* 
	* Be sure to call endSave() after all items are saved.
	*/
	virtual void beginSave() = 0;

	/*! End the saving operation and update the library.
	*
	* This function must be called always after calling the beginSave().
	*/
	virtual void endSave() = 0;

signals:

	//! Emit an error message to be printed to user.
	void errorMessage(QString const& message);

	//! Emit a notice message to be printed to user.
	void noticeMessage(QString const& message);

	//! Signal that user wants to open the specified component for editing.
	void openDesign(VLNV const& vlnv, QString const& viewName);

	//! Signal that user wants to open specified bus for editing
	void openBus(VLNV const& busDefVLNV, VLNV const& absDefVLNV, bool disableBusDef);

	//! Signal that user wants to open specified component for editing
	void openComponent(VLNV const& vlnv);

	//! Signal that user wants to create a new bus with given vlnv
	void createBus(VLNV const& vlnv, QString const& directory);

	//! Signal that user wants to create a new component with given vlnv
	void createComponent(VLNV const& vlnv, QString const& directory);

	//! Signal that user wants to create a new abstraction definition for given bus definition.
	void createAbsDef(VLNV const& busDefVLNV, QString const& directory, bool disableBusDef);

	//! Signal that library has changed and VLNVDialer should be refreshed.
	void refreshDialer();

	//! Signal that the library item specified by vlnv is selected in one of the views.
	void itemSelected(VLNV const& vlnv);

	//! Signal that user wants to open a system for editing.
	void openSystemDesign(VLNV const& vlnv);

	//! Signal that user wants to open a platform component.
	void openPlatformComponent(VLNV const& vlnv);

};

#endif // LIBRARYINTERFACE_H