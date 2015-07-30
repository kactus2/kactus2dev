//-----------------------------------------------------------------------------
// File: businterfacesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACESMODEL_H
#define BUSINTERFACESMODEL_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------
class BusInterfacesModel : public QAbstractTableModel {
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The owner of this model.
	 */
	BusInterfacesModel(LibraryInterface* libHandler, QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);
	
	//! The destructor
	virtual ~BusInterfacesModel();

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

	/*! Check if the bus interfaces model is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

    /*!
     *  Returns the supported actions of a drop.
     *
     *      @return The drop actions supported by the model.
     */
    Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *      @return The supported MIME types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *      @param [in] data   The data associated to the drop.
     *      @param [in] action The drop action.  
     *      @param [in] row    The row beneath the drop position.
     *      @param [in] column The column beneath the drop position.
     *      @param [in] parent The parent index of the drop position.
     *
     *      @return True, if the model could handle the data, otherwise false.
     */
    bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column,
        QModelIndex const& parent);

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

	/*! Move item from the original position to new position.
	 *
	 *      @param [in] originalPos The index identifying the item to move.
	 *      @param [in] newPos      The index identifying the position of the item after the move.
     *
	*/
    virtual void onMoveItem( QModelIndex const& originalPos, QModelIndex const& newPos );

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new bus interface is added to the model.
	 *
	 *      @param [in] index The index of the added bus interface.
	 *
	*/
	void busifAdded(int index);

	/*! Emitted when a bus interface is removed from the model.
	 *
	 *      @param [in] index The index of the bus interface to remove.
	 *
	*/
	void busifRemoved(int index);

    /*! Emitted when a bus interface is moved from one position to another.
     *
     *      @param [in] source The row number of the bus interface before the move.
     *      @param [in] target The row number of the bus interface after the move.
     *
     */
    void busIfMoved(int source, int target);

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
	BusInterfacesModel(const BusInterfacesModel& other);

	//! No assignment
	BusInterfacesModel& operator=(const BusInterfacesModel& other);

    /*!
     *  Remove references from the expressions contained within the removed bus interface.
     *
     *      @param [in] busInterfaceIndex   The index of the removed bus interface.
     */
    void removeReferencesFromExpressions(int busInterfaceIndex);
   
	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! The bus interfaces being edited.
	QList<QSharedPointer<BusInterface> >& busifs_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // BUSINTERFACESMODEL_H
