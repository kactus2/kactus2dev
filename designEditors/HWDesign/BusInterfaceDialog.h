//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#ifndef SELECTINTERFACEMODEDIALOG_H
#define SELECTINTERFACEMODEDIALOG_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QTableView>

class Component;
class ConnectionEndpoint;
class BusInterface;
class CellEditTableView;
class LibraryInterface;
class Port;
class PortGenerationTableModel;

//-----------------------------------------------------------------------------
//! BusInterfaceDialog class.
//-----------------------------------------------------------------------------
class BusInterfaceDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] enableNameEdit  Enables the name editing in the dialog.
     *      @param [in] parent          The parent widget.
     */
    BusInterfaceDialog(bool enableNameEdit, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~BusInterfaceDialog();
    
    
    /*!
     *  Sets the interface name in the dialog.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Adds a mode to the possible alternatives.
     *
     *      @param [in] mode The interface mode to add.
     */
    void addMode(General::InterfaceMode mode);

    /*!
     *  Returns the interface name.
     */
    QString getName() const;

    /*!
     *  Returns the selected mode.
     */
    General::InterfaceMode getSelectedMode() const;

 /*!
     *  Sets the components and interface for copying for the dialog.
     *
     *      @param [in] srcComponent        The component to copy from.
     *      @param [in] busInterface        The bus interface to copy.
     *      @param [in] targetComponent     The component to copy to.
     *      @param [in] handler             The library interface.
     */
    void setBusInterfaces(QSharedPointer<Component> srcComponent, 
                          QSharedPointer<BusInterface> busInterface, 
                          QSharedPointer<Component> targetComponent,
                          LibraryInterface* handler);

    /*!
     *  Creates port mapping in draft component according to generation table contents.
     *
     *      @return The port maps.
    */
    QList< QSharedPointer<PortMap> > getPortMaps();

    /*!
     *  Creates the list of ports for draft component according to 
     *  generation table contents.
     *
     *      @return The ports.
    */
    QList< QSharedPointer<Port> > getPorts();

public slots:

    //! Called when the value of the name field has been changed.
    void onNameChanged();

    //! Called when a mode is selected using a radio button.
    void onModeSelected(bool const radioButtonChecked);

    //! Called when data in port generation table changes.
    void onTableDataChanged();


private:
    // Disable copying.
    BusInterfaceDialog(BusInterfaceDialog const& rhs);
    BusInterfaceDialog& operator=(BusInterfaceDialog const& rhs);

    /*!
     *  Sets dialog layout for interface selection.
     *
     */
    void setupLayout();

    /*!
     *  Creates and configures the port generation table view and model. 
     *  Modifies the dialog layout to show port generation table and 
     *  horizontal mode selection.
     *
     */
    void setupPortTable();

    /*!
     *  Creates and configures the port generation table view and model.
     *
     */
    void setupGeneratedPortsTable();

    /*!
     * Updates port generation table.
     *
     */
    void updatePortsView();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The layout for the UI widgets.
    QVBoxLayout* layout_;

    //! The label for name editing.
    QLabel* nameLabel_;

    //! The line edit for the name.
    QLineEdit* nameEdit_;

    //! The group box for the mode selection.
    QGroupBox* modeGroup_;

    //! The OK button.
    QPushButton* btnOK_;

    //! The Cancel button.
    QPushButton* btnCancel_;

    //! The mode radio buttons.
    QRadioButton* modeRadioButtons_[General::MONITOR + 1];

    //! The group box for the port tables.
    QGroupBox* tableGroup_;

    //! Flag for showing or hiding port table.
    bool tableEnable_;

    //! Library interface.
    LibraryInterface* lh_;

    //! The component from which the bus interface is copied.
    QSharedPointer<Component> sourceComp_;

    //! The component to which the bus interface is copied.
    QSharedPointer<Component> destComp_;

    //! List of generated ports.
    QList< QSharedPointer<Port> > ports_;

    //! List of generated port maps.
    QList< QSharedPointer<PortMap> > portMaps_;

    //! The bus interface the dialog is referring to.
    QSharedPointer<BusInterface> busIf_;

    //! The opposing end of the connection.
    ConnectionEndpoint const* opposingEnd_;

    //! The draft end of the connection.
    ConnectionEndpoint const* draftEnd_;

    //! Table model for port generation table.
    PortGenerationTableModel* portsModel_;

    //! Table view for port generation table.
    CellEditTableView* portsView_;

    //! Sorting proxy for port generation table.
    QSortFilterProxyModel* proxy_;

    //! Number of enabled modes.
    int modes_;
};

//-----------------------------------------------------------------------------

#endif // SELECTINTERFACEMODEDIALOG_H
