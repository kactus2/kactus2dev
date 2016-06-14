//-----------------------------------------------------------------------------
// File: libraryhandler.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryHandler is the class that implements the services to manage the IP-XACT library.
//-----------------------------------------------------------------------------

#ifndef LIBRARYHANDLER_H
#define LIBRARYHANDLER_H

#include "libraryinterface.h"
#include "librarytreewidget.h"
#include "librarydata.h"
#include "librarytreemodel.h"
#include "HierarchyView/hierarchymodel.h"
#include "HierarchyView/hierarchywidget.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QString>
#include <QTabWidget>
#include <QStringList>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QSharedPointer>
#include <QMap>
#include <QObject>

typedef QVector<QFileInfo> fileList;

class LibraryItem;
class Document;
class VLNVDialer;

//-----------------------------------------------------------------------------
//! LibraryHandler is the class that implements the services to manage the IP-XACT library.
//-----------------------------------------------------------------------------
class LibraryHandler : public QTabWidget, public LibraryInterface
{
    Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] dialer  The dialer that provides search options.
	 *      @param [in] parent  The parent widget of this instance.
	 */
    LibraryHandler(VLNVDialer* dialer, QWidget* parent);

	//! The destructor
    virtual ~LibraryHandler();

	/*! Get a model that matches given VLNV.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 *      @param [in] vlnv Identifies the desired document.
	 *
	 *      @return The model that matches the document.
	*/
	virtual QSharedPointer<Document> getModel(VLNV const& vlnv);

    /*! Get a model that matches given VLNV for read-only access.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 *      @param [in] vlnv Identifies the desired document.
	 *
	 *      @return The model that matches the document.
	*/
	virtual QSharedPointer<Document const> getModelReadOnly(VLNV const& vlnv);
	
	/*! Checks if the library already contains the specified vlnv.
	 *
	 *      @param [in] vlnv The vlnv that is searched within the library.
	 *
	 *      @return True if the vlnv was found
	*/
	virtual bool contains(VLNV const& vlnv);

	/*! Get a path to the specified IP-Xact document.
	 *
	 *      @param [in] vlnv Specifies the wanted IP-Xact document.
	 *
	 *      @return The path to the document. If vlnv is not found then empty string is returned.
	*/
	virtual const QString getPath(VLNV const& vlnv) const;

	/*! Get the directory path to the specified IP-XACT document.
	 *
	 *      @param [in] vlnv Specifies the wanted IP-XACT document.
	 *
	 *      @return The directory path to the document. Does not contain the xml file name.
	*/
	virtual QString getDirectoryPath(VLNV const& vlnv) const;

	/*! Write the model to file system to given file path
     *
     * This function takes the given directory path and writes the given model to
     * that directory. The path must not contain the file name.
     * If file already exists in file system it is written over.
     * 
     * When model is saved using this function it is not necessary to call 
     * onItemSaved() because it is called by this function.
     *
     *      @param [in] path            Directory path that specifies the directory to save to file into.
     *      @param [in] model           The library component that is written.
     * 
     *      @return True if the model was in valid state and was successfully written.
     */
    virtual bool writeModelToFile(QString const& path, QSharedPointer<Document> model);

	/*! Write the already registered model to file system.
	 *
	 * This function demands that the model is already part of the library and
	 * it's file path can be found. The changes made to the model are written
	 * to the previous file so the model can not be restored to it's previous 
	 * state after this operation.
	 * 
	 *      @param [in] model           The model to be written.
	 * 
	 *      @return True if the model was in valid state and was successfully written.
	*/
	virtual bool writeModelToFile(QSharedPointer<Document> model);

	//! Search for IP-Xact files in the file system and add them to library
	virtual void searchForIPXactFiles();
    
	/*! Get list of vlnvs that are needed by given document.
	 *
	 * This function takes an IP-Xact document and searches it and returns list
	 * of all vlnvs that are needed by that document. Function also searches
	 * all possible sub vlnvs that need other vlnvs and adds them to the list.
	 * The list doesn't contain single vlnv twice.
	 *
	 *      @param [in] vlnv Reference to the vlnv that is used as starting point for the search.
	 *      @param [out] list Reference to the list where all vlnvs are added to.
	 *
	 *      @return List containing all vlnvs that are referenced in any of the documents.
	 */
	 virtual void getNeededVLNVs(VLNV const& vlnv, QList<VLNV>& list);

	 /*! Get list of files that are needed by the given document.
	 *
	 * This function returns a list of file paths that are converted as
	 * absolute file paths to the needed files. This function searches files
	 * only from this document, it does not search possible subcomponents.
	 *
	 *      @param [in] vlnv    Reference to the vlnv that is used for the search.
	 *      @param [out] list   The files are appended to the list if they are not already on the list.
	 */
	 virtual void getDependencyFiles(VLNV const& vlnv, QStringList& list);

	 /*! Get list of the files that are needed by the given document either directly or indirectly.
	  *
	  *      @param [in] vlnv   The vlnv identifying the document.
	  *      @param [out] list  The files are appended to the list if they are not already on the list.
	  *
	 */
	 virtual void getHierarchicalDependencyFiles(VLNV const& vlnv, QStringList& list);

	 /*! Get the library tree's root item
	 *
	 *      @return The root item.
	 */
	 virtual LibraryItem const* getTreeRoot() const;

	 /*! Get the document type of given vlnv.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 *      @param [in] vlnv Specifies the document that's type is wanted.
	 *
	 *      @return Type of the document.
	*/
	virtual VLNV::IPXactType getDocumentType(VLNV const& vlnv);

	/*! Count how many times the given component is instantiated in the library.
	 *
	 *      @param [in] vlnv Identifies the component that's instances are searched.
	 *
	 *      @return Number of found instances.
	 */
	virtual int referenceCount(VLNV const& vlnv) const;

	/*! Get the items that have referenced the given vlnv in their meta data.
	 *
	 *      @param [out] list           The search results.
	 *      @param [in] vlnvToSearch    Identifies the item to search for.
	 * 
	 *      @return Number of owners found.
	*/
	virtual int getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const;

	/*! Get the items that are needed by the specified item.
	 *
	 *      @param [in] list QList where the search results are appended.
	 *      @param [in] vlnvToSearch Identifies the item thats child-items are wanted.
	 *
	 *      @return int The number of found children.
	*/
	virtual int getChildren(QList<VLNV>& list, VLNV const& vlnvToSearch) const;

	/*! Get the VLNV of the design for a given hierarchy reference.
	 *
	 * This function can be used by calling it with a hierarchy reference found
	 * if a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design vlnv. If reference is for a
	 * configuration then the design vlnv is searched from the configuration and
	 * design vlnv is returned. If design is not found then invalid vlnv is 
	 * returned.
	 * 
	 *      @param [in] hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 *      @return VLNV The vlnv identifying the design object.
	*/
	virtual VLNV getDesignVLNV(VLNV const& hierarchyRef);

	/*! Get pointer to the design for a given hierarchy reference.
	 * 
	 * * This function can be used by calling it with a hierarchy reference found
	 * if a component's hierarchical view. Function checks if the reference is
	 * directly for a design and returns the design pointer. If reference is for a
	 * configuration then the design vlnv is searched from the configuration and
	 * design pointer is returned. If design is not found then null pointer is 
	 * returned.
	 * 
	 *      @param [in] hierarchyRef The hierarchical reference obtained from component's view.
	 *
	 *      @return QSharedPointer<Design> The design.
	*/
	virtual QSharedPointer<Design> getDesign(VLNV const& hierarchyRef);

	/*! Check if the identified object is in valid state.
	 *
	 *      @param [in] vlnv Identifies the object to check.
	 *
	 *      @return bool True if the object was valid. False if invalid or object was not found in library.
	*/
	virtual bool isValid(VLNV const& vlnv);

public slots:

	/*! Check the library items for validity
	 * 
	 * This function automatically removes the invalid library items.
	 *
	*/
	virtual void onCheckLibraryIntegrity();

	/*! Edit an item in the library
	 *
	 *      @param [in] vlnv Reference to the vlnv that identifies the object to edit.
	 *
	*/
	virtual void onEditItem(VLNV const& vlnv);

	/*! Export a library item and it's subitems to a new location.
	 *
	 *      @param [in] vlnv VLNV tag that identifies the top item to export.
	 *
	*/
	virtual void onExportItem(const VLNV vlnv);
    
	/*! Export a group of items to a new location
	 *
	 *      @param [in] vlnvs QList containing the VLNVs that are to be exported.
	 *      @param [in] targetPath Path to the location to export the objects to.
	 *
	*/
	virtual void onExportItems(const QList<VLNV> vlnvs);

    /*!
     *  Shows errors about the library item with the given VLNV.
     *
     *      @param [in] vlnv The VLNV of the library item.
     */
    virtual void onShowErrors(VLNV const& vlnv);

	/*! Selects the given vlnv item in the library
	 *
	 *      @param [in] vlnv Specifies the item in the library to select
	 *
	*/
	virtual void onSelectionChanged(VLNV const& vlnv);

	/*! Clear the item selection
	 *
	*/
	virtual void onClearSelection();

	/*! Open the specified component design
	 *
	 *      @param [in] vlnv Identifies the component that's design is wanted
	 *
	*/
	virtual void onOpenDesign(VLNV const& vlnv);

    /*!
     *  Opens the memory design of the given HW design.
     *  
     *      @param [in] vlnv    The vlnv of the HW design.
     */
    virtual void onOpenMemoryDesign(VLNV const& vlnv);

    /*! Open the specified component SW design
	 *
	 *      @param [in] vlnv Identifies the component that's SW design is wanted
	 *
	*/
	virtual void onOpenSWDesign(VLNV const& vlnv);

    /*! Open the specified component system design
	 *
	 *      @param [in] vlnv Identifies the component that's system design is wanted
	 *
	*/
	virtual void onOpenSystemDesign(VLNV const& vlnv);

	/*! Create a new item for given vlnv.
	 * 
	 * The type of the vlnv defines the type of the object to create.
	 * After creating the object an editor for it is opened.
	 *
	 *      @param [in] vlnv Identifies the object to create
	 *
	*/
	void onCreateNewItem(VLNV const& vlnv);

	/*! Create new design with given vlnv.
	 *
	 *      @param [in] vlnv The vlnv that identifies the design.
	 *
	*/
	virtual void onCreateDesign(VLNV const& vlnv);

	/*! Remove the specified library object from the library and file system.
	 * 
	 * This function does not ask user to verify the deletion, it just removes the object and
	 * it's xml file. If vlnv is for component the component's files are not removed.
	 * 
	 *      @param [in] vlnv Identifies the object.
	 *
	*/
	virtual void removeObject(VLNV const& vlnv);

	/*! Remove the specified library objects from the library and file system.
	 * 
	 * This function opens a dialog that asks user to confirm the deletion of the objects.
	 * 
	 *      @param [in] vlnvList Identifies the objects to remove.
	 *
	*/
	virtual void removeObjects(const QList<VLNV>& vlnvList);

	/*! Call this function before saving several objects to library.
	*
	* When calling this before saving and then using writeModelToFile() the 
	* library is not update between each save operation thus making the program
	* faster.
	* 
	* Be sure to call endSave() after all items are saved.
	*/
	virtual void beginSave();

	/*! End the saving operation and update the library.
	*
	* This function must be called always after calling the beginSave().
	*/
	virtual void endSave();

signals:

	//! Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! Signal that user wants to open the specified component for editing.
	void openDesign(VLNV const& vlnv, const QString& viewName);

    //! Signal that user wants to open the memory design of the specified component for editing.
    void openMemoryDesign(VLNV const& vlnv, const QString& viewName);

    //! Signal that user wants to open the specified software design for editing.
    void openSWDesign(VLNV const& vlnv, QString const& viewName);

    //! Signal that user wants to open a system for editing.
    void openSystemDesign(VLNV const& vlnv, QString const& viewName);

	//! Signal that user wants to open specified bus for editing
	void openBus(VLNV const& busDefVLNV, VLNV const& absDefVLNV, bool disableBusDef);

	//! Signal that user wants to open specified component for editing
	void openComponent(VLNV const& vlnv);

    //! Signal that user wants to open specified COM definition for editing
    void openComDefinition(VLNV const& vlnv);

    //! Signal that user wants to open specified API definition for editing
    void openApiDefinition(VLNV const& vlnv);

	//! Signal that user wants to create a new bus with given vlnv
	void createBus(VLNV const& vlnv, const QString& directory);

	//! Signal that user wants to create a new component with given vlnv
    void createComponent(KactusAttribute::ProductHierarchy,
                         KactusAttribute::Firmness,
                         VLNV const& vlnv, const QString& directory);

    //! Signal that user wants to create a new design for the given component.
    void createDesignForExistingComponent(VLNV const& vlnv);

    //! Signal that user wants to create a new SW design for the given component.
    void createSWDesign(VLNV const& vlnv);

    //! Signal that user wants to create a new system design for the given component.
    void createSystemDesign(VLNV const& vlnv);

	//! Signal that user wants to create a new abstraction definition for given bus definition.
	void createAbsDef(VLNV const& busDefVLNV, const QString& directory, bool disableBusDef);

    //! Signal that user wants to create a new COM definition with given vlnv
    void createComDef(VLNV const& vlnv, const QString& directory);

    //! Signal that user wants to create a new API definition with given vlnv
    void createApiDef(VLNV const& vlnv, const QString& directory);

	//! Signal that library has changed and VLNVDialer should be refreshed.
	void refreshDialer();

	//! Signal that the library item specified by vlnv is selected in one of the views.
	void itemSelected(VLNV const& vlnv);

	//! Signal that user wants to open a platform component.
	void openPlatformComponent(VLNV const& vlnv);

private slots:
    
	/*! Create a new abstraction definition for given bus definition.
	 *
	 *      @param [in] busDefVLNV Identifies the bus definition to create the abs def for.
	 *
	*/
	void onCreateAbsDef(VLNV const& busDefVLNV);

	/*! Remove the item with specified vlnv from the library.
	 *
	 *      @param [in] vlnv Identifies the library object to remove.
	 *
	*/
	void onRemoveVLNV(const QList<VLNV> vlnvs);

	/*! This function should be called every time an object is written to disk.
	* 
	* This function makes sure that the library object is displayed correctly 
	* on library views and possible changes made to the object are updated to
	* views.
	* 
	*      @param [in] vlnv Identifies the object that was saved.
	*
	*/
	void onItemSaved(VLNV const& vlnv);

private:

    //! No copying
    LibraryHandler(const LibraryHandler &other);

    //! No assignment
    LibraryHandler &operator=(const LibraryHandler &other);

    //-----------------------------------------------------------------------------
    // The private functions used by public class methods
    //-----------------------------------------------------------------------------

    /*!
     *  Writes a document into a file.
     *
     *      @param [in] filePath    The path to the file to write.
     *      @param [in] model       The document to write to the file.
     *
     *      @return True, if the file was successfully written, otherwise false.
     */
    bool writeFile(QString const& filePath, QSharedPointer<Document> model);

	/*! Copy the files associated with specified IP-Xact object.
	 *
	 *      @param [in] target              Directory where the files are copied to
	 *      @param [in] vlnv                Specifies the IP-Xact object that's files are copied.
	 *      @param [in/out] handledFiles    List of files that have been copied
	 *      @param [in/out] yesToAll        Info if user has selected "yes to all" to overwrite
	 *      @param [in/out] noToAll         Info is user has selected "No to all" to overwrite
	 *
	*/
	void copyFiles(QDir const& target, VLNV const& vlnv, fileList& handledFiles, bool& yesToAll,	bool& noToAll);

	/*! Copy list of files to a new location.
	 *
	 *      @param [in] files           List of files to copy
	 *      @param [in/out] sourceDir   The directory that is used as source directory for relative file paths.
	 *      @param [in/out] targetDir   The target directory that is used to generate the correct relative paths.
	 *      @param [in/out] handledFiles    List of files that have been copied
	 *      @param [in/out] yesToAll        Info if user has selected "yes to all" to overwrite
	 *      @param [in/out] noToAll         Info is user has selected "No to all" to overwrite
	 *
	*/
	void copyFiles(const QStringList& files, QDir& sourceDir, QDir& targetDir, fileList& handledFiles, 
        bool& yesToAll, bool& noToAll);

    	/*! Copy a single file
	 *
	 *      @param [in] source          The source file to be copied
	 *      @param [in/out] target T        he directory where the file is copied to.
	 *      @param [in/out] handledFiles    List of files that have been copied
	 *      @param [in/out] yesToAll        Info if user has selected "yes to all" to overwrite
	 *      @param [in/out] noToAll         Info is user has selected "No to all" to overwrite
	 *
	*/
	void copyFile(QFileInfo const& source, QDir& target, fileList& handledFiles, bool& yesToAll, bool& noToAll);

	/*! Connect the signals and slots that keep the data models synchronized.
	 *
	*/
	void syncronizeModels();
    
    /*!
     *  Connects a library view filter to dialer for selecting filter conditions.
     *
     *      @param [in] filter   The filter to connect.
     *      @param [in] dialer   The dialer for selecting filters.
     */
    void connectLibraryFilter(LibraryFilter* filter, VLNVDialer* dialer) const;

	/*! Clear the empty directories from the disk within given path.
	 *
	 *      @param [in] dirPath The path that is removed until one of the base library locations is reached.
	 *      @param [in] libraryLocations Contains the base library locations user has defined.
	 *
	*/
	void clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations);

	/*! Check if the path contains one of the given paths.
	 *
	 *      @param [in] path The path to a directory.
	 *      @param [in] pathsToSearch Paths that are checked if they are contained in the given path.
	 *
	*/
	bool containsPath(const QString& path, const QStringList& pathsToSearch) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The data model that contains the library and is model for search view
    QSharedPointer<LibraryData> data_;

	//! The model for the tree view
    QSharedPointer<LibraryTreeModel> treeModel_;

	//! The model for the hierarchy view
	QSharedPointer<HierarchyModel> hierarchyModel_;

	//! The widget that contains the GUI items for the tree tab
    LibraryTreeWidget* treeWidget_;

	//! The widget that contains the GUI items for the hierarchy tab
	HierarchyWidget* hierarchyWidget_;

	/*! Contains the library objects that have been parsed.
	 *
	 * Key = VLNV that identifies the library object.
	 * Value = pointer to the library object that has been parsed.
	 */
	QMap<VLNV, QSharedPointer<Document> > objects_;

    //! Validity status for library objects.
    QMap<VLNV, bool> objectValidity_;

	//! If true then items are being saved and library is not refreshed
	bool saveInProgress_;

	//! Contains the IP-Xact items to be added to the library.
	QMap<VLNV, QString> itemsToAdd_;

    //! Contains the IP-Xact items that have been saved, but not updated in library views yet.
    QVector<VLNV> modifiedItems_;
};

#endif // LIBRARYHANDLER_H
