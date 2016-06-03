//-----------------------------------------------------------------------------
// File: BusInterfaceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.12.2015
//
// Description:
// Validator for the ipxact:busInterface.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEVALIDATOR_H
#define BUSINTERFACEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QSharedPointer>
#include <QString>

class AbstractionType;
class AddressSpace;
class Choice;
class ExpressionParser;
class FileSet;
class LibraryInterface;
class MasterInterface;
class MemoryMap;
class Port;
class RemapState;
class View;
class PortMapValidator;
class AbstractionDefinition;
class PortAbstraction;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:busInterface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusInterfaceValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The parse to use for solving expressions.
     *      @param [in] choices             List of available choices.
     *      @param [in] views               List of available views.
     *      @param [in] ports               List of available ports.
     *      @param [in] addressSpaces       List of available address spaces.
     *      @param [in] memoryMaps          List of available memory maps.
     *      @param [in] busInterfaces       List of available bus interfaces.
     *      @param [in] fileSets            List of available file sets.
     *      @param [in] remapStates         List of available remap states.
     *      @param [in] portMapValidator    Validator used for port maps.
     *      @param [in] parameterValidator  Validator used for parameters.
     *      @param [in] libraryHandler      The library interface.
     */
    BusInterfaceValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<QList<QSharedPointer<View> > > views,
        QSharedPointer<QList<QSharedPointer<Port> > > ports,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces,
        QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
        QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,
        QSharedPointer<PortMapValidator> portMapValidator,
        QSharedPointer<ParameterValidator2014> parameterValidator,
        LibraryInterface* libraryHandler);

	//! The destructor.
	virtual ~BusInterfaceValidator();
    
    /*!
     *  Change the available component elements.
     *
     *      @param [in] newChoices          The new choices.
     *      @param [in] newViews            The new views.
     *      @param [in] newPorts            The new ports.
     *      @param [in] newAddressSpaces    The new address spaces.
     *      @param [in] newMemoryMaps       The new memory maps.
     *      @param [in] newBusInterfaces    The new bus interfaces.
     *      @param [in] newFileSets         The new file sets.
     *      @param [in] newRemapStates      The new remap states.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices,
        QSharedPointer<QList<QSharedPointer<View> > > newViews,
        QSharedPointer<QList<QSharedPointer<Port> > > newPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > newMemoryMaps,
        QSharedPointer<QList<QSharedPointer<BusInterface> > > newBusInterfaces,
        QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets,
        QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates);

    /*!
     *  Get the used port map validator.
     *
     *      @return The used port map validator.
     */
    QSharedPointer<PortMapValidator> getPortMapValidator();

    /*!
     *  Validates the given bus interface.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the bus interface is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] isPresent   The selected is present value.
     *
     *      @return True, if the is present is valid, otherwise false.
     */
    bool hasValidIsPresent(QString const& isPresent) const;

    /*!
     *  Check if the bus type is valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the bus type is valid, otherwise false.
     */
    bool hasValidBusType(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the abstraction types are valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the abstraction types are valid, otherwise false.
     */
    bool hasValidAbstractionTypes(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the bus interface mode is valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the bus interface mode is valid, otherwise false.
     */
    bool hasValidInterfaceMode(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the bits in LAU value is valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the bits in LAU value is valid, otherwise false.
     */
    bool hasValidBitsInLAU(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the bit steering is valid.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the bit steering is valid, otherwise false.
     */
    bool hasValidBitSteering(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if the bus interface contains valid parameters.
     *
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Locate errors within a bus interface.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected busInterface.
     *      @param [in] context         Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

private:

	// Disable copying.
	BusInterfaceValidator(BusInterfaceValidator const& rhs);
	BusInterfaceValidator& operator=(BusInterfaceValidator const& rhs);

    /*!
     *  Check if an abstraction type has valid abstraction reference.
     *
     *      @param [in] abstraction     The selected abstraction type.
     *
     *      @return True, if the abstraction reference is valid, otherwise false.
     */
    bool abstractionTypeHasValidAbstractionReference(QSharedPointer<AbstractionType> abstraction) const;

    /*!
     *  Check if an abstraction type has valid view reference.
     *
     *      @param [in] abstraction     The selected abstraction type.
     *
     *      @return True, if the referenced view is valid, otherwise false.
     */
    bool abstractionTypeHasValidViewReference(QSharedPointer<AbstractionType> abstraction) const;

    /*!
     *  Check if an abstraction type has valid port maps.
     *
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] abstraction     The selected abstraction type.
     *
     *      @return True, if the port maps are valid, otherwise false.
     */
    bool abstractionTypeHasValidPortMaps(QSharedPointer<BusInterface> busInterface,
        QSharedPointer<AbstractionType> abstraction) const;

    /*!
     *  Check if the master interface is valid.
     *
     *      @param [in] master  The selected master interface.
     *
     *      @return True, if the master interface is valid, otherwise false.
     */
    bool hasValidMasterInterface(QSharedPointer<MasterInterface> master) const;

    /*!
     *  Check if the slave interface is valid.
     *
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] slave           The selected slave interface.
     *
     *      @return True, if the slave interface is valid, otherwise false.
     */
    bool hasValidSlaveInterface(QSharedPointer<BusInterface> busInterface, QSharedPointer<SlaveInterface> slave)
        const;

    /*!
     *  Check if the slave interface has a valid memory map reference.
     *
     *      @param [in] busInterface    The containing bus interface
     *      @param [in] slave           The selected slave interface.
     *
     *      @return True, if the memory map reference is valid, otherwise false.
     */
    bool slaveInterfaceHasValidMemoryMapRef(QSharedPointer<BusInterface> busInterface,
        QSharedPointer<SlaveInterface> slave) const;

    /*!
     *  Check if the slave interface has valid bridges.
     *
     *      @param [in] slave   The selected slave interface.
     *
     *      @return True, if the bridges are valid, otherwise false.
     */
    bool slaveInterfaceHasValidBridges(QSharedPointer<SlaveInterface> slave) const;

    /*!
     *  Check if a slave interface bridge have a valid master interface reference.
     *
     *      @param [in] bridge  The selected bridge.
     *
     *      @return True, if the master interface reference is valid, otherwise false.
     */
    bool slaveBridgeReferencesValidMaster(QSharedPointer<SlaveInterface::Bridge> bridge) const;

    /*!
     *  Check if the slave interface file set reference group is valid.
     *
     *      @param [in] slave   The selected slave interface.
     *
     *      @return True, if the slave interface file set reference group is valid, otherwise false.
     */
    bool slaveInterfaceFileSetRefGroupsAreValid(QSharedPointer<SlaveInterface> slave) const;

    /*!
     *  Check if a file set reference is valid.
     *
     *      @param [in] fileSet reference   The selected file set reference.
     *
     *      @return True, if the file set reference is valid, otherwise false.
     */
    bool slaveFileSetReferenceIsValid(QString const& fileSetReference) const;

    /*!
     *  Check if the system interface is valid.
     *
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] systemGroup     The selected system group
     *
     *      @return True, if the system interface is valid, otherwise false.
     */
    bool hasValidSystemInterface(QSharedPointer<BusInterface> busInterface, QString const& systemGroup) const;

    /*!
     *  Check if the mirrored slave interface is valid.
     *
     *      @param [in] mirroredSlave   The selected mirrored slave interface.
     *
     *      @return True, if the mirrored slave interface is valid, otherwise false.
     */
    bool hasValidMirroredSlaveInterface(QSharedPointer<MirroredSlaveInterface> mirroredSlave) const;

    /*!
     *  Check if the mirrored slave interface range is valid.
     *
     *      @param [in] mirroredSlave   The selected mirrored slave interface.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool mirroredSlaveRangeIsValid(QSharedPointer<MirroredSlaveInterface> mirroredSlave) const;

    /*!
     *  Check if the mirrored slave interface remap address is valid.
     *
     *      @param [in] remapAddress    The selected remap address.
     *
     *      @return True, if the remap address is valid, otherwise false.
     */
    bool mirroredSlaveRemapAddressIsValid(QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const;

    /*!
     *  Check if the mirrored slave interface remap address remap state is valid.
     *
     *      @param [in] remapAddress    The selected mirrored slave interface remap address.
     *
     *      @return True, if the mirrored slave interface remap address is valid, otherwise false.
     */
    bool mirroredSlaveStateIsValid(QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const;

    /*!
     *  Check if the monitor interface is valid.
     *
     *      @param [in] busInterface        The containing bus interface.
     *      @param [in] monitorInterface    The selected monitor interface.
     *
     *      @return True, if the monitor interface is valid, otherwise false.
     */
    bool hasValidMonitorInterface(QSharedPointer<BusInterface> busInterface,
        QSharedPointer<BusInterface::MonitorInterface> monitorInterface) const;

    /*!
     *  Check if the interface reference has a valid presence.
     *
     *      @param [in] interfacePresence   The presence of the original interface.
     *      @param [in] targetPresence      The presence of the target interface.
     *
     *      @return True, if the interface reference has a valid presence, otherwise false.
     */
    bool interfaceReferenceHasValidPresence(QString const& interfacePresence, QString const& targetPresence) const;

    /*!
     *  Find errors within name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within bus type.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInBusType(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within parameters.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within abstraction types.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInAbstractionTypes(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within abstraction type abstraction reference.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInAbstractionReference(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
        QString const& context) const;

    /*!
     *  Find errors within abstraction type port maps.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInAbstractionPortMaps(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QSharedPointer<AbstractionType> abstraction, QString const& context) const;

    /*!
     *  Find errors within interface mode.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] busInterface            The selected bus interface.
     *      @param [in] busInterfaceContext     Context information of the bus interface
     *      @param [in] containingContext       Context to help locate the error.
     */
    void findErrorsInInterfaceMode(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& busInterfaceContext, QString const& containingContext) const;

    /*!
     *  Find errors within master interface.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] master      The selected master interface.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInMasterInterface(QVector<QString>& errors, QSharedPointer<MasterInterface> master,
        QString const& context) const;

    /*!
     *  Find errors within slave interface.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] slave           The selected slave interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInSlaveInterface(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QSharedPointer<SlaveInterface> slave, QString const& context) const;

    /*!
     *  Find errors within system mode.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] systemGroup     The selected system group.
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInSystemInterface(QVector<QString>& errors, QString const& systemGroup,
        QSharedPointer<BusInterface> busInterface, QString const& context) const;

    /*!
     *  Find errors within mirrored slave interface.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] mirroredSlave   The selected mirrored slave interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInMirroredSlaveInterface(QVector<QString>& errors,
        QSharedPointer<MirroredSlaveInterface> mirroredSlave, QString const& context) const;

    /*!
     *  Find errors within monitor interface.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] monitor     The selected monitor interface.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInMonitorInterface(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QSharedPointer<BusInterface::MonitorInterface> monitor, QString const& context) const;

    /*!
     *  Find errors within bis in LAU.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInBitsInLAU(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Find errors within bit steering.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] busInterface    The selected bus interface.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInBitSteering(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
        QString const& context) const;

    /*!
     *  Get the abstraction definition from the selected reference.
     *
     *      @param [in] abstractionReference    The selected abstraction reference.
     *
     *      @return Abstraction definition matching the abstraction reference.
     */
    QSharedPointer<AbstractionDefinition const> getAbstractionDefinition(
        QSharedPointer<ConfigurableVLNVReference> abstractionReference) const;

    /*!
     *  Get the logical port.
     *
     *      @param [in] portName                The name of the logical port.
     *      @param [in] abstractionDefinition   Abstraction definition containing the logical port.
     *
     *      @return The selected logical port.
     */
    QSharedPointer<PortAbstraction> getLogicalPort(QString const& portName,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition) const;

    /*!
     *  Check if the logical port has a valid presence.
     *
     *      @param [in] abstractionDefinition   The abstraction definition containing the logical port.
     *      @param [in] logicalPortName         The name of the logical port.
     *      @param [in] mode                    Interface mode used in the containing bus interface.
     *
     *      @return True, if the logical port has a valid presence, otherwise false.
     */
    bool logicalPortHasValidPresence(QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QString const& logicalPortName, General::InterfaceMode mode) const;

    /*!
     *  Check if the required logical ports have port maps.
     *
     *      @param [in] busInterface            Bus interface containing the port maps.
     *      @param [in] abstractionDefinition   Abstraction definition containing the logical port.
     *      @param [in] portMaps                The port maps.
     *
     *      @return True, if the required logical port has been mapped, otherwise false.
     */
    bool requiredPortAbstractionsAreMapped(QSharedPointer<BusInterface> busInterface,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    /*!
     *  Check if a logical port has been referenced in any port maps.
     *
     *      @param [in] portName    Name of the selected logical port.
     *      @param [in] portMaps    List of contained port maps.
     *
     *      @return True, if the logical port has been referenced in a port map, otherwise false.
     */
    bool logicalPortHasReferencingPortMaps(QString const& portName,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    /*!
     *  Find errors in required port map logical ports.
     *
     *      @param [in] errors                  List of errors found.
     *      @param [in] context                 Context of the bus interface.
     *      @param [in] busInterface            The containing bus interface.
     *      @param [in] abstractionDefinition   The abstraction definition containing the logical port.
     *      @param [in] portMaps                List of port maps.
     */
    void findErrorsInRequiredPortAbstractions(QVector<QString>& errors, QString const& context,
        QSharedPointer<BusInterface> busInterface,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The currently available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices_;

    //! The currently available views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! The currently available ports.
    QSharedPointer<QList<QSharedPointer<Port> > > availablePorts_;

    //! The currently available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > availableAddressSpaces_;

    //! The currently available memory maps.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > availableMemoryMaps_;

    //! The currently available bus interfaces.
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableBusInterfaces_;
    
    //! The currently available file sets.
    QSharedPointer<QList<QSharedPointer<FileSet> > > availableFileSets_;

    //! The currently available remap states.
    QSharedPointer<QList<QSharedPointer<RemapState> > > availableRemapStates_;

    //! The library interface.
    LibraryInterface* libraryHandler_;

    //! Validator for parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;

    //! Validator for port maps.
    QSharedPointer<PortMapValidator> portMapValidator_;
};

#endif // ADDRESSSPACEVALIDATOR_H
