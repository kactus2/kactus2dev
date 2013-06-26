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

#include <models/generaldeclarations.h>

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QTableView>

class BusPortItem;
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
     *  Sets the opposing and draft interface ports for the dialog.
     *
     *      @param [in] opposingBusPort The bus port item in opposing component.
     *      @param [in] draftBusPort The bus port item in draft component.
     *      @param [in] lh The library interface.
     */
    void setBusPorts(BusPortItem const* opposingBusPort, BusPortItem const* draftBusPort, 
                        LibraryInterface* lh);

    /*!
     *  Creates port mapping in draft component according to generation table contents.
     *
     *      @return The port maps.
    */
    QList< QSharedPointer<General::PortMap> > getPortMaps() const;

    /*!
     *  Creates the list of ports for draft component according to 
     *  generation table contents.
     *
     *      @return The ports.
    */
    QList< QSharedPointer<Port> > getPorts() const;

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

    bool tableEnable_;

    //! Library interface.
    LibraryInterface* lh_;

    QSharedPointer<BusInterface> busIf_;

    BusPortItem const* opposingBusPort_;

    BusPortItem const* draftBusPort_;

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
