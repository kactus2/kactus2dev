//-----------------------------------------------------------------------------
// File: InterfacePortMapModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.05.2016
//
// Description:
// Table model for visualizing interface port maps.
//-----------------------------------------------------------------------------

#ifndef INTERFACEPORTMAPMODEL_H
#define INTERFACEPORTMAPMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

class PortMap;
class BusInterface;
class LibraryInterface;
class ConnectionEndpoint;
class AbstractionDefinition;
class Component;
class ComponentParameterFinder;
class ExpressionParser;
class Design;
class ActiveInterface;

//-----------------------------------------------------------------------------
//! Table model for visualizing interface port maps.
//-----------------------------------------------------------------------------
class InterfacePortMapModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] handler     The library handler.
     *      @param [in] parent      Pointer to the owner of this model.
	 */
	InterfacePortMapModel(LibraryInterface* library, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~InterfacePortMapModel();

    /*!
     *  Sets the interface data source being edited.
     *
     *      @param [in] busItem             The data source.
     *      @param [in] activeInterfaces    A list of active interfaces containing a reference to the selected item.
     */
    void setInterfaceData(ConnectionEndpoint* busItem, QList<QSharedPointer<ActiveInterface> > activeInterfaces);

	/*!
     *  Returns the number of rows in the model.
	 *
	 *      @param [in] parent Model index of the parent of the item. Must be invalid because this is not a
     *                  hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Returns the number of columns in the model.
	 *
	 *      @param [in] parent Model index of the parent of the item. Must be invalid because this is not a
     *                  hierarchical model.
	 *
	 *      @return Always returns 3.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Returns the data for the specified item for specified role.
	 *
	 *      @param [in] index  Identifies the item that's data is wanted.
	 *      @param [in] role   Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Returns the data for the headers.
	 *
	 *      @param [in] section      The column that's header is wanted
	 *      @param [in] orientation  Only Qt::Horizontal is supported
	 *      @param [in] role         Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index  Specifies the item that's data is modified
	 *      @param [in] value  The value to be set.
	 *      @param [in] role   The role that is trying to modify the data. Only Qt::EditRole
	 *                         is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Returns information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

signals:

	/*!
     *  Emitted when contents of the model change.
     */
	void contentChanged();

private:
    // Disable copying.
    InterfacePortMapModel(InterfacePortMapModel const& rhs);
    InterfacePortMapModel& operator=(InterfacePortMapModel const& rhs);

    /*!
     *  Check if a given port is excluded.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is excluded, otherwise false.
     */
    bool portIsExcluded(QString const& portName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Structure of a port mapping item.
    struct MappingItem
    {
        //! The logical port and its bounds.
        QString logicalPort_;

        //! Informs whether the logical port is ok or not.
        bool logicalIsOk_;

        //! The name of the physical port.
        QString physicalPortName_;

        //! The physical port and its bounds.
        QString physicalPort_;

        //! Informs whether the physical port is ok or not.
        bool physicalIsOk_;

        //! Is the mapped port excluded.
        bool isExcluded_;
    };

    //! The library handler
    LibraryInterface* libraryHandler_;

    //! List of the mapped port items.
    QList<MappingItem> mappingItems_;

    //! The component parameter finder.
    QSharedPointer<ComponentParameterFinder> componentFinder_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! List of the active interfaces referencing the selected end point.
    QList<QSharedPointer<ActiveInterface> > activeInterfaces_;

    //! The selected end point.
    ConnectionEndpoint* endPoint_;
};

//-----------------------------------------------------------------------------

#endif // INTERFACEPORTMAPMODEL_H