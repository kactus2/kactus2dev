/* 
 *  	Created on: 10.10.2011
 *      Author: Antti Kamppi
 * 		filename: interfaceeditor.h
 *		Project: Kactus 2
 */

#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>

class DiagramConnectionEndPoint;

class InterfaceEditor : public QWidget {
	Q_OBJECT

public:

	InterfaceEditor(QWidget *parent);
	
	//! \brief The destructor
	virtual ~InterfaceEditor();

	/*! \brief Set the interface for the editor.
	 *
	 * \param interface Pointer to the interface.
	 *
	*/
	void setInterface(DiagramConnectionEndPoint* interface);

public slots:

	/*! \brief Clear the editor so no interface details are shown
	 *
	*/
	void clear();

	/*! \brief Refresh the editor contents without changing the interface.
	 *
	*/
	void refresh();

private slots:

	/*! \brief Handler for change in interface name.
	 *
	 * \param newName The new name of the interface.
	 *
	*/
	void onInterfaceNameChanged(const QString& newName);

	/*! \brief Handler for change in interface mode.
	 *
	 * \param newMode The new mode of the interface.
	 *
	*/
	void onInterfaceModeChanged(const QString& newMode);

	/*! \brief Handler for changes in the port map.
	 *
	*/
	void onPortMapChanged();

	/*! \brief Handler for changes in the description editor.
	 *
	*/
	void onDescriptionChanged();

signals:

	//! \brief Emitted when contents of the editor changes.
	//void contentChanged();

private:
	//! \brief No copying
	InterfaceEditor(const InterfaceEditor& other);

	//! \brief No assignment
	InterfaceEditor& operator=(const InterfaceEditor& other);

	/*! \brief Set the port maps from interface_ to mappings_ so they are displayed
	 *
	*/
	void setPortMaps();

	//! \brief Widget to display the bus type.
	VLNVDisplayer busType_;

	//! \brief Widget to display the abstraction type.
	VLNVDisplayer absType_;

	//! \brief The editor for the interface name.
	QLineEdit nameEdit_;

	//! \brief The label for the name editor
	QLabel nameLabel_;

	//! \brief The editor for the interface mode.
	QComboBox modeEdit_;

	//! \brief The label for the mode editor
	QLabel modeLabel_;

	//! \brief Pointer to the interface being edited.
	DiagramConnectionEndPoint* interface_;

	//! \brief Label for the mappings of logical and physical ports
	QLabel mappingsLabel_;

	//! \brief Contains the mapping of ports and logical signals
	QTableWidget mappings_;

	//! \brief Label for the description editor
	QLabel descriptionLabel_;

	//! \brief Editor for the description of the interface.
	QPlainTextEdit descriptionEdit_;

};

#endif // INTERFACEEDITOR_H
