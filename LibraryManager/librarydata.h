/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include "vlnv.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QFileInfo>
#include <QList>
#include <QXmlSchema>
#include <QMap>
#include <QSharedPointer>

class LibraryHandler;

/*! \brief LibraryData is the data model that manages the actual VLNV library.
 * 
 * This class reads and saves the library file in file system and provides 
 * services to manage the library. This class also provides model for a table
 * view so contents of the library can be viewed.
 *
 */
class LibraryData : public QAbstractItemModel {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	LibraryData(LibraryHandler *parent);

	//! \brief The destructor
	virtual ~LibraryData();

	/*! \brief Search for IP-Xact files within path
	 *
	 * \param path Specifies the path to search for IP-Xact files.
	*/
	void searchForFiles(const QString& path);

	/*! \brief Open a library file and parse it and add the files to the library
	 *
	 * \param filePath The path to the library file that is opened. If this is
	 * not specified then the default library file is used.
	 * 
	 * The default library file is created to the user's home directory in 
	 * directory ./kactus2
	 *
	*/
	void openLibraryFile(const QString filePath = QString());

	/*! \brief Save the library to a file.
	 *
	 * \param filePath The path to the file to be saved. If this is not 
	 * specified the default library file is used.
	 *
	*/
	void saveLibraryFile(const QString filePath = QString());

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
	 * \param refreshLibrary If true then the library views are rebuilt.
	 *
	 * \return True if the vlnv was added, false if not.
	*/
	bool addVLNV(const VLNV& vlnv, const QString& path, bool refreshLibrary);

	/*! \brief Update the VLNV for the given document.
	 * 
	 * If the library does not contain the given vlnv yet it is added to the
	 * library. 
	 * Note: Although it is possible, this function should not be used to 
	 * add new items to the library because it may be slow (linear time).
	 * This function should be used to change the vlnv of an existing document.
	 * 
	 * \param path The path to the document in the file system.
	 * \param newVLNV The new vlnv for the document.
	 *
	*/
	//void updateVLNV(const QString& path, const VLNV& newVLNV);

	/*! \brief Update the path of the given vlnv object.
	 *
	 * \param vlnv Identifies the vlnv object.
	 * \param path Path to the object's file.
	 *
	*/
	void updatePath(const VLNV& vlnv, const QString& path);

	/*! \brief Checks if the library already contains the specified vlnv
	 *
	 * \param vlnv The vlnv that is searched from the library
	 *
	 * \return True if the vlnv is found in the library
	*/
	bool contains(const VLNV& vlnv);

	/*! \brief create a model index for the item with given row, column and
	 * parent
	 *
	 * \param row row of the item
	 * \param column column of the item
	 * \param parent item's parent's model index
	 * \return model index of the item
	 */
	virtual QModelIndex index(int row, int column,
			const QModelIndex & parent = QModelIndex()) const;

	/*! \brief Fuction returns a model index of the items parent
	 *
	 * \param child model index of the child-item
	 * \return Always returns an invalid model index because this is not 
	 * hierarchical model.
	 */
	virtual QModelIndex parent(const QModelIndex& index) const;

	/*! \brief Returns the number of rows in model
	 *
	 * \param parent model index of the item
	 * \return number of rows in model
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Returns the number of columns in the model
	 *
	 * \param parent model index of the item
	 * \return number of columns in the model
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Returns data saved in the node identified by index
	 *
	 * Data is returned in QVariant. The node is identified by model index.
	 * The data is returned only for supported roles, for other an invalid
	 * QVariant is returned.
	 * \param index model index of the node
	 * \param role Int-type parameter to identify the role the data is provided
	 * for
	 * \return QVariant type which holds the nodes data
	 */
	virtual QVariant data(const QModelIndex& index,
			int role = Qt::DisplayRole) const;

	/*! \brief Returns the flags that give information how the node can be used
	 *
	 * \param index the model index of the wanted node
	 * \return the flags describing how the node can be handled
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Provide the headerdata for views
	 *
	 * \param section Specifies the column of the header
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specifies the type of data wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;

	/*! \brief Get the library items stored in the model.
	 *
	 * \return QList containing pointers to the items
	 */
	QList<VLNV*> getItems() const;

	/*! \brief Checks if the specified VLNV is found in the library.
	 *
	 * \param vlnv The vlnv that is to be searched.
	 *
	 * \return True if vlnv is found, false if not.
	*/
	bool exists(const VLNV& vlnv) const;

	/*! \brief Get the type of the given document.
	 * 
	 * If vlnv is not found in the library then VLNV::INVALID is returned.
	 *
	 * \param vlnv Specifies the document that's type is wanted.
	 *
	 * \return VLNV::IPXactType The type of the given document.
	*/
	VLNV::IPXactType getType(const VLNV& vlnv) const;

	/*! \brief Get pointer to the VLNV instance owned by the Library Data
	 *
	 * \param vlnv Reference to the vlnv that contains same fields as the searched vlnv.
	 *
	 * \return Pointer to the original vlnv instance.
	*/
	VLNV* getOriginalPointer(const VLNV& vlnv) const;

	/*! \brief Check that the given file path contains a valid IP-Xact file.
	 *
	 * If vlnvToCompare is used then this function also checks that the vlnv of
	 * the file matches the given vlnv.
	 *
	 * \param filePath Absolute file path to the file to be checked.
	 * \param vlnvToCompare If specified this vlnv is compared to the file's vlnv
	 * and if they don't match then false is returned.
	 *
	 * \return bool False if file path does not contain a valid IP-Xact file or 
	 * if vlnvToCompare is given and vlnvs don't match.
	*/
	bool isValidIPXactFile(const QString& filePath, const VLNV& vlnvToCompare = VLNV());

signals:

	//! \brief Emit an error message to be printed to user.
	void errorMessage(const QString& message);

	//! \brief Emit a notice message to be printed to user.
	void noticeMessage(const QString& message);

	//! \brief Emit a message that component is to be opened
	void openComponent(const VLNV& vlnv);

	//! \brief Inform tree model that a vlnv is to be removed from the tree.
	void removeVLNV(VLNV* vlnv);

	//! \brief Inform tree model that the model should be reset
	void resetModel();

	//! \brief Inform tree model that the vlnv should be hidden
	//void hideVLNV(VLNV* vlnv);

	//! \brief Inform tree model that the vlnv should be visible
	//void showVLNV(VLNV* vlnv);

	//! \brief Open the design of a component.
	void openDesign(const VLNV& vlnv);

	//! \brief Open the component in a component editor.
	void editItem(const VLNV& vlnv);

	//! \brief Create a new bus definition.
	void createBusDef(const VLNV& vlnv);

	//! \brief Create a new component with given vlnv.
	void createComponent(const VLNV& vlnv);

	//! \brief Create new design with given vlnv.
	void createDesign(const VLNV& vlnv);

public slots:

	//! \brief Open the selected hierarchical design
	void onOpenDesign(const QModelIndex& index);

	//! \brief When open is selected in search view
	void onOpenComponent(const QModelIndex& index);

	//! \brief Create new component
	void onCreateNewComponent(const QModelIndex& index);

	//! \brief Create new design
	void onCreateNewDesign(const QModelIndex& index);

	//! \brief Open bus definition in an editor
	void onOpenBusDef(const QModelIndex& index);

	//! \brief Create a new bus definition
	void onCreateBusDef(const QModelIndex& index);

	//! \brief When delete is selected in search view
	void onDeleteItem(const QModelIndex& index);

	//! \brief When export is selected in search view
	void onExportItem(const QModelIndex& index);

	//! \brief Remove the specified VLNV from the library
	void onRemoveVLNV(VLNV* vlnv);

	/*! \brief Check the integrity of the library.
	 *
	*/
	void checkIntegrity();

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

	/*! \brief Scan folder and its subfolders for IP-XACT documents
	 *
	 * Function takes a path and starts scanning it recursively to find
	 * valid IP-XACT documents. The documents are validated and valid ones
	 * are added to the library database
	 * \param path the path to start scanning from
	 */
	void scanFolder(const QString &path/*, QXmlSchema &schema*/);

	/*! \brief Validate xml file against given xml schema
	 *
	 * This function uses the schema it is given as parameter and validates
	 * the file given to it.
	 * \param file QFileInfo object that points to file to be validated
	 * \param schema QXxmlSchema object that has the schema loaded into it
	 * \return True if the file is valid according to schema. False if not.
	 */
	bool validateIPXactDoc(QFileInfo &file, QXmlSchema &schema);

	/*! \brief Add a new IP-XACT file to the component library
	 *
	 * Before calling this function the file to be added MUST be validated
	 * against IP-Xact schema. If invalid files are given to this function
	 * it will cause a segmentation fault.
	 *
	 * \exception Invalid_file Occurs if the file can not be opened.
	 *
	 * \parameter filePath the path to the IP-Xact document to be added
	 */
	void addFileToLibrary(const QString& filePath);

	/*! \brief Check what kind of IP-Xact document is the file
	 *
	 * NOTE: this function must be called only for .xml files
	 *
	 * \param file QFileInfo object pointing to a file that's type needs to
	 * be checked
	 * \return enum IPXactType which tells what type IP-Xact document the file
	 * is or if it's not a IP-Xact doc.
	 */
	VLNV::IPXactType checkDocType(QFileInfo &file);

	/*! \brief Map containing all the VLNVs that are in the library.
	 *
	 * key: VLNV instance containing the vlnv information
	 * value: filepath of the component
	 */
	QMap<VLNV, QString> libraryItems_;

	/*! \brief The data structure that is used to create the model for the views
	 *
	 * The list contains pointers to LibraryItems that hold info of item's
	 * visibility status and also contain pointer to the item's vlnv element.
	 */
	QList<VLNV*> table_;

	//! \brief Pointer to the LibraryHandler instance that owns this dataModel.
	LibraryHandler *handler_;

};

#endif // LIBRARYDATA_H
