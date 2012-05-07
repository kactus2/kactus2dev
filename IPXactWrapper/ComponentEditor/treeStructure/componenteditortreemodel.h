/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditortreemodel.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORTREEMODEL_H
#define COMPONENTEDITORTREEMODEL_H

#include "componenteditoritem.h"

#include <QAbstractItemModel>

class ComponentEditorTreeModel : public QAbstractItemModel {
	Q_OBJECT

public:

	ComponentEditorTreeModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ComponentEditorTreeModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

private:
	//! \brief No copying
	ComponentEditorTreeModel(const ComponentEditorTreeModel& other);

	//! \brief No assignment
	ComponentEditorTreeModel& operator=(const ComponentEditorTreeModel& other);

	//! \brief Pointer to the root item of the tree.
	QSharedPointer<ComponentEditorItem> rootItem_;
};

#endif // COMPONENTEDITORTREEMODEL_H
