/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include "vlnv.h"
#include <models/librarycomponent.h>

#include <QAbstractItemModel>
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
class LibraryData : public QAbstractItemModel {

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
	//bool exists(const VLNV& vlnv) const;

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

	//! \brief Emit a message that component is to be opened
	void openComponent(const VLNV& vlnv);

	//! \brief Inform tree model that a vlnv is to be removed from the tree.
	void removeVLNV(VLNV* vlnv);

	//! \brief Inform tree model that a vlnv is to be added to the tree.
	void addVLNV(VLNV* vlnv);

	//! \brief Inform tree model that the model should be reset
	void resetModel();

	//! \brief Open the design of a component.
	void openDesign(const VLNV& vlnv);

    //! \brief Open the SW design of a component.
    void openSWDesign(const VLNV& vlnv);

    //! \brief Open the system design of a component.
    void openSystemDesign(const VLNV& vlnv);

	//! \brief Open the component in a component editor.
	void editItem(const VLNV& vlnv);

	//! \brief Create a new bus definition.
	void createBusDef(const VLNV& vlnv);

    //! \brief Create a new COM definition.
    void createComDef(const VLNV& vlnv);

    //! \brief Create a new API definition.
    void createApiDef(const VLNV& vlnv);

	//! \brief Create a new component with given vlnv.
	void createComponent(const VLNV& vlnv);

	//! \brief Create new design with given vlnv.
	void createDesign(const VLNV& vlnv);

    //! \brief Create new SW design with given vlnv.
    void createSWDesign(const VLNV& vlnv);

public slots:

	//! \brief Open the selected hierarchical design
	void onOpenDesign(const QModelIndex& index);

    //! \brief Open the selected SW design
    void onOpenSWDesign(const QModelIndex& index);

    //! \brief Open the selected system design
    void onOpenSystemDesign(const QModelIndex& index);

	//! \brief When open is selected in search view
	void onOpenComponent(const QModelIndex& index);

	//! \brief Create new component
	void onCreateNewComponent(const QModelIndex& index);

	//! \brief Create new design
	void onCreateNewDesign(const QModelIndex& index);


    //! \brief Create new SW design
    void onCreateNewSWDesign(const QModelIndex& index);

	//! \brief Open bus definition in an editor
	void onOpenBusDef(const QModelIndex& index);

    //! \brief Open COM definition in an editor
    void onOpenComDef(const QModelIndex& index);

    //! \brief Open API definition in an editor
    void onOpenApiDef(const QModelIndex& index);

	//! \brief Create a new bus definition
	void onCreateBusDef(const QModelIndex& index);

    //! \brief Create a new COM definition
    void onCreateComDef(const QModelIndex& index);

    //! \brief Create a new API definition
    void onCreateApiDef(const QModelIndex& index);

	//! \brief When delete is selected in search view
	void onDeleteItem(const QModelIndex& index);

	//! \brief When export is selected in search view
	void onExportItem(const QModelIndex& index);

	//! \brief Remove the specified VLNV from the library
	void onRemoveVLNV(VLNV* vlnv);

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

	/*! \brief The data structure that is used to create the model for the views
	 *
	 * The list contains pointers to LibraryItems that hold info of item's
	 * visibility status and also contain pointer to the item's vlnv element.
	 */
	QList< QSharedPointer<VLNV> > table_;

	//! \brief Pointer to the LibraryHandler instance that owns this dataModel.
	LibraryHandler *handler_;

};

#endif // LIBRARYDATA_H
