//-----------------------------------------------------------------------------
// File: librarydata.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryData is the data model that manages the actual VLNV library.
//-----------------------------------------------------------------------------

#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <IPXACTmodels/Design/validator/DesignValidator.h>

#include <IPXACTmodels/designConfiguration/validators/DesignConfigurationValidator.h>

#include <IPXACTmodels/kactusExtensions/validators/SystemDesignConfigurationValidator.h>

#include <QObject>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QTimer>
#include <QRegExpValidator>
#include <QWidget>

class AbstractionDefinition;
class BusDefinition;
class Component;
class Document;
class LibraryHandler;
class ScanProgressWidget;

class ComponentParameterFinder;

//-----------------------------------------------------------------------------
//! LibraryData is the data model that manages the actual VLNV library.
//-----------------------------------------------------------------------------
class LibraryData : public QObject
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *      @param [in] parent          The parent object of this widget.
	 *      @param [in] parentWidget    The widget containing the displayed library.
	*/
	LibraryData(LibraryHandler* parent, QWidget* parentWidget);

	//! The destructor
	virtual ~LibraryData();

	/*! Returns the absolute file path of the specified IP-Xact document.
	 *
	 *      @param [in] vlnv The vlnv that specifies the wanted IP-Xact document.
	 *
	 *      @return The absolute filepath of the document.
	*/
	QString getPath(VLNV const& vlnv);

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
	bool contains(VLNV const& vlnv);

	/*! Get the library items stored in the model.
	 *
	 *      @return All the items.
	 */
	QList<VLNV> getItems() const;

	/*! Get the type of the given document.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 *      @param [in] vlnv    Specifies the document that's type is wanted.
	 *
	 *      @return The type of the given document.
	*/
	VLNV::IPXactType getType(VLNV const& vlnv) const;

	/*! Search all saved library paths for IP-Xact objects.
	 * 
	 * The found objects are displayed in the library.
	 * When search is complete the library integrity is checked.
	 * 
	*/
	void parseLibrary();

    /*! Get a model that matches given VLNV.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 *      @param [in] vlnv Identifies the desired document.
	 *
	 *      @return The model that matches the document. If vlnv is not found, a null pointer is returned.
     *              The ownership of the parsed object is passed to the caller.
	*/
	QSharedPointer<Document> getModel(VLNV const& vlnv);

	//! Check the integrity of the library.
	void checkLibraryIntegrity();

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
	QVector<QString> findErrorsInDocument(QSharedPointer<Document> document, QString const& path);
   
signals:

	//! Emit an error message to be printed to user.
	void errorMessage(QString const& message);

	//! Emit a notice message to be printed to user.
	void noticeMessage(QString const& message);

	//! Inform tree model that a vlnv is to be removed from the tree.
	void removeVLNV(VLNV const& vlnv);

	//! Inform tree model that a vlnv is to be added to the tree.
	void addVLNV(VLNV const& vlnv);

	//! Inform tree model that the model should be reset
	void resetModel();

public slots:

    //! Runs one step of the library parsing.
    void performParseLibraryStep();

    //! Runs one step of the integrity check.
    void performIntegrityCheckStep();
   
    //! Remove the specified VLNV from the library
	void onRemoveVLNV(VLNV const& vlnv);

	//! Reset the library
	void resetLibrary();

private:

	//! No copying
	LibraryData(const LibraryData& other);

	//! No assignment
	LibraryData& operator=(const LibraryData& other);

    /*!
     *  Change the component used in the component validator parameter finder.
     *
     *      @param [in] targetComponent     The new component.
     */
    void changeComponentValidatorParameterFinder(QSharedPointer<Component> targetComponent);

	/*! Search the directory and it's sub-directories for IP-Xact objects.
	 *
	 *      @param [in] directoryPath The absolute path of the directory to start the search.
	*/
	void parseDirectory(QString const& directoryPath);

	/*! Check if the file in given path is IP-Xact file and if it is then save it.
	 *
	 *      @param [in] filePath Absolute file path to the file to check.
	*/
	void parseFile(QString const& filePath);

    /*!
     *  Finds any errors within a given bus definition document.
     *
     *      @param [in]  busDefinition   The bus definition to search in.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInBusDefinition(QSharedPointer<BusDefinition> busDefinition, QVector<QString>& errorList);
    
    /*!
     *  Finds any errors within a given abstraction definition document.
     *
     *      @param [in]  abstraction    The bus definition to search in.
     *      @param [out] errorList      The list of errors to add any found errors.
     */
    void findErrorsInAbstractionDefinition(QSharedPointer<AbstractionDefinition> abstraction, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given component.
     *
     *      @param [in] component   The selected component.
     *      @param [in] errorList   The list of errors.
     */
    void findErrorsInComponent(QSharedPointer<Component> component, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given design.
     *
     *      @param [in] design      The selected design.
     *      @param [in] errorList   The list of errors.
     */
    void findErrorsInDesign(QSharedPointer<Design> design, QVector<QString>& errorList);

    /*!
     *  Finds any errors within a given design configuration.
     *
     *      @param [in] configuration   The selected design configuration.
     *      @param [in] errorList       The list of errors.
     */
    void findErrorsInDesignConfiguration(QSharedPointer<DesignConfiguration> configuration,
        QVector<QString>& errorList);

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

    /*!
     *  Finds the VLNV identifier for the given document.
     *
     *      @param [in] doc   The document whose VLNV to find.
     *
     *      @return The VLNV identifier for the document.
     */
    VLNV getDocumentVLNV(QDomDocument& doc);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent widget.
    QWidget* parentWidget_;

	/*! Map containing all the VLNVs that are in the library.
	 *
	 * key: VLNV instance containing the vlnv information
	 * value: filepath of the component
	 */
	QMap<VLNV, QString> libraryItems_;

	//! The LibraryHandler instance that owns this class.
	LibraryHandler *handler_;

    //! The progress dialog widget for scans.
    ScanProgressWidget* progWidget_;

    //! The number of timer steps.
    int timerSteps_;

    //! The index of the current timer step.
    int timerStep_;

    //! The timer for scans. 
    QTimer* timer_;

    //! The location list to scan.
    QList<QString> locations_;

    //! The iterator for integrity check scan.
    QMap<VLNV, QString>::iterator iterObjects_;

    //! Number of errors found during the integrity check.
    int errors_;

    //! Number of failed objects found during the integrity check.
    int failedObjects_;

    //! Number of syntax errors found during the integrity check.
    int syntaxErrors_;

    //! Number of VLNV errors found during the integrity check.
    int vlnvErrors_;

    //! Number of file errors found during the integrity check.
    int fileErrors_;

	//! The total number of files all library components contain.
	 int fileCount_;

	//! Checks if the given string is a URL (invalids are allowed) or not.
	 QRegExpValidator* urlTester_;

     //! The parameter finder used in the component validator.
     QSharedPointer<ComponentParameterFinder> componentValidatorFinder_;

     //! The used component validator.
     ComponentValidator componentValidator_;

     //! The used design validator.
     DesignValidator designValidator_;

     //! The used design configuration validator.
     DesignConfigurationValidator designConfigurationValidator_;

     //! The used system design configuration validator.
     SystemDesignConfigurationValidator systemDesignConfigurationValidator_;
};

#endif // LIBRARYDATA_H
