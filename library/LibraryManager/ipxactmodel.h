/* 
 *
 *  Created on: 18.6.2010
 */

#ifndef IPXACTMODEL_H_
#define IPXACTMODEL_H_

#include <IPXACTmodels/vlnv.h>

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QModelIndex>
#include <QVariant>
#include <QSharedPointer>
#include <QString>

// Class that wraps one QDomNode
class IPXactItem;

/*! \brief IPXactModel class represents one IP-XACT document
 *
 * IPXactModel class parses and stores the indormation in a single
 * IP-XACT document and provides services for reading and writing that
 * document. The document passed to this class must already be read from
 * a file succesfully.
 */
class IPXactModel : public QAbstractItemModel {

	// A macro that must appear in the private section of a class definition
	Q_OBJECT

public:

	/*! \brief A Constructor to create an instance for one IP-XACT document
	 *
	 * This constructor takes a document and its parent
	 *
	 * \param document A QDomDocument that has the information of the IP-XACT
	 * document
	 * \param parent QObject-pointer that points to parent object of the model
	 */
	IPXactModel(QDomDocument document, QObject *parent = 0);

	/*! \brief The destructor of the class IPXactModel
	 *
	 * This destructor deletes the rootItem of the model, which calls the
	 * destructors of the child items to free the allocated memory
	 */
	~IPXactModel();

	/*! \brief Get the data of the node identified by index
	 *
	 * Returns QVariant data that is saved in the node identified by index
	 *
	 * \param index A reference to QModelIndex that indentifies the desired
	 * node
	 * \param role Int-type parameter to identify the role the data is
	 * provided for
	 * Supported roles:
	 * - DisplayRole
	 * - ToolTipRole
	 * - EditRole
	 * \return QVariant type which holds the nodes data
	 */
	QVariant data(const QModelIndex &index, int role) const;

	/*! \brief set the role data for the item at index to value
	 *
	 * This function is used to edit the data identified with index
	 * \param index A reference to the index that's value is to be edited
	 * \param value A reference to the value to be set
	 * \param role The role the data is supplied to
	 * Supported roles:
	 * - EditRole
	 * \return true is operation is succesfull, false if not
	 */
	bool setData(const QModelIndex &index, const QVariant &value,
			int role = Qt::EditRole);

	/*! \brief Returns the item flags for the given index
	 *
	 * \param index To identify which flags are wanted
	 * \return Qt::ItemFlags The itemflags of the object identified by index
	 */
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/*! \brief Get the headerdata of the model
	 *
	 * The function returns the header of the section identified by section
	 * parameter. Role is checked and no data is given to inappropriate roles
	 *
	 * \param section Int-type parameter to identify the section of the header
	 * \param role What is the role of the functions caller
	 * Supported roles:
	 * - DisplayRole
	 * \return QVariant The data in the header of the identified section
	 */
	QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;

	/*! \brief Create a model index for the item with the given row, column
	 * and parent
	 *
	 * \param row Row of the item
	 * \param column Column of the item
	 * \param parent Reference to the items parent
	 * \return QModelIndex Created model index of the item, If no suitable
	 * child item is found an invalid ModelIndex is returned
	 */
	QModelIndex index(int row, int column,
			const QModelIndex &parent = QModelIndex()) const;

	/*! \brief Function returns a model index of the child's parent
	 *
	 * The model index for the parent of the child is created and returned
	 *
	 * \param child A reference to the child whose parent index is created
	 * \return QModelIndex The created index of the parent. If the parent
	 * item is root item or it doesn't exist an invalid model index is returned
	 */
	QModelIndex parent(const QModelIndex &child) const;

	/*! \brief Returns number of rows beneath the given item
	 *
	 * \param parent The number of the rows beneath the item identified by
	 * parent
	 * \return Int-type indicating the number of rows
	 */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	/*! \brief Get the number of columns of the item
	 *
	 * \param parent A reference to the item whose column count is wanted
	 * \return Int-type indicating the amount of columns
	 */
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	/*! \brief get the VLNV of the document
	 *
	 * Returns the Vendor, library, name and version information of the
	 * IP-XACT document
	 * \return The VLNV struct that holds the information
	 */
	VLNV getVLNV();

	/*! \brief Get the type of the document
	 *
	 * This function can be used to find out what type of IP-XACT document
	 * the document is. If the document type is not identified then invalid
	 * is returned
	 * \return The VLNV::IPXactType enum value. INVALID if the document is not
	 * one of the IP-Xact docs.
	 */
	VLNV::IPXactType getDocType() const;


private:

	/*! \brief No cpying
	 *
	 */
	IPXactModel(const IPXactModel &other);

	/*! \brief No assignment
	 *
	 */
	IPXactModel &operator=(const IPXactModel &other);

	/*!
	 * \brief The data in the XML-document
	 */
	QDomDocument IPXactDoc_;

	/*!
	 * \brief Pointer to the root item in the XML document
	 */
	QSharedPointer<IPXactItem> rootItem_;

};

#endif /* IPXACTMODEL_H_ */
