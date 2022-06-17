//-----------------------------------------------------------------------------
// File: BusInterfacePortMapTab.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Tab for editing and viewing bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEPORTMAPTAB_H
#define BUSINTERFACEPORTMAPTAB_H

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortListSortProxyModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PhysicalPortMappingTableView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PhysicalPortMappingTableModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapAutoConnector.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapSortFilter.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QCheckBox>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QComboBox>

#include <QSortFilterProxyModel>

class BusInterface;
class Component;
class LibraryInterface;
class PortMap;
class ExpressionParser;
class VLNV;
class ParameterFinder;
class PortMapDelegate;
class PortMapValidator;
class PortMapAutoConnector;
class AbstractionType;
class PortMapInterface;
class BusInterfaceInterface;
class AbstractionTypeInterface;

//-----------------------------------------------------------------------------
//! Tab for editing and viewing bus interface port maps.
//-----------------------------------------------------------------------------
class BusInterfacePortMapTab : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
     *      @param [in] busInterface        Interface for accessing bus interfaces.
     *      @param [in] busName             Name of the edited bus interface.
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] finder              The used parameter finder.
     *      @param [in] portMapInterface    Interface for accessing port maps.
	 *      @param [in] parent              The owner of the editor.
	 */
    BusInterfacePortMapTab(LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> finder,
        PortMapInterface* portMapInterface,
        QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~BusInterfacePortMapTab() = default;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	virtual void refresh();

	/*!
     *  Set the abstraction type that defines the logical signals to use.
	 *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
	 */
    virtual void setAbsType(int const& abstractionIndex);

    /*!
     *  Sets a subset of component ports to be visible in the physical port list.
     *
     *      @param [in] ports   List of port names to show.
     */
    virtual void setPhysicalPorts(QStringList const& ports);

    /*!
     *  Setup the available abstraction definitions.
     */
    void setAbstractionDefinitions();

public slots:

    /*!
     *  Handle the change in the name of the edited bus interface.
     *
     *      @param [in] newName     New name of the edited bus interface.
     */
    void changeBusName(std::string const& newName);

signals:

    //! Emitted when an error occurs.
    void errorMessage(QString const&);

    //! Emitted when a notice should be sent to user.
    void noticeMessage(QString const&);

	//! Emitted when contents of the model change.
	void contentChanged();

    /*!
     *  Emitted when a help page should be changed in the context help window.
     *
     *      @param [in] url     URL of the help page.
     */
    void helpUrlRequested(QString const& url);

    /*!
     *  Emitted when the selected parameter is referenced. Increases the number of references.
     *
     *      @param [in] id  The ID of the referenced parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Emitted when the selected parameter is referenced. Decreases the number of references.
     *
     *      @param [in] id  The ID of the referenced parameter.
     */
    void decreaseReferences(QString id);

protected:

    //! Handler for widget's show event
    virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Handles the filtering of a connected port.
     *
     *      @param [in] portName    The name of the connected port.
     */
    void onPortConnected(QString const& portName);

    /*!
     *  Handles the filtering of a disconnected port.
     *
     *      @param [in] portName    The name of the disconnected port.
     */
    void onPortDisconnected(QString const& portName);

    /*!
     *  Handles the filtering by the selected direction.
     *
     *      @param [in] newDirection    The selected direction.
     */
    void onDirectionFilterChanged(QString const& newDirection);

    /*!
     *  Handle the selection of currently active the abstraction type.
     */
    void onAbstractionChanged();

    /*!
     *  Handle creation of port maps with required logical signals.
     */
    void onCreateRequiredSignals();

    /*!
     *  Handle creation of port maps with optional logical signals.
     */
    void onCreateOptionalSignals();

    /*!
     *  Handle creation of port maps with all logical signals.
     */
    void onCreateAllSignals();

private:
	
    //! No copying.	No assignment.
	BusInterfacePortMapTab(const BusInterfacePortMapTab& other);
	BusInterfacePortMapTab& operator=(const BusInterfacePortMapTab& other);

    /*!
     *  Setup the currently active abstraction type.
     */
    void setupAbstraction();

	/*!
     *  Set up the layout of the GUI items
     */
	void setupLayout();

    /*!
     *  Setup the port type filter.
     *
     */
    void setupTypeFilter();

    /*!
     *  Add the required items to the direction filter.
     */
    void addItemsToDirectionFilter();

    /*!
     *  Connect the signals.
     */
    void connectItems();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! Name of the edited bus interface.
    std::string busName_;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! The view to display the physical ports.
    PhysicalPortMappingTableView physicalPortView_;

	//! The model to present the physical ports.
    PhysicalPortMappingTableModel physicalPortModel_;

    //! Sorter for the physical ports.
    PortListSortProxyModel physicalPortSorter_;

    //! Editor for filtering of physical ports by name.
    QLineEdit* nameFilterEditor_;

    //! The port type selector.
    QComboBox typeFilter_;

    //! The direction filter.
    QComboBox directionFilter_;

    //! The connected ports filter.
    QCheckBox hideConnectedPortsBox_;

    //! Editor for physical port prefix for auto connect.
    QLineEdit* physicalPrefixEditor_;

    //! Model for the constructed port maps.
    PortMapModel portMapModel_;

    //! Proxy for filtering/sorting port maps.
    PortMapSortFilter portMapSorter_;

    //! View for the constructed port maps.
    PortMapView portMapView_;

    //! Delegate for the constructed port maps.
    PortMapDelegate* portMapDelegate_;

    //! Button for automatically constructing the port maps.
    QPushButton autoConnectButton_;

    //! Button for removing all the port maps.
    QPushButton removeAllMappingsButton_;

    //! Automatic port map creator.
    PortMapAutoConnector autoConnector_;

    //! Selects the active abstraction type.
    QComboBox* abstractionSelector_;
};

#endif // BUSINTERFACEPORTMAPTAB_H
