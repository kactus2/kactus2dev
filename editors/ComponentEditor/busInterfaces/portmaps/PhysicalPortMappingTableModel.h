//-----------------------------------------------------------------------------
// File: PhysicalPortMappingTableModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Model to display the physical ports of a component in port mapping.
//-----------------------------------------------------------------------------

#ifndef PHYSICALPORTMAPPINGTABLEMODEL_H
#define PHYSICALPORTMAPPINGTABLEMODEL_H

#include "PortMappingTableModel.h"

#include <QAbstractTableModel>
#include <QSharedPointer>

class Component;
class Port;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Model to display the physical ports of a component in port mapping.
//-----------------------------------------------------------------------------
class PhysicalPortMappingTableModel : public PortMappingTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component           The component that's ports are displayed.
     *      @param [in] componentParser     The used expression parser.
	 *      @param [in] parent              The owner of this model.
	 */
	PhysicalPortMappingTableModel(QSharedPointer<Component> component,
        QSharedPointer<ExpressionParser> componentParser, QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~PhysicalPortMappingTableModel();;

	/*!
     *  Refresh the list of physical ports.
	 */
	virtual void refresh();

    /*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid because this is not
     *                              a hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return The requested data.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

public slots:

    /*!
     *  Add a new port to the list of ports.
     *
     *      @param [in] portName    The name of the port to be added.
     */
    void addPort(QString const& portName);

private:

	//! No copying.
	PhysicalPortMappingTableModel(const PhysicalPortMappingTableModel& other);
	PhysicalPortMappingTableModel& operator=(const PhysicalPortMappingTableModel& other);
	
    /*!
     *  Get the size of the selected port.
     *
     *      @param [in] selectedPort    The selected port.
     */
    QVariant getPortSize(QSharedPointer<Port> selectedPort) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The component being edited.
	QSharedPointer<Component> component_;

    //! The list of available ports.
    QList<QSharedPointer<Port> > ports_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // PHYSICALPORTMAPPINGTABLEMODEL_H
