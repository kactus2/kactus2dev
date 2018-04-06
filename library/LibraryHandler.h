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

#include "LibraryInterface.h"
#include "LibraryTreeWidget.h"
#include "LibraryTreeModel.h"

#include "DocumentFileAccess.h"
#include "DocumentValidator.h"

#include "HierarchyView/hierarchymodel.h"
#include "HierarchyView/hierarchywidget.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QString>
#include <QTabWidget>
#include <QStringList>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSharedPointer>
#include <QMap>
#include <QObject>

typedef QVector<QFileInfo> fileList;

class Document;
class LibraryItem;
class MessageMediator;
class TableViewDialog;

class ObjectSelectionDialog;
class ObjectSelectionListItem;

//-----------------------------------------------------------------------------
//! LibraryHandler is the class that implements the services to manage the IP-XACT library.
//-----------------------------------------------------------------------------
class LibraryHandler : public QObject, public LibraryInterface
{
    Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in] dialer  The dialer that provides search options.
     *      @param [in] parent  The parent widget of this instance.
     */
    LibraryHandler(QWidget* parentWidget, MessageMediator* messageChannel, QObject* parent = 0);

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

    /*! Gets all the VLNVs currently in the library.
     *
     *      @return All known VLNVs in the library.
    */
    virtual QList<VLNV> getAllVLNVs() const;

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

    HierarchyModel *getHierarchyModel();

    LibraryTreeModel *getTreeModel();

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
    virtual void onExportItem(VLNV const& vlnv);
    
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

    //!  Shows a report of all errors within the library items.
    void onGenerateIntegrityReport();

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
     *      @param [in] vlnv         Identifies the component that's design is wanted
     *      @param [in] viewName     Identifies the view for the design.
     *
    */
    virtual void onOpenDesign(VLNV const& vlnv, QString const& viewName);

    /*!
     *  Opens the memory design of the given HW design.
     *  
     *      @param [in] vlnv        The vlnv of the HW design.
     *      @param [in] activeView  The active view of the selected HW design.
     */
    virtual void onOpenMemoryDesign(VLNV const& vlnv, QString const& activeView);

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

    //! Emit a change in the library status e.g. loading.
    void statusMessage(QString const& message);

    void progressStatus();

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

    //! Signal that user wants to open specified catalog for editing
    void openCatalog(VLNV const& vlnv);

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

    //! Inform that object has been updated.
    void updatedVLNV(VLNV const& vlnv);

    //! Inform tree model that a vlnv is to be added to the tree.
    void addVLNV(VLNV const& vlnv);

    //! Inform tree model that a vlnv is to be removed from the tree.
    void removeVLNV(VLNV const& vlnv);


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

    //! Closes the integrity report widget.
    void onCloseIntegrityReport();

    void onFileChanged(const QString &path);
private:

    //! No copying
    LibraryHandler(const LibraryHandler &other);

    //! No assignment
    LibraryHandler &operator=(const LibraryHandler &other);

    //-----------------------------------------------------------------------------
    // The private functions used by public class methods
    //-----------------------------------------------------------------------------

    /*! Check the validity of VLNV references within a document.
     *
     *      @param [in] document    The document to check.
     *
     *      @return True if the VLVN references are valid, otherwise false.
     */
    bool validateDependentVLNVReferencences(QSharedPointer<Document> document);

    /*!
     *  Finds any errors within a given document VLNV references.
     *
     *      @param [in] document    The document whose VLNV references to check.
     *      @param [out] errorList  The list of errors to add any found errors.
     */
    void findErrorsInDependentVLNVReferencences(QSharedPointer<const Document> document, QVector<QString>& errorList);

    /*! Check the validity of directory references within a document.
     *
     *      @param [in] document    The document to check.
     *      @param [in] documentPath    The path to the document XML file.
     *
     *      @return True if the directory references are valid, otherwise false.
     */
    bool validateDependentDirectories(QSharedPointer<Document> document, QString const& documentPath);

    /*!
     *  Finds any errors within a given document directory references.
     *
     *      @param [in] document        The document whose directory references to check.
     *      @param [in] documentPath    The path to the document XML file.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInDependentDirectories(QSharedPointer<const Document> document, QString const& documentPath,
        QVector<QString>& errorList);

    /*! Check the validity of file references within a document.
     *
     *      @param [in] document    The document to check.
     *
     *      @return True if the file references are valid, otherwise false.
     */
    bool validateDependentFiles(QSharedPointer<Document> document, QString const& documentPath);

    /*!
     *  Finds any errors within a given document file references.
     *
     *      @param [in] document        The document whose file references to check.
     *      @param [in] documentPath    The path to the document XML file.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInDependentFiles(QSharedPointer<const Document> document, QString const& documentPath,
        QVector<QString>& errorList);

    /*! Copy the files associated with specified IP-Xact object.
     *
     *      @param [in] target              Directory where the files are copied to
     *      @param [in] vlnv                Specifies the IP-Xact object that's files are copied.
     *      @param [in/out] handledFiles    List of files that have been copied
     *      @param [in/out] yesToAll        Info if user has selected "yes to all" to overwrite
     *      @param [in/out] noToAll         Info is user has selected "No to all" to overwrite
     *
    */
    void copyFiles(QDir const& target, VLNV const& vlnv, fileList& handledFiles, bool& yesToAll,    bool& noToAll);

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
     *      @param [in] source              The source file to be copied
     *      @param [in/out] target T        he directory where the file is copied to.
     *      @param [in/out] handledFiles    List of files that have been copied
     *      @param [in/out] yesToAll        Info if user has selected "yes to all" to overwrite
     *      @param [in/out] noToAll         Info is user has selected "No to all" to overwrite
     *
     *      @return True, if the file was copied, false otherwise.
     */
    bool copyFile(QFileInfo const& source, QDir& target, fileList& handledFiles, bool& yesToAll, bool& noToAll);

    /*! Connect the signals and slots that keep the data models synchronized.
     *
    */
    void syncronizeModels();


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
    
    /*!
     *  Construct the items for the selection dialog.
     *
     *      @param [in] exportDialog    The selection dialog.
     *      @param [in] exportedVLNVs   List of selectable VLNVs.
     */
    void constructItemsForSelectionDialog(ObjectSelectionDialog* exportDialog, const QList<VLNV> exportedVLNVs);

    /*!
     *  Export the selected VLNV object.
     *
     *      @param [in] destinationFolder   Destination folder for the export.
     *      @param [in] vlnv                VLNV of the exported item.
     *      @param [in] handledFiles        List of the files that have been handled.
     *      @param [in] yesToAll            Container for yes to all status.
     *      @param [in] noToAll             Container for no to all status.
     *
     *      @return True, if the selected VLNV object was exported, false otherwise.
     */
    bool exportSelectedVLNVObject(QDir const& destinationFolder, VLNV const& vlnv, fileList& handledFiles,
        bool& yesToAll, bool& noToAll);

    /*!
     *  Export the selected objects.
     *
     *      @param [in] exportedItems       The selected items to be exported.
     *      @param [in] destinationPath     The export destination path.
     *
     *      @return The count of the exported items in a pair of 1. VLNVs and 2. files.
     */
    QPair<int, int> exportSelectedObjects(QVector<ObjectSelectionListItem*> exportedItems,
        QString const& destinationPath);

    /*!
     *  Create the export message.
     *
     *      @param [in] vlnvCount           The amount of exported VLNV items.
     *      @param [in] fileCount           The amount of exported files.
     *      @param [in] destinationPath     The export destination.
     *
     *      @return The constructed export message.
     */
    QString createExportMessage(int vlnvCount, int fileCount, QString const& destinationPath) const;

    /*!
     *  Create the delete message.
     *
     *      @param [in] vlnvCount   The amount of deleted VLNV items.
     *      @param [in] fileCount   The amount of deleted files.
     *
     *      @return The constructed delete message.
     */
    QString createDeleteMessage(int vlnvCount, int fileCount) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Widget to parent open dialogs.
    QWidget* parentWidget_;
    
    MessageMediator* messageChannel_;

    DocumentFileAccess fileAccess_;

    struct DocumentInfo
    {
        QSharedPointer<Document> document;
        QString path;
        bool isValid;

        DocumentInfo(QString const& filePath = QString(), QSharedPointer<Document> doc = nullptr, bool valid = false):
            document(doc), path(filePath), isValid(valid) {}
    };

    struct CheckStatistics
    {
        int totalFileCount;
        int invalidDocumentCount;
    };

    /*! Contains the library objects that have been parsed.
     *
     * Key = VLNV that identifies the library object.
     * Value = pointer to the library object that has been parsed.
     */
    QMap<VLNV, DocumentInfo> documentCache_;


    //! Checks if the given string is a URL (invalids are allowed) or not.
    QRegularExpressionValidator urlTester_;

    DocumentValidator validator_;

    //! The model for the tree view
    LibraryTreeModel* treeModel_;

    //! The model for the hierarchy view
    HierarchyModel* hierarchyModel_;

    //! Widget for showing integrity report on-demand.
    TableViewDialog* integrityWidget_;

    //! If true then items are being saved and library is not refreshed
    bool saveInProgress_;

    bool writeFile(QSharedPointer<Document> model, const QString &filePath);
    void parseLibrary(const QStringList &locations);
    void parseDirectory(const QString &directoryPath);
    void parseFile(const QString &filePath);
    bool validateDocument(QSharedPointer<Document> document, const QString &documentPath);
    QVector<QString> findErrorsInDocument(QSharedPointer<Document> document, const QString &path);

    //! Watch for changes in the IP-XACT files.
    QFileSystemWatcher fileWatch_;

    CheckStatistics checkResults_;


    void showNotFoundError(VLNV const& vlnv);
};

#endif // LIBRARYHANDLER_H
