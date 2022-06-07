//-----------------------------------------------------------------------------
// File: PortMapModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.05.2022
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPMODEL_H
#define PORTMAPMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

class PortMapInterface;

//-----------------------------------------------------------------------------
//! A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapModel : public ReferencingTableModel, ParameterizableTable
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterFinder     The used parameter finder.
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] parent              The owner of this model.
     */
    PortMapModel(QSharedPointer<ParameterFinder> parameterFinder, PortMapInterface* portMapInterface,
        QObject* parent);

    /*!
     *  The destructor.
     */
    virtual ~PortMapModel() = default;

    /*!
     *  Refreshes the model so that modifications from outside are made visible.
     */
    void refresh();


    /*!
     *  Resets the model and restores the port maps from the component.
     */
    void reset();

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent  Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent  Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data associated with given object.
     *
     *      @param [in] index Model index that identifies the object that's data is wanted.
     *      @param [in] role Specifies the type of data wanted.
     *
     *      @return QVariant Containing the requested data.
     */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	 */
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */);

    /*!
     *  Get the flags that identify possible methods for given object.
     *
     *      @param [in] index   Model index identifying the object that's flags are requested.
     *
     *      @return Qt::ItemFlags that specify how the object can be handled.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Get the supported drop actions.
     *
     *      @return The supported drop actions.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Drop the mime data.
     *
     *      @param [in] data    The actual data.
     *      @param [in] action  The used drop action.
     *      @param [in] row     The selected row.
     *      @param [in] column  The selected column.
     *      @param [in] parent  Parent index of the new data.
     */
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
        const QModelIndex &parent);

public slots:

    /*!
     *  Handler for adding a new port map.
     *
     *      @param [in] itemIndex   The index of the new port.
     */
    void onAddPortMap(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing a port map.
     *
     *      @param [in] itemIndex   The index of the removed port.
     */
    void onRemovePortMap(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing all port maps from all the logical ports.
     */
    void onRemoveAllPortMappings();

    /*!
     *  Add a port map to the correct parent item.
     *
     *      @param [in] physicalPorts   The weighted physical ports.
     */
    void onAddAutoConnectedPortMaps(QVector<QString> physicalPorts);

signals:

    /*!
     *  Emitted when the contents have changed.
     */
    void contentChanged();

    /*!
     *  Emitted when a port is connected.
     *
     *      @param [in] portName    The name of the connected port.
     */
    void portConnected(QString const& portName);

    /*!
     *  Emitted when a port is disconnected.
     *
     *      @param [in] portName    The name of the disconnected port.
     */
    void portDisconnected(QString const& portName);

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;
    
    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index whose expression to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const; 

    /*!
     *  Validates the data in an index.
     *
     *      @param [in] index   The index whose data to validate
     *
     *      @return True, if the data in the index is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The id of the referenced parameter.
     *
     *      @return The amount of references made to the selected id on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const override final;

private:

    // Disable copying.
    PortMapModel(PortMapModel const& rhs);
    PortMapModel& operator=(PortMapModel const& rhs);

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the expression of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the value for the selected index.
     *
     *      @param [in] index           The selected index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Check if an index is valid.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the selected index is valid, otherwise false.
     */
    bool indexIsValid(QModelIndex const& index) const;

    /*!
     *  Emit a port connection signal.
     *
     *      @param [in] oldName     The old name of the port.
     *      @param [in] newName     The new name of the port.
     */
    void sendPortConnectionSignal(QString const& oldName, QString const& newName);

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPMODEL_H
