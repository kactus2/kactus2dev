/* 
 *
 *  Created on: 22.2.2011
 *      Author: Antti Kamppi
 * 		filename: libraryinterface.h
 */

#ifndef LIBRARYINTERFACE_H
#define LIBRARYINTERFACE_H

#include "vlnv.h"

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/design.h>

#include <QObject>
#include <QSharedPointer>

class LibraryItem;

/*! \brief LibraryInterface defines an interface to operate the IP-Xact-library.
 * 
 * All classes that use the IP-Xact library should do it through this interface.
 */
class LibraryInterface {

public:

	/*! \brief Get a model that matches given VLNV.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 * \param vlnv Identifies the desired document.
	 *
	 * \return Shared pointer to the model that matches the document.
	 * If vlnv is not found in library a null pointer is returned. The ownership 
	 * of the parsed object remains on Library Handler.
	*/
	virtual QSharedPointer<LibraryComponent> getModel(const VLNV& vlnv) = 0;

    /*! \brief Get a model that matches given VLNV for read-only access.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 * \param vlnv Identifies the desired document.
	 *
	 * \return Shared pointer to the model that matches the document.
	 * If vlnv is not found in library a null pointer is returned. The ownership 
	 * of the parsed object remains on Library Handler.
	*/
	virtual QSharedPointer<LibraryComponent const> getModelReadOnly(const VLNV& vlnv) = 0;

	/*! \brief Add a new VLNV tag to the library
	 *
	 * This function adds the specified VLNV into the library. The document
	 * is found at the specified path.
	 *
	 * \param vlnv A Pointer to the VLNV instance to be added
	 * \param path QString containing the filepath to the document to be added.
	 * The path has to be absolute path.
	 *
	 * \return True if add was succesful. False if file was not added i.e.
	 * if the VLNV was already in the library
	 */
	//virtual bool registerVLNV(const VLNV& vlnv, const QString& path) = 0;

	/*! \brief Checks if the library already contains the specified vlnv.
	 *
	 * \param vlnv The vlnv that is searched within the library.
	 *
	 * \return True if the vlnv was found
	*/
	virtual bool contains(const VLNV& vlnv) = 0;

	/*! \brief Get a path to the specified IP-Xact document.
	 *
	 * \param vlnv Specifies the wanted IP-Xact document.
	 *
	 * \return const QString containing the path to the document. If vlnv is
	 * not found then null string is returned.
	*/
	virtual const QString getPath(const VLNV& vlnv) const = 0;

	/*! \brief Get the directory path to the specified IP-XACT document.
	 *
	 * Method: 		getDirectoryPath
	 * Full name:	LibraryInterface::getDirectoryPath
	 * Access:		virtual public 
	 *
	 * \param vlnv Specifies the wanted IP-XACT document.
	 *
	 * \return QString containing the directory path to the document. Does not contain the xml file name.
	*/
	virtual QString getDirectoryPath(const VLNV& vlnv) const = 0;

	/*! \brief Write the model to file system to given file path
     *
     * This function takes the given directory path and writes the given model to
     * that directory. The path must not contain the file name.
     * If file already exists in file system it is written over.
     * 
     * When model is saved using this function it is not necessary to call 
     * onItemSaved() because it is called by this function.
     *
     * \param path Directory path that specifies the directory to save to file into.
     * \param model Pointer to the library component that is written.
     * \param printErrors If true then the errors detected in the model are printed to user.
     * 
     * \return True if the model was in valid state and was successfully written.
     */
    virtual bool writeModelToFile(const QString path, 
		QSharedPointer<LibraryComponent> model,
		bool printErrors = true) = 0;

	/*! \brief Write the already registered model to file system.
	 *
	 * This function demands that the model is already part of the library and
	 * it's file path can be found. The changes made to the model are written
	 * to the previous file so the model can not be restored to it's previous 
	 * state after this operation.
	 * 
	 * \param model Pointer to the model to be written.
	 * \param printErrors If true then the errors detected in the model are printed to user.
	 * 
	 * \return True if the model was in valid state and was successfully written.
	*/
	virtual bool writeModelToFile(QSharedPointer<LibraryComponent> model,
		bool printErrors = true) = 0;

	/*! \brief Search for IP-Xact files in the file system and add them to library
	 * 
	 * Function asks user to select a file path to start the recursive search
	 * within subdirectories.
	*/
	virtual void searchForIPXactFiles() = 0;

	/*! \brief Get list of vlnvs that are needed by given document.
	*
	* This function takes an IP-Xact document and searches it and returns list
	* of all vlnvs that are needed by that document. Function also searches
	* all possible sub vlnvs that need other vlnvs and adds them to the list.
	* The list doesn't contain single vlnv twice.
	*
	* \param vlnv Reference to the vlnv that is used as starting point for the
	* search.
	* \param list Reference to the list where all vlnvs are added to.
	*
	* \return List containing all vlnvs that are referenced in any of the
	* documents.
	*/
	virtual void getNeededVLNVs(const VLNV& vlnv, QList<VLNV>& list) = 0;

	/*! \brief Get list of files that are needed by the given document.
	*
	* This function returns a list of file paths that are converted as
	* absolute file paths to the needed files. This function searches files
	* only from this document, it does not search possible subcomponents.
	*
	* \param vlnv Reference to the vlnv that is used for the search.
	* \param list The files are appended to the list if they are not
	* already on the list.
	*/
	virtual void getDependencyFiles(const VLNV& vlnv, QStringList& list) = 0;

	/*! \brief Get list of the files that are needed by the given document either directly or indirectly.
	  *
	  * \param vlnv The vlnv identifying the document.
	  * \param list The files are appended to the list if they are not already on the list.
	  *
	 */
	 virtual void getHierarchicalDependencyFiles(const VLNV& vlnv, QStringList& list) = 0;

	/*! \brief Get const-pointer to the library tree's root item
	*
	* \return LibraryItem const* Pointer to the root item.
	*/
	virtual LibraryItem const* getTreeRoot() const = 0;

	/*! \brief Get the document type of given vlnv.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 * \param vlnv Specifies the document that's type is wanted.
	 *
	 * \return VLNV::IPXactType Type of the document.
	*/
	virtual VLNV::IPXactType getDocumentType(const VLNV& vlnv) = 0;

	/*! \brief Count how many times the given component is instantiated in the library.
	 *
	 * \param vlnv Identifies the component that's instances are searched.
	 *
	 * \return Number of found instances.
	 */
	virtual int referenceCount(const VLNV& vlnv) const = 0;

	/*! \brief Get the components that have instantiated the given vlnv in their design.
	 *
	 * \param list QList where the search results are appended.
	 * \param vlnvToSearch Identifies the component to search for.
	 * 
	 * \return Number of owners found.
	 *
	*/
	virtual int getOwners(QList<VLNV>& list, const VLNV& vlnvToSearch) const = 0;

	/*! \brief Get the items that are needed by the specified item.
	 *
	 * \param list QList where the search results are appended.
	 * \param vlnvToSearch Identifies the item thats child-items are wanted.
	 *
	 * \return int The number of found children.
	*/
	virtual int getChildren(QList<VLNV>& list, const VLNV& vlnvToSearch) const = 0;

	/*! \brief Get the VLNV of the design for a given hierarchy reference.
	 *
	 * This function can be used by calling it with a hierarchy reference found
	 * in a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design vlnv. If reference is for a
	 * configuration then the design vlnv is searched from the configuration and
	 * design vlnv is returned. If design is not found then invalid vlnv is 
	 * returned.
	 * 
	 * \param hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 * \return VLNV The vlnv identifying the design object.
	*/
	virtual VLNV getDesignVLNV(const VLNV& hierarchyRef) = 0;

	/*! \brief Get pointer to the design for a given hierarchy reference.
	 * 
	 * * This function can be used by calling it with a hierarchy reference found
	 * if a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design pointer. If reference is for a
	 * configuration then the design vlnv is searched from the configuration and
	 * design pointer is returned. If design is not found then null pointer is 
	 * returned.
	 * 
	 * \param hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 * \return QSharedPointer<Design> Pointer to the design.
	*/
	virtual QSharedPointer<Design> getDesign(const VLNV& hierarchyRef) = 0;

	/*! \brief Check if the identified object is in valid state.
	 *
	 * \param vlnv Identifies the object to check.
	 *
	 * \return bool True if the object was valid. False if invalid or object was
	 * not found in library.
	*/
	virtual bool isValid(const VLNV& vlnv) = 0;

public Q_SLOTS:

	/*! \brief Check the library items for validity
	 * 
	 * This function automatically removes the invalid library items.
	 *
	*/
	virtual void onCheckLibraryIntegrity() = 0;

	/*! \brief Selects the given vlnv item in the library
	 *
	 * \param vlnv Specifies the item in the library to select
	 *
	*/
	virtual void onSelectionChanged(const VLNV& vlnv) = 0;

	/*! \brief Clear the item selection
	 *
	*/
	virtual void onClearSelection() = 0;


	/*! \brief Edit an item in the library
	 *
	 * \param vlnv Reference to the vlnv that identifies the object to edit.
	 *
	*/
	virtual void onEditItem(const VLNV& vlnv) = 0;

	/*! \brief Open the specified component design
	 *
	 * \param vlnv Identifies the component that's design is wanted
	 *
	*/
	virtual void onOpenDesign(const VLNV& vlnv) = 0;

	/*! \brief Create a new item for given vlnv.
	 * 
	 * The type of the vlnv defines the type of the object to create.
	 * After creating the object an editor for it is opened.
	 *
	 * \param vlnv Identifies the object to create
	 *
	*/
	virtual void onCreateNewItem(const VLNV& vlnv) = 0;

	/*! \brief Create new design with given vlnv.
	 *
	 * \param vlnv The vlnv that identifies the design.
	 *
	*/
	virtual void onCreateDesign(const VLNV& vlnv) = 0;

	/*! \brief Remove the specified library object from the library and file system.
	 * 
	 * This function does not ask user to verify the deletion, it just removes the object and
	 * it's xml file. If vlnv is for component the component's files are not removed.
	 * 
	 * \param vlnv Identifies the object.
	 *
	*/
	virtual void removeObject(const VLNV& vlnv) = 0;

	/*! \brief Remove the specified library objects from the library and file system.
	 * 
	 * This function opens a dialog that asks user to confirm the deletion of the objects.
	 * 
	 * \param vlnvList Identifies the objects to remove.
	 *
	*/
	virtual void removeObjects(const QList<VLNV>& vlnvList) = 0;

	/*! \brief Call this function before saving several objects to library.
	*
	* When calling this before saving and then using writeModelToFile() the 
	* library is not update between each save operation thus making the program
	* faster.
	* 
	* Be sure to call endSave() after all items are saved.
	*/
	virtual void beginSave() = 0;

	/*! \brief End the saving operation and update the library.
	*
	* This function must be called always after calling the beginSave().
	*/
	virtual void endSave() = 0;

Q_SIGNALS:

	//! \brief Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! \brief Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! \brief Signal that user wants to open the specified component for editing.
	void openDesign(const VLNV& vlnv, const QString& viewName);

	//! \brief Signal that user wants to open specified bus for editing
	void openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV, bool disableBusDef);

	//! \brief Signal that user wants to open specified component for editing
	void openComponent(const VLNV& vlnv);

	//! \brief Signal that user wants to create a new bus with given vlnv
	void createBus(const VLNV& vlnv, const QString& directory);

	//! \brief Signal that user wants to create a new component with given vlnv
	void createComponent(const VLNV& vlnv, const QString& directory);

	//! \brief Signal that user wants to create a new abstraction definition for given bus definition.
	void createAbsDef(const VLNV& busDefVLNV, const QString& directory, bool disableBusDef);

	//! \brief Signal that library has changed and VLNVDialer should be refreshed.
	void refreshDialer();

	//! \brief Signal that the library item specified by vlnv is selected in one of the views.
	void itemSelected(const VLNV& vlnv);

	//! \brief Signal that user wants to open a system for editing.
	void openSystemDesign(const VLNV& vlnv);

	//! \brief Signal that user wants to open a platform component.
	void openPlatformComponent(const VLNV& vlnv);

private:
};

#endif // LIBRARYINTERFACE_H