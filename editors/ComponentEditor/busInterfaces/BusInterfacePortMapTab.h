//-----------------------------------------------------------------------------
// File: BusInterfacePortMapTab.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Tab for editing and viewing bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEPORTMAPTAB_H
#define BUSINTERFACEPORTMAPTAB_H

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapTreeView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapTreeModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortListSortProxyModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PhysicalPortMappingTableView.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PhysicalPortMappingTableModel.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapAutoConnector.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QCheckBox>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class BusInterface;
class Component;
class LibraryInterface;
class PortMap;
class ExpressionParser;
class VLNV;
class ParameterFinder;
class PortMapTreeDelegate;
class PortMapValidator;
class PortMapAutoConnector;

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
	 *      @param [in] busif               The bus interface being edited.
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] formatter           The used expression formatter.
     *      @param [in] finder              The used parameter finder.
     *      @param [in] portMapValidator    Validator used for port maps.
	 *      @param [in] parent              The owner of the editor.
	 */
	BusInterfacePortMapTab(LibraryInterface* libHandler, QSharedPointer<Component> component, 
        QSharedPointer<BusInterface> busif,	QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> formatter, QSharedPointer<ParameterFinder> finder,
        QSharedPointer<PortMapValidator> portMapValidator, QWidget* parent);
	
	/*!
     *  The destructor.
     */
    virtual ~BusInterfacePortMapTab();

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	virtual void refresh();

	/*!
     *  Set the abstraction type that defines the logical signals to use.
	 *
	 *      @param [in] vlnv    VLNV of the new abstraction type.
     *      @param [in] mode    The interface mode of the bus interface.
	 */
	virtual void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

    /*!
     *  Sets a subset of component ports to be visible in the physical port list.
     *
     *      @param [in] ports   List of port names to show.
     */
    virtual void setPhysicalPorts(QStringList const& ports);

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

private:
	
    //! No copying.	No assignment.
	BusInterfacePortMapTab(const BusInterfacePortMapTab& other);
	BusInterfacePortMapTab& operator=(const BusInterfacePortMapTab& other);

	/*!
     *  Set up the layout of the GUI items
     */
	void setupLayout();

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

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busif_;

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

    //! The direction filter.
    QComboBox directionFilter_;

    //! The connected ports filter.
    QCheckBox hideConnectedPortsBox_;

    //! Model for the constructed port maps.
    PortMapTreeModel portMapsModel_;

    //! View for the constructed port maps.
    PortMapTreeView portMapsView_;

    //! Delegate for the constructed port maps.
    PortMapTreeDelegate* portMapsDelegate_;

    //! Button for automatically constructing the port maps.
    QPushButton autoConnectButton_;

    //! Button for removing all the port maps.
    QPushButton removeAllMappingsButton_;

    //! Automatic port map creator.
    PortMapAutoConnector autoConnector_;
};

#endif // BUSINTERFACEPORTMAPTAB_H
