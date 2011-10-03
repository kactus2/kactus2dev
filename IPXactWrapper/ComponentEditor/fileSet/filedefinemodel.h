/* 
 *
 *  Created on: 15.2.2011
 *      Author: Antti Kamppi
 * 		filename: filedefinemodel.h
 */

#ifndef FILEDEFINEMODEL_H
#define FILEDEFINEMODEL_H

#include <models/file.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>

class File;

/*! \brief Model to display the define-elements for a file.
 *
 */
class FileDefineModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Define contains the elements for a single spirit:define element.
	struct Define {
	
		//! \brief Name of the define symbol
		QString name_;

		//! \brief The element name for display purposes.
		QString displayName_;

		//! \brief The description of the define.
		QString description_;

		//! \brief The value of the define parameter.
		QString value_;

		/*! \brief The constructor
		 *
		 * \param name Name of the define
		 * \param value Value of the define parameter
		 *
		*/
		Define(const QString name, const QString value);

		/*! \brief The constructor
		 *
		 * \param name Name of the define
		 * \param displayName displayName of the define
		 * \param description Description of the define
		 * \param value Value of the define parameter.
		 *
		*/
		Define(const QString name, const QString displayName, 
			const QString description, const QString value);

		/*! \brief The default constructor
		 *
		*/
		Define();
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param file Pointer to the file being modified.
	 *
	 * \return 
	*/
	FileDefineModel(QObject *parent, File* file);

	//! \brief The destructor
	virtual ~FileDefineModel();

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Always returns 4
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * \param section The column that's header is wanted
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specified the type of data that is wanted.
	 *
	 * \return QT_NAMESPACE::QVariant
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get information on how specified item can be handled.
	 *
	 * \param index Specifies the item that's flags are wanted.
	 *
	 * \return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Set the data for specified item.
	 *
	 * \param index Specifies the item that's data is modified
	 * \param value The value to be set.
	 * \param role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * \return True if data was successfully set.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! \brief Insert rows to the model
	 *
	 * \param row The row to start inserting rows to.
	 * \param count The number of rows to insert.
	 * \param parent Model index of the parent to which the rows are inserted.
	 *
	 * \return True if rows were successfully inserted.
	*/
	bool insertRows(int row, int count, 
		const QModelIndex& parent = QModelIndex());

	/*! \brief Remove rows from the model
	 *
	 * \param row The row to start removing rows from.
	 * \param count The number of rows to remove
	 * \param parent Model index of the parent from which the rows are removed.
	 *
	 * \return True if rows were successfully removed.
	*/
	bool removeRows(int row, int count, 
		const QModelIndex& parent = QModelIndex());

	/*! \brief Is the model in valid condition.
	 *
	 * \return True if the items in the model are valid.
	*/
	bool isValid() const;

	/*! \brief Apply the changes from the editor to the model
	 *
	*/
	void apply();

	/*! \brief Restore the changes from the model to the editor.
	 *
	*/
	void restore();

signals:

	//! \brief Emitted when the contents of the editor change
	void contentChanged();

private:

	//! \brief No copying
	FileDefineModel(const FileDefineModel& other);

	//! No assignment
	FileDefineModel& operator=(const FileDefineModel& other);
	
	//! \brief The defines displayed in the model
	QList<Define> defines_;

	//! \brief Pointer to the file being modified.
	File* file_;
};

#endif // FILEDEFINEMODEL_H
