/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include "vlnv.h"
#include <models/librarycomponent.h>

#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QFileInfo>
#include <QList>
#include <QXmlSchema>
#include <QMap>
#include <QSharedPointer>
#include <QMainWindow>

class LibraryHandler;

/*! \brief LibraryData is the data model that manages the actual VLNV library.
 * 
 * This class reads and saves the library file in file system and provides 
 * services to manage the library. This class also provides model for a table
 * view so contents of the library can be viewed.
 *
 */
class LibraryData : public QObject {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	LibraryData(LibraryHandler *parent, QMainWindow* mainWnd);

	//! \brief The destructor
	virtual ~LibraryData();

	/*! \brief Returns the absolute file path of the specified IP-Xact document.
	 *
	 * \param vlnv The vlnv that specifies the wanted IP-Xact document.
	 *
	 * \return const QString that contains the absolute filepath of the document.
	*/
	const QString getPath(const VLNV& vlnv);

	/*! \brief Add the given vlnv to the library
	 *
	 * \param vlnv The vlnv to be added.
	 * \param path The absolute file path to the file to be added to the library.
	 *
	 * \return True if the vlnv was added, false if not.
	*/
	bool addVLNV(const VLNV& vlnv, const QString& path);

	/*! \brief Checks if the library already contains the specified vlnv
	 *
	 * \param vlnv The vlnv that is searched from the library
	 *
	 * \return True if the vlnv is found in the library
	*/
	bool contains(const VLNV& vlnv);

	/*! \brief Get the library items stored in the model.
	 *
	 * \return QList containing the items.
	 */
	QList<VLNV> getItems() const;

	/*! \brief Get the type of the given document.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 * \param vlnv Specifies the document that's type is wanted.
	 *
	 * \return VLNV::IPXactType The type of the given document.
	*/
	VLNV::IPXactType getType(const VLNV& vlnv) const;

	/*! \brief Search all saved library paths for IP-Xact objects.
	 * 
	 * The found objects are displayed in the library.
	 * When search is complete the library integrity is checked.
	 * 
	 * \param showProgress If true then the progress bar displaying the progress
	 * of check is displayed to user.
	*/
	void parseLibrary(bool showProgress = true);

	/*! \brief Get a model that matches given VLNV.
	 *
	 * This function can be called to get a model that matches an IP-Xact document.
	 * 
	 * \param vlnv Identifies the desired document.
	 *
	 * \return Shared pointer to the model that matches the document.
	 * If vlnv is not found in library a null pointer is returned. The ownership 
	 * of the parsed object is passed to the caller.
	*/
	QSharedPointer<LibraryComponent> getModel(const VLNV& vlnv);

	/*! \brief Check the integrity of the library.
	 *
	 * \param showProgress If true then the progress bar displaying the progress
	 * of check is displayed to user.
	*/
	void checkLibraryIntegrity(bool showProgress = true);

signals:

	//! \brief Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! \brief Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! \brief Inform tree model that a vlnv is to be removed from the tree.
	void removeVLNV(const VLNV& vlnv);

	//! \brief Inform tree model that a vlnv is to be added to the tree.
	void addVLNV(const VLNV& vlnv);

	//! \brief Inform tree model that the model should be reset
	void resetModel();

public slots:

	//! \brief Remove the specified VLNV from the library
	void onRemoveVLNV(const VLNV& vlnv);

	//! \brief Reset the library
	void resetLibrary();

private:

	//! \brief No copying
	LibraryData(const LibraryData& other);

	//! \brief No assignment
	LibraryData& operator=(const LibraryData& other);

	/*! \brief Ask user to select a directory.
	 *
	 * The function prompts user to select a directory and sets the path to the
	 * directory into list at index 0. The list is cleared before adding the
	 * directory path.
	 *
	 * \param list Reference to the list where the directory is saved to.
	 */
	void getDirectory(QStringList& list);

	/*! \brief Search the directory and it's sub-directories for IP-Xact objects.
	 *
	 * \param directoryPath The absolute path of the directory to start the search.
	 *
	*/
	void parseDirectory(const QString& directoryPath);

	/*! \brief Check if the file in given path is IP-Xact file and if it is then save it.
	 *
	 * \param filePath Absolute file path to the file to check.
	 *
	*/
	void parseFile(const QString& filePath);

    //! The main window.
    QMainWindow* mainWnd_;

	/*! \brief Map containing all the VLNVs that are in the library.
	 *
	 * key: VLNV instance containing the vlnv information
	 * value: filepath of the component
	 */
	QMap<VLNV, QString> libraryItems_;

	//! \brief Pointer to the LibraryHandler instance that owns this class.
	LibraryHandler *handler_;

};

#endif // LIBRARYDATA_H
