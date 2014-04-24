/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametermodel.h
 */

#ifndef MODELPARAMETERMODEL_H
#define MODELPARAMETERMODEL_H

#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/component.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QSharedPointer>
#include <QString>

/*! \brief Table model that contains the model parameters for the table view.
 *
 * This model can be used to edit and update the model parameters of a component.
 */
class ModelParameterModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	ModelParameterModel(QSharedPointer<Component> component,
		QObject *parent);

	//! \brief The destructor
	virtual ~ModelParameterModel();

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
	 * \return Always returns 5
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

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! \brief Get the index of the identified model parameter.
	 *
	 * Method: 		index
	 * Full name:	ModelParameterModel::index
	 * Access:		virtual public 
	 *
	 * \param modelParamName The name of the model parameter which's index is requested.
	 *
	 * \return QModelIndex of the first column of the specified model parameter. Invalid if named parameter is not found.
	*/
	virtual QModelIndex index(QSharedPointer<ModelParameter> modelParam) const;

    virtual QSharedPointer<ModelParameter> getParameter(QModelIndex const& index) const;

public slots:

	/*! \brief Remove a row from the model
	 *
	 * \param row Specifies the row to remove
	*/
	void onRemoveRow(int row);

	/*! \brief Add a new empty row to the model
	 *
	*/
	void onAddRow();

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void onAddItem(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void onRemoveItem(const QModelIndex& index);

	/*! \brief Add a new model parameter to the model
	 *
	 * Method: 		addModelParameter
	 * Full name:	ModelParameterModel::addModelParameter
	 * Access:		public 
	 *
	 * \param modelParam The model parameter to be added.
	 *
	*/
	void addModelParameter(QSharedPointer<ModelParameter> modelParam);

    void removeModelParameter(QSharedPointer<ModelParameter> removedParam);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    
    void modelParameterRemoved(QSharedPointer<ModelParameter> modelParam);

private:

	//! \brief No copying
	ModelParameterModel(const ModelParameterModel& other);

	//! No assignment
	ModelParameterModel& operator=(const ModelParameterModel& other);

    /*!
     *   Locks the name, type and usage columns of a parameter model.
     *
     *      @param [in] modelParam The parameter model to lock.
     */
    void lockModelParameter(QSharedPointer<ModelParameter> modelParam);

 /*!
     *   Unocks the name, type and usage columns of a parameter model.
     *
     *      @param [in] modelParam The parameter model to lock.
     */
    void unlockModelParameter(QSharedPointer<ModelParameter> modelParam);

    /*!
     *   Locks the given index disabling editing.
     *
     *      @param [in] index The index to lock.
     */
    void lockIndex(QModelIndex const& index);

  /*!
     *   Unlocks the given index disabling editing.
     *
     *      @param [in] index The index to unlock.
     */
    void unlockIndex(QModelIndex const& index);

    /*!
     *   Checks if given index is locked.
     *
     *      @param [in] index The index to check.
	 *
	 *      @return True if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

	//! \brief The table that is displayed to the user.
	QList<QSharedPointer<ModelParameter> >& table_;

    //! \brief The locked indexes that cannot be edited.
    QList<QPersistentModelIndex> lockedIndexes_;
};

#endif // MODELPARAMETERMODEL_H
