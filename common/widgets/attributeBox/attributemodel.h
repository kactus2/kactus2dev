/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#ifndef ATTRIBUTEMODEL_H_
#define ATTRIBUTEMODEL_H_

#include <QAbstractTableModel>
#include <QObject>
#include <QVariant>
#include <QSharedPointer>
#include <QMap>

/*! \brief AttributeModel is a model class to display the attributes.
 *
 * Model is for a table view with 2 columns: Name and value of the attribute.
 * This model contains the attributes and holds them in a datastructure where
 * they can be queried by other classes.
 */
class AttributeModel: public QAbstractTableModel {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 */
	AttributeModel(QObject* parent = 0);

	//! \brief The destructor
	virtual ~AttributeModel();

	/*! \brief Get the number of rows to display.
	 *
	 * \param parent QModelIndex of the parent whose row count is wanted.
	 *
	 * \return Number of rows.
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns to display.
	 *
	 * This function always returns 2.
	 *
	 * \param parent QModelIndex of the parent whose columns are wanted.
	 *
	 * \return Number of columns, always 2.
	 */
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data to be displayed in a single item.
	 *
	 * \param index ModelIndex of the wanted item
	 * \param role Specifies the role the data is wanted for.
	 *
	 * \return Data for the given item with specified role.
	 */
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the data to be displayed in the header.
	 *
	 * \param section Specifies which section's header is wanted.
	 * \param orientation Specifies the orientation of the header, only
	 * Qt::Horizontal is supported by this model.
	 * \param role Specifies the role the data is wanted for.
	 *
	 * \return Data for the given header with given role.
	 */
	QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for a single item.
	 *
	 * \param index Specifies the item that's data is to be changed.
	 * \param value The data that is to be stored in the item.
	 * \param role Specifies the role that changes the data.
	 *
	 * \return True if data was successfully set, false otherwise.
	 */
	bool setData(const QModelIndex& index, const QVariant& value,
			int role = Qt::EditRole);

	/*! \brief Get the flags that define the supported operations for item.
	 *
	 * \param index ModelIndex that specifies the item.
	 *
	 * \return ItemFlags that tell what operations are possible for given item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get attributes stored in the model.
	 *
	 * \return QMap containg the attributes name-value pairs
	 *  Key = name of the attribute
	 *  Value = value of the attribute
	 */
	QMap<QString, QString> getAttributes() const;

	/*! \brief Remove an attribute from the model.
	 *
	 * \param index QModelIndex that identifies the attribute to remove.
	 */
	void removeAttribute(QModelIndex index);

	/*! \brief Create a new attribute to the model.
	 *
	 * \param name Name of the attribute.
	 * \param value Value of the attribute.
	 */
	void addAttribute(const QString& name, const QString& value);

	/*! \brief Check that all attributes stored in the model are valid.
	 *
	 * This function checks that there is no empty names or values.
	 *
	 * \return True if all names and values contain non-empty string.
	 */
	bool validAttributes() const;

	/*! \brief Set attributes for the model.
	 *
	 * \param attributes QMap containing the attributes to be set for the model.
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Remove all attributes from the model.
	 *
	*/
	void clear();

signals:

	/*! \brief This signal is emitted when an attribute changes in the model.
	 *
	 * This is used to tell the view to resize it's columns to match the size
	 * of the items.
	 * 
	 * \param column Specifies which column of the attributeModel is changed.
	 */
	void attributeChanged(int column);

private:

	//! No copying
	AttributeModel(const AttributeModel& other);

	//! No assignment
	AttributeModel& operator=(const AttributeModel& other);

	//! \brief AttributePair contains info for one attribute name-value-pair.
	struct AttributePair {

		//! \brief Name of the attribute
		QString name_;

		//! \brief Value of the attribute
		QString value_;

		/*! \brief struct constructor
		 *
		 * \param name Name of the attribute
		 * \param value Value of the attribute
		 */
		AttributePair(const QString name, const QString value);
	};

	//! \brief Contains the attributes in the model.
	QList<QSharedPointer<AttributePair> > attributes_;
};

#endif /* ATTRIBUTEMODEL_H_ */
