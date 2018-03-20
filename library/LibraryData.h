//-----------------------------------------------------------------------------
// File: librarydata.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryData is the data model that manages the file access for VLNV items
//-----------------------------------------------------------------------------

#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include "DocumentFileAccess.h"
#include "DocumentValidator.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QFile>
#include <QFileSystemWatcher>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QRegExpValidator>
#include <QWidget>

class AbstractionDefinition;
class BusDefinition;
class Component;
class Document;
class LibraryInterface;
class MessageMediator;

//-----------------------------------------------------------------------------
//! LibraryData is the data model that manages the file access for VLNV items
//-----------------------------------------------------------------------------
class LibraryData : public QObject
{
    Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in] library         The library owning this data object.
     *      @param [in] messageChannel  Channel for user messages.
     *      @param [in] parent          The parent object.
     */
    LibraryData(LibraryInterface* library, MessageMediator* messageChannel, QObject* parent);

    //! The destructor
    virtual ~LibraryData();

    /*! Returns the absolute file path of the specified IP-XACT document.
     *
     *      @param [in] vlnv The vlnv that specifies the wanted IP-XACT document.
     *
     *      @return The absolute filepath of the document.
    */
    QString getPath(VLNV const& vlnv) const;

    /*! Add the given vlnv to the library
     *
     *      @param [in] vlnv    The vlnv to be added.
     *      @param [in] path    The absolute file path to the file to be added to the library.
     *
     *      @return True if the vlnv was added, false if not.
    */
    bool addVLNV(VLNV const& vlnv, QString const& path);

    /*! Checks if the library already contains the specified vlnv
     *
     *      @param [in] vlnv The vlnv that is searched from the library
     *
     *      @return True if the vlnv is found in the library
    */
    bool contains(VLNV const& vlnv) const;
    
    /*! Get the library items stored in the model.
     *
     *      @return All the items.
     */
    QList<VLNV> getItems() const;

    /*! Get a model that matches given VLNV.
     *
     * This function can be called to get a model that matches an IP-XACT document.
     * 
     *      @param [in] vlnv Identifies the desired document.
     *
     *      @return The model that matches the document. If vlnv is not found, a null pointer is returned.
     *              The ownership of the parsed object is passed to the caller.
    */
    QSharedPointer<Document> getModel(VLNV const& vlnv);

    /*!
     *  Writes a document into a file.
     *
     *      @param [in] model       The document to write to the file.
     *      @param [in] filePath    The path to the file to write. If empty, current path is used.
     *
     *      @return True, if the file was successfully written, otherwise false.
     */
    bool writeFile(QSharedPointer<Document> model, QString const& filePath = QString());

    /*! Check the document validity.
     *
     *      @param [in] document    The document to check.
     *
     *      @return True if the document was valid, otherwise false.
     */
    bool validateDocument(QSharedPointer<Document> document);

    /*! Finds errors in document validity.
     *
     *      @param [in] document    The document to check.
     *      @param [in] path        The path to the document IP-XACT file.
     *
     *      @return Any errors within the document.
    */
    QVector<QString> findErrorsInDocument(QSharedPointer<Document> document);

    /*! Search all saved library paths for IP-Xact objects.
     * 
     * The found objects are displayed in the library.
     * When search is complete the library integrity is checked.
     * 
    */
    void parseLibrary(QStringList const& locations);

    //! Check the integrity of the library.
    void checkLibraryIntegrity();

signals:

    //! Inform tree model that a vlnv is to be removed from the tree.
    void removeVLNV(VLNV const& vlnv);

    //! Inform tree model that a vlnv is to be added to the tree.
    void addVLNV(VLNV const& vlnv);

    //! Inform that object has been updated.
    void updatedVLNV(VLNV const& vlnv);

public slots:
   
    //! Remove the specified VLNV from the library
    void onRemoveVLNV(VLNV const& vlnv);

 private slots:

    /*!
     *  Called when a known IP-XACT file has changed on the disk.
     *
     *      @param [in] path    The path to the changed file.
     */
    void onFileChanged(QString const& path);

private:

    //! No copying
    LibraryData(const LibraryData& other);

    //! No assignment
    LibraryData& operator=(const LibraryData& other);

    //! Runs one step of the library parsing.
    void performParseLibraryStep(QString const& location);

    //! Runs one step of the integrity check.
    void performIntegrityCheck(VLNV const& documentVLNV);

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
    
    /*! Search the directory and it's sub-directories for IP-Xact objects.
     *
     *      @param [in] directoryPath The absolute path of the directory to start the search.
    */
    void parseDirectory(QString const& directoryPath);

    /*! Check if the file in given path is IP-XACT file and if it is then save it.
     *
     *      @param [in] filePath Absolute file path to the file to check.
    */
    void parseFile(QString const& filePath);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The LibraryHandler instance that owns this class.
    LibraryInterface* library_;

    //! Channel for notice and error messages to the user.
    MessageMediator* messageChannel_;

    /*! Map containing all the VLNVs that are in the library.
     *
     * key: VLNV instance containing the vlnv information
     * value: filepath of the component
     */
    QMap<VLNV, QString> libraryItems_;

    
    //! Number of failed objects found during the integrity check.
    int failedObjects_;

    //! The total number of files all library components contain.
    int fileCount_;

    //! Checks if the given string is a URL (invalids are allowed) or not.
    QRegularExpressionValidator urlTester_;

    DocumentFileAccess fileAccess_;

    DocumentValidator validator_;

    //! Watch for changes in the IP-XACT files.
    QFileSystemWatcher fileWatch_;

};

#endif // LIBRARYDATA_H
