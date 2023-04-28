//-----------------------------------------------------------------------------
// File: connectioneditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.10.2011
//
// Description:
// Editor to display/edit details of a connection.
//-----------------------------------------------------------------------------

#ifndef CONNECTIONEDITOR_H
#define CONNECTIONEDITOR_H

#include "AdHocBoundsModel.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QString>
#include <QSharedPointer>
#include <QStackedWidget>

class DesignDiagram;
class ExpressionParser;
class GraphicsConnection;
class LibraryInterface;
class BusInterface;
class Component;
class ConnectionEndpoint;
class ComponentInstance;

//-----------------------------------------------------------------------------
//! Editor to display/edit details of a connection.
//-----------------------------------------------------------------------------
class ConnectionEditor : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] library     The instance that manages the library
     *      @param [in] parent      The owner of this editor.
	 *
	*/
	ConnectionEditor(LibraryInterface* library, QWidget* parent);
	
	//! The destructor
	virtual ~ConnectionEditor() = default;

public slots:

	/*! Set the connection to be edited.
	 *
	 *      @param [in] connection The connection to edit.
	 *
	*/
	void setConnection(GraphicsConnection* connection, DesignDiagram* diagram);

	/*! Clear the editor so no connection details are shown
	 *
	*/
	void clear();

	/*! Refresh the editor contents without changing the connection.
	 *
	*/
	void refresh();

private slots:

    //! Called when name or description is edited.
	void onNameOrDescriptionChanged();

private:
	//! No copying
	ConnectionEditor(const ConnectionEditor& other);

	//! No assignment
	ConnectionEditor& operator=(const ConnectionEditor& other);

    /*!
     *  Finds the VLNV for the bus abstraction.
     *
     *      @param [in] endPoint        The selected bus interface end point.
     *      @param [in] busInterface    The bus interface whose abstraction definition VLNV to find.
     *
     *      @return The VLNV for the given bus interface abstraction definition.
     */
    VLNV findAbstractionVLNV(ConnectionEndpoint* endPoint, QSharedPointer<BusInterface> busInterface) const;

	//! Set port maps so that editor displays the connected physical ports.
	void setPortMaps();

    //! Sets the headers for the port connection table.
    void setTableHeaders();

    /*!
     *  Finds the names of all logical ports mapped in the given port maps.
     *
     *      @param [in] portMaps1   Port maps for first interface.
     *      @param [in] portMaps2   Port maps for second interface.
     *
     *      @return The names of all logical ports.
     */
    QStringList getAllLogicalPorts(QList<QSharedPointer<PortMap> > const& portMaps1, 
        QList<QSharedPointer<PortMap> > const& portMaps2) const;

	/*! Add a mapping for physical ports to port widget for two given port maps.
	 *
	 *      @param [in/out] row             The row to insert the physical map to.
	 *      @param [in]     invalid         Defines if this map is already been declared as invalid.
	 *      @param [in]     portMap1        The first port map.
	 *      @param [in]     component1      The component that contains the port map1.
	 *      @param [in]     portMap2        The second port map.
	 *      @param [in]     component2      The component that contains the port map2.
	 *
	 */
	void addMap(QSharedPointer<PortMap> portMap1, QSharedPointer<Component> component1,
        QSharedPointer<PortMap> portMap2, QSharedPointer<Component> component2, 
        QSharedPointer<ExpressionParser> expressionParser, bool validAbstraction);

    /*!
     *  Creates an item for the port connectivity table.
     *
     *      @param [in] portName    The name of the port.
     *      @param [in] left        The left bound of the port.
     *      @param [in] right       The right bound of the port.
     *      @param [in] isValid     Is the port connectivity valid or not.
     *
     *      @return Item for the port connectivity table representing the given port.
     */
    QTableWidgetItem* createPortItem(QString const& portName, int left, int right, bool isValid);

    /*!
     *  Calculate mapped physical port bounds.
     *
     *      @param [in] parser                  The used expression parser.
     *      @param [in] containingPortMap       The port map containing the mapped physical port.
     *      @param [in] containingComponent     The component containing the physical port.
     *
     *      @return Integer pair, where the first is the left bound, and the second is the right bound.
     */
    QPair<int, int> calculateMappedPhysicalPortBounds(QSharedPointer<ExpressionParser> parser,
        QSharedPointer<PortMap> containingPortMap, QSharedPointer<Component> containingComponent);

    /*!
     *  Check if the mapped physical port is not valid.
     *
     *      @param [in] invalid                 The current validity of the connection.
     *      @param [in] containingPortMap       Port map containing the physical port.
     *      @param [in] containingComponent     Component containing the referenced physical port.
     *
     *      @return True, if the physical port is not valid, otherwise false.
     */
    bool isValidPhysicalPort (QSharedPointer<PortMap> containingPortMap,
        QSharedPointer<Component> containingComponent);

    /*!
     *  Calculate the mapped logical port bounds.
     *
     *      @param [in] parser              The used expression parser.
     *      @param [in] containingPortMap   The port map containing the logical port.
     *
     *      @return Integer pair, where the first value is the higher bound and the second value is the lower bound.
     */
    QPair<int, int> calculateMappedLogicalPortBounds(QSharedPointer<ExpressionParser> parser,
        QSharedPointer<PortMap> containingPortMap);
    
    /*!
     *  Sets the editor layout.
     */
    void setupLayout();

    /*!
     *  Get the port maps for the selected bus interface end point.
     *
     *      @param [in] endPoint        The selected end point.
     *      @param [in] busInterface    The bus interface contained within the selected end point.
     *
     *      @return A list of port maps contained within the selected end point
     */
    QList<QSharedPointer<PortMap> > getPortMapsForEndPoint(ConnectionEndpoint* endPoint,
        QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Get the active view for the item containing the selected end point item.
     *
     *      @param [in] endPoint    The selected end point.
     *
     *      @return The name of the active view containing the selected end point item.
     */
    QString getActiveViewForEndPoint(ConnectionEndpoint* endPoint) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Widget to display the connection type (COM/API/bus type).
	VLNVDisplayer type_;

	//! Widget to display the abstraction type.
	VLNVDisplayer absType_;

	//! Label for connected instances.
	QLabel instanceLabel_;

	//! Label to display the connected interfaces.
	QLabel connectedInstances_;

	//! Used to separate the connected interfaces from name edit
	QGroupBox nameGroup_;

	//! Label for connection name.
	QLabel nameLabel_;

	//! Editor to set the name of the interconnection.
	QLineEdit nameEdit_;

	//! Label for description.
	QLabel descriptionLabel_;

	//! Editor for description.
	QPlainTextEdit descriptionEdit_;

	//! Label for port widget.
	QLabel portsLabel_;

	//! Widget to display the port connections.
	QTableWidget portWidget_;

	//! The connection being edited.
	GraphicsConnection* connection_;

    //! The diagram containing the connection.
    DesignDiagram* diagram_;

	//! The instance that manages the library
	LibraryInterface* library_;

    //! The port ad-hoc bounds table.
    QTableView adHocBoundsTable_;

    //! The ad-hoc bounds model.
    AdHocBoundsModel adHocBoundsModel_;

    //! Stack containing the connection types.
    QStackedWidget* connectionTypeTable_;
};

#endif // CONNECTIONEDITOR_H
