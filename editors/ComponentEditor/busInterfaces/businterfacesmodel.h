//-----------------------------------------------------------------------------
// File: businterfacesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACESMODEL_H
#define BUSINTERFACESMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;
class ParameterFinder;
class Parameter;
class BusInterfaceInterface;
class Component;

//-----------------------------------------------------------------------------
//! The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------
class BusInterfacesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *    @param [in] libHandler          The instance that manages the library.
	 *    @param [in] component           The containing component.
	 *    @param [in] parameterFinder     The parameter finder.
     *    @param [in] busInterface        Interface for accessing bus interfaces.
	 *    @param [in] parent              The owner of this model.
	 */
    BusInterfacesModel(LibraryInterface* libHandler, QSharedPointer<Component> component, 
        QSharedPointer<ParameterFinder> parameterFinder, BusInterfaceInterface* busInterface, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~BusInterfacesModel() = default;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
     */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
     */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section The section specifies the row/column number for the header.
	 *    @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *    @param [in] role Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
     */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index Specifies the item that's data is requested.
	 *    @param [in] role The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
     */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *    @param [in] index The model index of the item that's data is to be saved.
	 *    @param [in] value The data that is to be saved.
	 *    @param [in] role The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving happened successfully.
     */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

    /*!
     *  Returns the supported actions of a drop.
     *
     *    @return The drop actions supported by the model.
     */
    Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *    @return The supported MIME types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *    @param [in] data   The data associated to the drop.
     *    @param [in] action The drop action.  
     *    @param [in] row    The row beneath the drop position.
     *    @param [in] column The column beneath the drop position.
     *    @param [in] parent The parent index of the drop position.
     *
     *    @return True, if the model could handle the data, otherwise false.
     */
    bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column,
        QModelIndex const& parent);

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *    @param [in] index The index identifying the position for new item.
     */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *    @param [in] index The index identifying the item to remove.
     */
	virtual void onRemoveItem(QModelIndex const& index);

	/*!
     *  Move item from the original position to new position.
	 *
	 *    @param [in] originalPos The index identifying the item to move.
	 *    @param [in] newPos      The index identifying the position of the item after the move.
     */
    virtual void onMoveItem( QModelIndex const& originalPos, QModelIndex const& newPos );

    /*!
     *  Handle the bus interface copying.
     *
     *    @param [in] indexList   List of indexes to be copied.
     */
    void onCopyRows(QModelIndexList indexList);

    /*!
     *  Handle the bus interface pasteing.
     */
    void onPasteRows();

signals:

	/*!
     *  Emitted when the contents of the model change.
     */
	void contentChanged();

	/*!
     *  Emitted when a new bus interface is added to the model.
	 *
	 *    @param [in] index The index of the added bus interface.
     */
	void busifAdded(int index);

	/*!
     *  Emitted when a bus interface is removed from the model.
	 *
	 *    @param [in] index The index of the bus interface to remove.
     */
	void busifRemoved(int index);

    /*!
     *  Emitted when a bus interface is moved from one position to another.
     *
     *    @param [in] source The row number of the bus interface before the move.
     *    @param [in] target The row number of the bus interface after the move.
     */
    void busIfMoved(int source, int target);

    /*!
     *  Increase the number of references made to the specified parameter.
     *
     *    @param [in] id  Id of the parameter whose reference count is increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references made to the specified parameter.
     *
     *    @param [in] id  Id of the parameter whose reference count is decreased.
     */
    void decreaseReferences(QString id);

    /*!
     *  Emitted when a abstraction def or bus def with different std revision is dropped in the editor.
     */
    void stdRevisionMismatch() const;

private:

    //! No copying. No assignment.
	BusInterfacesModel(const BusInterfacesModel& other);
	BusInterfacesModel& operator=(const BusInterfacesModel& other);

    /*!
     *  Remove references from the expressions contained within the removed bus interface.
     *
     *    @param [in] busInterfaceIndex   The index of the removed bus interface.
     */
    void removeReferencesFromExpressions(int busInterfaceIndex);

    /*!
     *  Increase the number of references made in the selected bus interface.
     *
     *    @param [in] registerName    Name of the selected bus interface.
     */
    void increaseReferencesInPastedBus(QString const& busName);

    /*!
     *  Calculates the parameters used in the selected bus interface.
     *
     *    @param [in] registerName    Name of the selected bus interface.
     *
     *    @return A map containing pairs of referenced ids and the number of references made to them.
     */
    QMap<QString, int> getReferencedParameters(QString const& busName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The containing component.
    QSharedPointer<Component> containingComponent_;
};

#endif // BUSINTERFACESMODEL_H
