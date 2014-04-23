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
class VLNV;
//-----------------------------------------------------------------------------
//! Bus editor page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardBusEditorPage : public QWizardPage
{
    Q_OBJECT
public:

    //! Selection for signal naming source.
    enum SignalNamingPolicy
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
    /*!
     *  The constructor.
     *
     *      @param [in] component       The component whose bus interface is being edited.
     *      @param [in] busIf           The bus interface being edited.
     *      @param [in] lh              The component library handler.
     *      @param [in] physicalPorts   Physical ports used in logical signal generation.
     *      @param [in] parent          The parent wizard.
     *      @param [in] absDefVLNV      VLNV of the editable abstraction defintion.
     *      @param [in] namingPolicy    Policy for naming generated logical signals.
     */
    BusInterfaceWizardBusEditorPage(QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busIf, LibraryInterface* lh, 
        QStringList physicalPorts,
        BusInterfaceWizard* parent, VLNV& absDefVLNV, SignalNamingPolicy namingPolicy = NAME);

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

public slots:

    //! Handler for port abstraction renaming.
    virtual void portRenamed(QString const& oldName, QString const& newName);

    //! Handler for port abstraction removal.
    virtual void portRemoved(QString const& portName, const General::InterfaceMode mode);

private:
    // Disable copying.
    BusInterfaceWizardBusEditorPage(BusInterfaceWizardBusEditorPage const& rhs);
    BusInterfaceWizardBusEditorPage& operator=(BusInterfaceWizardBusEditorPage const& rhs);

    /*!
    *  Sets the page layout.
    */
    void setupLayout();

    /*!
     *  Creates the logical ports to the abstraction definition and initial mapping of physical ports
     *  to the created logical ports. 
     *
     *      @param [in] physPorts   The physical ports from which the logical ports are generated. 
     *      @param [in] absDef      The abstraction definition to create the ports to. 
     *                              Previous ports will be overwritten.
     */
    void createLogicalPortsAndMappings(QStringList const& physPorts, QSharedPointer<AbstractionDefinition> absDef);

    /*!
     *  Creates initial mapping of physical ports to a single logical port.
     *
     *      @param [in] physPorts       The physical ports to map.
     *      @param [in] logicalPort     The logical port to map to.
     *      @param [in] logicalPort     The direction of the logical port.
     */
    void createLogicalMappings(QStringList const& physPorts, QString const& logicalPort, 
        General::Direction logicalDirection);

    /*!
     *  Creates port maps to the bus interface based on the initial mapping.     
     */
    void createPortMaps();

    /*!
     *  Finds a logical port by name.
     *
     *      @param [in] portName    The name of the port to find.
     *      @param [in] ports       The group of ports to search.
     *
     *      @return The searched port or 0 if port was not found in the group.
     */
    QSharedPointer<PortAbstraction> findPortByName(QString const& portName, 
        const QList<QSharedPointer<PortAbstraction> >& ports);


    /*!
     *  Creates a port abstraction.
     *
     *      @param [in] portName        Name of the port.
     *      @param [in] portDirection   Direction of the port.
     *      @param [in] portWidth       Width of the port.
     *
     *      @return The created port.
     */
    QSharedPointer<PortAbstraction> createAbsPort(QString const& portName, General::Direction portDirection, int portWidth );

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The abstraction definition VLNV.
    VLNV absDefVLNV_;

    //! The library handler.
    LibraryInterface* handler_;

    QStringList physicalPorts_;

    //! The editor for bus and abstraction definition.
    BusEditor editor_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The source for logical signal names.
    SignalNamingPolicy portNamesPolicy_;

    //! Mapping for logic signal generation. Physical port names as keys and generated abstract signals as values.
    QMap<QString, QString > portMappings_;

    //! Flag for indicating changes on the page. 
    bool hasChanged_;

    //! Enumeration for port mapping generation.
    enum PortMapGenerationMode
    {
        NO_GENERATION = 0,
        GENERATE_SINGLE,
        GENERATE_ALL
    };

    //! The active mode for generation.
    PortMapGenerationMode mappingMode_;
};

#endif // BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H
