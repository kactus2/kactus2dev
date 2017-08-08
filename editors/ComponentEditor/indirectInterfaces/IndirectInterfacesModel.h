//-----------------------------------------------------------------------------
// File: IndirectInterfacesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Data model for component indirect interfaces.
//-----------------------------------------------------------------------------

#ifndef INDIRECT_INTERFACES_MODEL_H
#define INDIRECT_INTERFACES_MODEL_H

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class IndirectInterface;
//class IndirectInterfaceValidator;
class Component;
class ParameterFinder;
class Parameter;

//-----------------------------------------------------------------------------
//! Data model for component indirect interfaces.
//-----------------------------------------------------------------------------
class IndirectInterfacesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The owner of this model.
	 */
	IndirectInterfacesModel(QSharedPointer<Component> component,
        //QSharedPointer<IndirectInterfaceValidator> validator,
        QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);
	
	//! The destructor
	virtual ~IndirectInterfacesModel();

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section The section specifies the row/column number for the header.
	 *      @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is requested.
	 *      @param [in] role The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index The model index of the item that's data is to be saved.
	 *      @param [in] value The data that is to be saved.
	 *      @param [in] role The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	*/
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(QModelIndex const& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new bus interface is added to the model.
	 *
	 *      @param [in] index The index of the added bus interface.
	 *
	*/
	void interfaceAdded(int index);

	/*! Emitted when a bus interface is removed from the model.
	 *
	 *      @param [in] index The index of the bus interface to remove.
	 *
	*/
	void interfaceRemoved(int index);

    /*!
     *  Increase the number of references made to the specified parameter.
     *
     *      @param [in] id  Id of the parameter whose reference count is increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references made to the specified parameter.
     *
     *      @param [in] id  Id of the parameter whose reference count is decreased.
     */
    void decreaseReferences(QString id);

private:

	//! No copying
	IndirectInterfacesModel(const IndirectInterfacesModel& other);
	IndirectInterfacesModel& operator=(const IndirectInterfacesModel& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The component being edited.
	QSharedPointer<Component> component_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

	//! The indirect interfaces being edited.
	QSharedPointer<QList<QSharedPointer<IndirectInterface> > > indirectInterfaces_;

    // QSharedPointer<IndirectInterfaceValidator> validator_;

};

#endif // INDIRECT_INTERFACES_MODEL_H
