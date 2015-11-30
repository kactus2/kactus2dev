//-----------------------------------------------------------------------------
// File: connectioneditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

class DesignDiagram;
class GraphicsConnection;
class LibraryInterface;
class Component;

//-----------------------------------------------------------------------------
//! Editor to display/edit details of a connection.
//-----------------------------------------------------------------------------
class ConnectionEditor : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The owner of this editor.
	 *      @param [in] handler The instance that manages the library
	 *
	*/
	ConnectionEditor(QWidget *parent, LibraryInterface* handler);
	
	//! The destructor
	virtual ~ConnectionEditor();

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


	void onNameOrDescriptionChanged();

private:
	//! No copying
	ConnectionEditor(const ConnectionEditor& other);

	//! No assignment
	ConnectionEditor& operator=(const ConnectionEditor& other);

	/*! Set port maps so that editor displays the connected physical ports.
	 *
	*/
	void setPortMaps();

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
	void addMap(int& row, bool invalid, 
		QSharedPointer<PortMap> portMap1,
		QSharedPointer<Component> component1,
		QSharedPointer<PortMap> portMap2,
		QSharedPointer<Component> component2);

	//! Widget to display the connection type (COM/API/bus type).
	VLNVDisplayer type_;

	//! Widget to display the abstraction type.
	VLNVDisplayer absType_;

	//! Label for connected instances.
	QLabel instanceLabel_;

	//! Label to display the connected interfaces.
	QLabel connectedInstances_;

	//! Used to separate the connected interfaces from name edit
	QGroupBox separator_;

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

    DesignDiagram* diagram_;

	//! The instance that manages the library
	LibraryInterface* handler_;

    //! The port ad-hoc bounds table.
    QTableView adHocBoundsTable_;

    //! The ad-hoc bounds model.
    AdHocBoundsModel adHocBoundsModel_;
};

#endif // CONNECTIONEDITOR_H
