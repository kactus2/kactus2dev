/* 
 *  	Created on: 10.10.2011
 *      Author: Antti Kamppi
 * 		filename: interfaceeditor.h
 *		Project: Kactus 2
 */

#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <IPXactWrapper/ComponentEditor/software/PropertyValueEditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>

class ConnectionEndpoint;
class ComDefinition;

/*! \brief Editor to display/edit the details of a bus interface.
 *
 */
class InterfaceEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	InterfaceEditor(QWidget *parent, LibraryInterface* handler);
	
	//! \brief The destructor
	virtual ~InterfaceEditor();

	/*! \brief Set the interface for the editor.
	 *
	 * \param interface Pointer to the interface.
	 *
	*/
	void setInterface(ConnectionEndpoint* interface);

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

    //! Handler for changes in dependency direction.
    void onDependencyDirectionChanged(QString const& newDir);

    //! Handler for changes in COM direction.
    void onComDirectionChanged(QString const& newDir);

    //! Handler for changes in COM transfer type.
    void onComTransferTypeChanged(QString const& newTransferType);

    //! Handler for changes in COM property values.
    void onComPropertyValuesChanged();

private:
	//! \brief No copying
	InterfaceEditor(const InterfaceEditor& other);

	//! \brief No assignment
	InterfaceEditor& operator=(const InterfaceEditor& other);

	/*! \brief Set the port maps from interface_ to mappings_ so they are displayed
	 *
	*/
	void setPortMaps();

	//! \brief Widget to display the bus/API/COM type.
	VLNVDisplayer type_;

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

    //! The label for dependency direction.
    QLabel dependencyDirLabel_;

    //! The editor for dependency direction.
    QComboBox dependencyDirCombo_;

    //! The label for direction editor.
    QLabel comDirectionLabel_;

    //! The editor for direction.
    QComboBox comDirectionCombo_;

    //! Label for transfer type editor.
    QLabel transferTypeLabel_;

    //! Data type editor.
    QComboBox transferTypeCombo_;

	//! \brief Pointer to the interface being edited.
	ConnectionEndpoint* interface_;

    //! COM definition related to the interface (if COM).
    QSharedPointer<ComDefinition const> comDef_;

	//! \brief Label for the mappings of logical and physical ports
	QLabel mappingsLabel_;

	//! \brief Contains the mapping of ports and logical signals
	QTableWidget mappings_;

	//! \brief Label for the description editor
	QLabel descriptionLabel_;

	//! \brief Editor for the description of the interface.
	QPlainTextEdit descriptionEdit_;

    //! Label for the property value editor.
    QLabel propertyValueLabel_;

    //! Property value editor for SW properties.
    PropertyValueEditor propertyValueEditor_;

    //! Empty dummy widget for adding stretch for API interface view.
    QWidget dummyWidget_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // INTERFACEEDITOR_H
