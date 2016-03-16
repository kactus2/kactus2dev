//-----------------------------------------------------------------------------
// File: interfaceeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.10.2011
//
// Description:
// Editor to display/edit the details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <editors/ComponentEditor/software/PropertyValueEditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>

class ConnectionEndpoint;
class ComDefinition;

//-----------------------------------------------------------------------------
//! Editor to display/edit the details of a bus interface.
//-----------------------------------------------------------------------------
class InterfaceEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	InterfaceEditor(QWidget *parent, LibraryInterface* handler);
	
	//! The destructor.
	virtual ~InterfaceEditor();

	/*!
     *  Set the interface for the editor.
	 *
	 *      @param [in] interface   Pointer to the interface.
	 */
	void setInterface(ConnectionEndpoint* interface);

public slots:

	/*!
     *  Clear the editor so no interface details are shown
	 */
	void clear();

	/*!
     *  Refresh the editor contents without changing the interface.
	 */
	void refresh();

private slots:

	/*!
     *  Handler for change in interface name.
	 *
	 *      @param [in] newName     The new name of the interface.
	 */
	void onInterfaceNameChanged(const QString& newName);

	/*!
     *  Handler for change in interface mode.
	 *
	 *      @param [in] newMode     The new mode of the interface.
	 */
	void onInterfaceModeChanged(const QString& newMode);

	/*!
     *  Handler for changes in the port map.
	 */
	void onPortMapChanged();

	/*!
     *  Handler for changes in the description editor.
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
	//! No copying.
	InterfaceEditor(const InterfaceEditor& other);

	//! No assignment.
	InterfaceEditor& operator=(const InterfaceEditor& other);

	/*!
     *  Set the port maps from interface_ to mappings_ so they are displayed.
	 */
	void setPortMaps();

	//! Widget to display the bus/API/COM type.
	VLNVDisplayer type_;

	//! Widget to display the abstraction type.
	VLNVDisplayer absType_;

	//! The editor for the interface name.
	QLineEdit nameEdit_;

	//! The label for the name editor.
	QLabel nameLabel_;

	//! The editor for the interface mode.
	QComboBox modeEdit_;

	//! The label for the mode editor.
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

	//! Pointer to the interface being edited.
	ConnectionEndpoint* interface_;

    //! COM definition related to the interface (if COM).
    QSharedPointer<ComDefinition const> comDef_;

	//! Label for the mappings of logical and physical ports
	QLabel mappingsLabel_;

	//! Contains the mapping of ports and logical signals
	QTableWidget mappings_;

	//! Label for the description editor
	QLabel descriptionLabel_;

	//! Editor for the description of the interface.
	QPlainTextEdit descriptionEdit_;

    //! Label for the property value editor.
    QLabel propertyValueLabel_;

    //! Property value editor for SW properties.
    PropertyValueEditor propertyValueEditor_;

    //! Empty dummy widget for adding stretch for API interface view.
    QWidget dummyWidget_;

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // INTERFACEEDITOR_H
