//-----------------------------------------------------------------------------
// File: PortMapTreeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREEMODEL_H
#define PORTMAPTREEMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QModelIndex>
#include <QIcon>
#include <QAbstractItemModel>
#include <QSharedPointer>

class AbstractionDefinition;
class BusInterface;
class Component;
class LibraryInterface;
class VLNV;
class ExpressionParser;
class PortAbstraction;
class PortMap;
class ExpressionFormatter;
class PortMapValidator;
class Port;
class AbstractionType;
class PortMapInterface;

//-----------------------------------------------------------------------------
//! A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapTreeModel : public QAbstractItemModel, ParameterizableTable
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component           The component to which this model is made.
     *      @param [in] handler             The library handler.
     *      @param [in] parameterFinder     The used parameter finder.
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] parent              The owner of this model.
     */
    PortMapTreeModel(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        PortMapInterface* portMapInterface,
        QObject *parent);

    /*!
     *  The destructor.
     */
    ~PortMapTreeModel();

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
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelIndex that identifies the parent of the given object.
     */
    virtual QModelIndex parent(QModelIndex const& child) const;

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
    void onRemovePort(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing all port maps connected to the logical port.
     *
     *      @param [in] itemIndex   The index of the logical port.
     */
    void onRemoveAllChildItemsFrom(QModelIndex const& itemIndex);

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

private:

    // Disable copying.
    PortMapTreeModel(PortMapTreeModel const& rhs);
    PortMapTreeModel& operator=(PortMapTreeModel const& rhs);

    /*!
     *  Create a parent index for a port map.
     *
     *      @param [in] childItem   The selected port map.
     *
     *      @return Parent index for the port map.
     */
    QModelIndex createParentIndexForPortMap(PortMap* childItem) const;

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
     *  Switch the logical port of the port map.
     *
     *      @param [in] index               The current index of the port map.
     *      @param [in] logicalPortName     Name of the new logical port.
     *      @param [in] portMapIndex        Index of the selected port map
     *      @param [in] value               The new logical port name.
     */
    void switchMappedLogicalPort(QModelIndex const& index, std::string const& logicalPortName,
        int const& portMapIndex, QVariant const& value);

    /*!
     *  Emit a port connection signal.
     *
     *      @param [in] oldName     The old name of the port.
     *      @param [in] newName     The new name of the port.
     */
    void sendPortConnectionSignal(QString const& oldName, QString const& newName);

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index               The selected index.
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        The selected port map index.
     */
    QVariant getBackgroundColour(QModelIndex const& index, std::string const& logicalPortName,
        int const& portMapIndex) const;

    /*!
     *  Get the logical port name and index of the port map matching the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return Pair containing the logical port name and index of the selected port map.
     */
    QPair<std::string, int> getPortNamePortMapIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The instance that manages the library.
    LibraryInterface* handler_;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPTREEMODEL_H
