//-----------------------------------------------------------------------------
// File: BusInterfaceWizardBusDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for editing bus and abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H
#define BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H

#include <QWizardPage>

#include <QMap>
#include <QPushButton>

#include <IPXactWrapper/BusEditor/buseditor.h>
#include <models/businterface.h>

class BusInterfaceWizard;
class LibraryInterface;
class BusDefinition;
class AbstractionDefinition;
class PortAbstraction;
class Component;

//-----------------------------------------------------------------------------
//! Bus editor page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardBusEditorPage : public QWizardPage
{
    Q_OBJECT
public:

    //! Selection for signal naming source.
    enum SignalNaming
    {
        NAME = 0,
        DESCRIPTION
    };

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component whose bus interface is being edited.
     *      @param [in] busIf       The bus interface being edited.
     *      @param [in] lh          The component library handler.
     *      @param [in] parent      The parent wizard.
     */
    BusInterfaceWizardBusEditorPage(QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busIf, LibraryInterface* lh, BusInterfaceWizard* parent);

    /*!
    *  Destructor.
    */
    ~BusInterfaceWizardBusEditorPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    virtual void initializePage();

    /*!
    *  Validates the page.
    */
    virtual bool validatePage();

    
    /*!
     *  Sets the source for logical signal naming.
     *
     *      @param [in] type   The type to set.         
     */
    void setSignalNaming(SignalNaming type);

public slots:

    //! Handler for port abstraction renaming.
    virtual void portRenamed(const QString& oldName, const QString& newName);

    //! Handler for port abstraction removal.
    virtual void portRemoved(const QString& portName, const General::InterfaceMode mode);

private:
    // Disable copying.
    BusInterfaceWizardBusEditorPage(BusInterfaceWizardBusEditorPage const& rhs);
    BusInterfaceWizardBusEditorPage& operator=(BusInterfaceWizardBusEditorPage const& rhs);

    /*!
    *  Sets the page layout.
    */
    void setupLayout();


    /*!
     *  Creates the logical ports to the abstraction definition.
     *
     *      @param [in] absDef   <Argument description>.
     */
    void createLogicalPorts(QSharedPointer<AbstractionDefinition> absDef);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent wizard.
    BusInterfaceWizard* parent_;

    //! The library handler.
    LibraryInterface* handler_;

    //! The editor for bus and abstraction definition.
    BusEditor editor_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The source for logical signal names.
    SignalNaming portNamesType_;

    // Mapping for logic signal generation. Physical port names as keys and generated abstract signals as values.
    QMap<QString, QString > portMappings_;

    bool hasChanged_;
};

#endif // BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H
