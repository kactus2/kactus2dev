/* 
 *  	Created on: 14.10.2011
 *      Author: Antti Kamppi
 * 		filename: connectioneditor.h
 *		Project: Kactus 2
 */

#ifndef CONNECTIONEDITOR_H
#define CONNECTIONEDITOR_H

#include "AdHocBoundsModel.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QString>
#include <QSharedPointer>

class GraphicsConnection;
class LibraryInterface;

/*! \brief Editor to display/edit details of a connection.
 *
 */
class ConnectionEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this editor.
	 * \param handler Pointer to the instance that manages the library
	 *
	*/
	ConnectionEditor(QWidget *parent, LibraryInterface* handler);
	
	//! \brief The destructor
	virtual ~ConnectionEditor();

public slots:

	/*! \brief Set the connection to be edited.
	 *
	 * \param connection Pointer to the connection to edit.
	 *
	*/
	void setConnection(GraphicsConnection* connection);

	/*! \brief Clear the editor so no connection details are shown
	 *
	*/
	void clear();

	/*! \brief Refresh the editor contents without changing the connection.
	 *
	*/
	void refresh();

private slots:

	/*! \brief Handler for name changes for connection.
	 *
	 * \param name The name that was set.
	 *
	*/
	void onNameChanged(const QString& name);

	/*! \brief Handler for description changes for connection.
	 *
	*/
	void onDescriptionChanged();

private:
	//! \brief No copying
	ConnectionEditor(const ConnectionEditor& other);

	//! \brief No assignment
	ConnectionEditor& operator=(const ConnectionEditor& other);

	/*! \brief Set port maps so that editor displays the connected physical ports.
	 *
	*/
	void setPortMaps();

	/*! \brief Add a mapping for physical ports to port widget for two given port maps.
	 *
	 * \param row The row to insert the physical map to.
	 * \param invalid Defines if this map is already been declared as invalid.
	 * \param portMap1 Pointer to the first port map.
	 * \param component1 Pointer to the component that contains the port map1.
	 * \param portMap2 Pointer to the second port map.
	 * \param component2 Pointer to the component that contains the port map2.
	 *
	*/
	void addMap(int& row, bool invalid, 
		QSharedPointer<PortMap> portMap1,
		QSharedPointer<Component> component1,
		QSharedPointer<PortMap> portMap2,
		QSharedPointer<Component> component2);

	//! \brief Widget to display the connection type (COM/API/bus type).
	VLNVDisplayer type_;

	//! \brief Widget to display the abstraction type.
	VLNVDisplayer absType_;

	//! \brief Label for connected instances.
	QLabel instanceLabel_;

	//! \brief Label to display the connected interfaces.
	QLabel connectedInstances_;

	//! \brief Used to separate the connected interfaces from name edit
	QGroupBox separator_;

	//! \brief Label for connection name.
	QLabel nameLabel_;

	//! \brief Editor to set the name of the interconnection.
	QLineEdit nameEdit_;

	//! \brief Label for description.
	QLabel descriptionLabel_;

	//! \brief Editor for description.
	QPlainTextEdit descriptionEdit_;

	//! \brief Label for port widget.
	QLabel portsLabel_;

	//! \brief Widget to display the port connections.
	QTableWidget portWidget_;

    //! Dummy widget to be used when there is no ports or ad-hoc bounds to show.
    QWidget dummyWidget_;

	//! \brief Pointer to the connection being edited.
	GraphicsConnection* connection_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* handler_;

    //! The port ad-hoc bounds table.
    QTableView adHocBoundsTable_;

    //! The ad-hoc bounds model.
    AdHocBoundsModel adHocBoundsModel_;
};

#endif // CONNECTIONEDITOR_H
