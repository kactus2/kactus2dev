//-----------------------------------------------------------------------------
// File: ComponentParameterReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.10.2017
//
// Description:
// Calculates the amount of references made to component parameters.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERREFERENCECOUNTER_H
#define COMPONENTPARAMETERREFERENCECOUNTER_H

#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include <IPXACTmodels/Component/MirroredTargetInterface.h>

class Component;
class Cpu;
class FileBuilder;
class FileSet;
class File;
class BuildCommand;
class MemoryMap;
class MemoryMapBase;
class AddressBlock;
class Register;
class RegisterFile;
class Field;
class FieldAccessPolicy;
class FieldSlice;
class FieldReset;
class WriteValueConstraint;
class Segment;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class InitiatorInterface;
class ModuleParameter;
class Port;
class PowerDomain;
class AddressSpace;
class RegisterBase;
class BusInterface;
class Region;
class RemapState;
class RemapPort;
class AbstractParameterInterface;
class IndirectInterface;
class Mode;
class PortSlice;

//-----------------------------------------------------------------------------
//! Calculates the amount of references made to component parameters.
//-----------------------------------------------------------------------------
class ComponentParameterReferenceCounter : public ParameterReferenceCounter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterFinder     Locator for component parameters.
     *      @param [in] component           The containing component.
     */
    ComponentParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<Component> component);

    /*!
     *  The destructor.
     */
    virtual ~ComponentParameterReferenceCounter() = default;

    //! No copying. No assignment.
    ComponentParameterReferenceCounter(const ComponentParameterReferenceCounter& other) = delete;
    ComponentParameterReferenceCounter& operator=(const ComponentParameterReferenceCounter& other) = delete;

    /*!
     *  Set a new component.
     *
     *      @param [in] newComponent    The selected component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Count the references made to the selected parameter in the file sets.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the file sets.
     */
    int countReferencesInFileSets(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected file set.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] fileSet         The selected file set.
     *
     *      @return The amount of references made to the selected parameter in the selected file set.
     */
    int countReferencesInSingleFileSet(QString const& parameterID, QSharedPointer<FileSet> fileSet) const;

    /*!
     *  Count the references made to the selected parameter in the selected file builders.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] builders        The selected file builders.
     *
     *      @return The amount of references made to the selected parameter in the selected file builders.
     */
    int countReferencesInFileBuilders(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<FileBuilder> > > builders) const;

    /*!
     *  Count the references made to the selected parameter in the selected file builder.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] builder         The selected file builder.
     *
     *      @return The amount of references made to the selected parameter in the selected file builder.
     */
    int countReferencesInSingleFileBuilder(QString const& parameterID, QSharedPointer<FileBuilder> builder) const;

    /*!
     *  Count the references made to the selected parameter in the selected files.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] files           The selected files.
     *
     *      @return The amount of references made to the selected parameter in the selected files.
     */
    int countReferencesInFiles(QString const& parameterID, QSharedPointer<QList<QSharedPointer<File> > > files)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected file.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] file            The selected file.
     *
     *      @return The amount of references made to the selected parameter in the selected file.
     */
    int countReferencesInSingleFile(QString const& parameterID, QSharedPointer<File> file) const;

    /*!
     *  Count the references made to the selected parameter in the selected build command.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] buildCommand    The selected build command.
     *
     *      @return The amount of references made to the selected parameter in the selected build command.
     */
    int countReferencesInBuildCommand(QString const& parameterID, QSharedPointer<BuildCommand> buildCommand)
        const;

    /*!
     *  Count the references made to the selected parameter in the contained memory maps.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained memory maps.
     */
    int countReferencesInMemoryMaps(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected memory map.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] memoryMap       The selected memory map.
     *
     *      @return The amount of references made to the selected parameter in the selected memory map.
     */
    int countReferencesInSingleMemoryMap(QString const& parameterID, QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Count the references made to the selected parameter in the selected base memory map.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] memoryMap       The selected base memory map.
     *
     *      @return The amount of references made to the selected parameter in the selected base memory map.
     */
    int countReferencesInBaseMemoryMap(QString const& parameterID, QSharedPointer<MemoryMapBase> memoryMap) const;

    /*!
     *  Count the references made to the selected parameter in the values of the selected memory base.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] memoryMap       The selected memory base.
     *
     *      @return The amount of references made to the selected parameter in the selected memory base.
     */
    int countReferencesInMemoryMapValues(QString const& parameterID, QSharedPointer<MemoryMapBase> memoryMap) const;

    /*!
     *  Count the references made to the selected parameter in the selected address blocks.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return The amount of references made to the selected parameter in the selected address block.
     */
    int countReferenceInAddressBlock(QString const& parameterID, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Count the references made to the selected parameter in the values of the selected address block.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return The amount of references made to the selected parameter in the values of the selected address
     *              block.
     */
    int countReferencesInAddressBlockValues(QString const& parameterID, QSharedPointer<AddressBlock> addressBlock)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected registers.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] registers       The selected registers.
     *
     *      @return The amount of references made to the selected parameter in the selected registers.
     */
    int countReferencesInRegisters(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registers) const;

    /*!
     *  Count the references made to the selected parameter in the selected register files.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] files           The selected register files.
     *
     *      @return The amount of references made to the selected parameter in the selected register files.
     */
    int countReferencesInRegisterFiles(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > const& files) const;

    /*!
     *  Count the references made to the selected parameter in the selected register.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] targetRegistesr     The selected registers.
     *
     *      @return The amount of references made to the selected parameter in the selected register.
     */
    int countReferencesInSingleRegister(QString const& parameterID, QSharedPointer<Register> targetRegister) const;

    /*!
     *  Count the references made to the selected parameter in the selected register file.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] targetFile      The selected register file.
     *
     *      @return The amount of references made to the selected parameter in the selected register file.
     */
    int countReferencesInSingleRegisterFile(QString const& parameterID, QSharedPointer<RegisterFile> targetFile)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected base register.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] baseRegister    The selected base register.
     *
     *      @return The amount of references made to the selected parameter in the selected base register.
     */
    int countReferencesInSingleBaseRegister(QString const& parameterID, QSharedPointer<RegisterBase> baseRegister)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected fields.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] fields          The selected fields.
     *
     *      @return The amount of references made to the selected parameter in the selected fields.
     */
    int countReferencesInFields(QString const& parameterID, QSharedPointer<QList<QSharedPointer<Field> > > fields)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected field.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] registerField   The selected field.
     *
     *      @return The amount of references made to the selected parameter in the selected field.
     */
    int countReferencesInSingleField(QString const& parameterID, QSharedPointer<Field> registerField) const;

    /*!
     *  Count the references made to the selected parameter in the resets of the selected field.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] resets          Reset values of the selected field.
     *
     *      @return The amount of references made to the selected parameter in the resets of the selected field.
     */
    int countReferencesInFieldResets(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<FieldReset> > >  resets) const;

    /*!
     *  Count the references made to the selected parameter in the selected field reset.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] fieldReset      The selected field reset.
     *
     *      @return The amount of references made to the selected parameter in the selected field reset.
     */
    int countReferencesInSingleFieldReset(QString const& parameterID, QSharedPointer<FieldReset> fieldReset) const;

    /*!
     *	Count the references made to the selected parameter in the field access policies in the selected field.
     *  
     *      @param [in] parameterID        ID of the selected parameter
     *      @param [in] accessPolicies     Field access policies of the selected field.
     *	    
     * 	    @return The number of references made to the selected parameters in the field access policies.
     */
    int countReferencesInFieldAccessPolicies(QString const& parameterID, 
        QSharedPointer<QList<QSharedPointer<FieldAccessPolicy> > > accessPolicies) const;

    /*!
     *  Count the references made to the selected parameter in the contained address spaces.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained address spaces.
     */
    int countReferencesInAddressSpaces(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected address space.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] space           The selected address space.
     *
     *      @return The amount of references made to the selected parameter in the selected address space.
     */
    int countReferencesInSingleAddressSpace(QString const& parameterID, QSharedPointer<AddressSpace> space) const;

    /*!
     *  Count the references made to the selected parameter in the selected address space values.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] space           The selected address space.
     *
     *      @return The amount of references made to the selected parameter in the selected address space values.
     */
    int countReferencesInSingleAddressSpaceItems(QString const& parameterID, QSharedPointer<AddressSpace> space)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected segments.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] segments        The selected segments.
     *
     *      @return The amount of references made to the selected parameter in the selected segments.
     */
    int countReferencesInSegments(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<Segment> > > segments) const;

    /*!
     *  Count the references made to the selected parameter in the selected segment.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] segment         The selected segment.
     *
     *      @return The amount of references made to the selected parameter in the selected segment.
     */
    int countReferencesInSingleSegment(QString const& parameterID, QSharedPointer<Segment> segment) const;

    /*!
     *  Count the references made to the selected parameter in the contained instantiations.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained instantiations.
     */
    int countReferencesInInstantiations(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected component instantiations.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instantiations  The selected component instantiations.
     *
     *      @return The amount of references made to the selected parameter in the selected component
     *              instantiations.
     */
    int countReferencesInComponentInstantiations(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations) const;

    /*!
     *  Count the references made to the selected parameter in the selected component instantiation.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instantiation   The selected component instantiation.
     *
     *      @return The amount of references made to the selected parameter in the selected component
     *              instantiation.
     */
    int countReferencesInSingleComponentInstantiation(QString const& parameterID,
        QSharedPointer<ComponentInstantiation> instantiation) const;

    /*!
     *  Count the references made to the selected parameter in the selected module parameters.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] moduleParameters    The selected module parameters.
     *
     *      @return The amount of references made to the selected parameter in the selected module parameters.
     */
    int countReferencesInModuleParameters(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters) const;

    /*!
     *  Count the references made to the selected parameter in the selected design configuration instantiations.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instantiations  The selected design configuration instantiations.
     *
     *      @return The amount of references made to the selected parameter in the selected design configuration
     *              instantiations.
     */
    int countReferencesInDesignConfigurationInstantiations(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > instantiations) const;

    /*!
     *  Count the references made to the selected parameter in the selected design configuration instantiation.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instantiation   The selected design configuration instantiation.
     *
     *      @return The amount of references made to the selected parameter in the selected design configuration
     *              instantiation.
     */
    int countReferencesInSingleDesignConfigurationInstantiation(QString const& parameterID,
        QSharedPointer<DesignConfigurationInstantiation> instantiation) const;

    /*!
     *  Count the references made to the selected parameter in the selected design instantiations.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the selected design instantiations.
     */
    int countReferencesInDesignInstantiations(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected design instantiation.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instantiation   The selected design instantiation.
     *
     *      @return The amount of references made to the selected parameter in the selected design instantiation.
     */
    int countReferencesInSingleDesigninstantiation(QString const& parameterID,
        QSharedPointer<DesignInstantiation> instantiation) const;

    /*!
     *  Count the references made to the selected parameter in the contained ports.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained ports.
     */
    int countReferencesInPorts(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected port.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] port            The selected port.
     *
     *      @return The amount of references made to the selected parameter in the selected port.
     */
    int countReferencesInSinglePort(QString const& parameterID, QSharedPointer<Port> port) const;

    /*!
     *  Count the references made to the selected parameter in the contained bus interfaces.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained bus interfaces.
     */
    int countReferencesInBusInterfaces(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected bus interface.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] busInterface    The selected bus interface.
     *
     *      @return The amount of references made to the selected parameter in the selected bus interface.
     */
    int countReferencesInSingleBusInterface(QString const& parameterID, QSharedPointer<BusInterface> busInterface)
        const;

    /*!
     *  Count the references made to the selected parameter in the selected mirrored slave bus interface.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] mirroredSlave   The selected mirrored slave bus interface.
     *
     *      @return The amount of references made to the selected parameter in the selected mirrored slave bus
     *              interface.
     */
    int countReferencesInMirroredSlaveInterface(QString const& parameterID,
        QSharedPointer<MirroredTargetInterface> mirroredSlave) const;

    /*!
     *  Count the references made to the selected parameter in the selected master bus interface.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] master          The selected master bus interface.
     *
     *      @return The amount of references made to the selected parameter in the selected master bus interface.
     */
    int countReferencesInMasterInterface(QString const& parameterID, QSharedPointer<InitiatorInterface> master) const;

    /*!
     *  Count the references made to the selected parameter in the selected mirrored slave remap address.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] remapAddress    The selected mirrored slave remap address.
     *
     *      @return The amount of references made to the selected parameter in the selected mirrored slave remap
     *              address.
     */
    int countReferencesInRemapAddress(QString const& parameterID,
        QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress) const;

    /*!
     *  Count the references made to the selected parameter in the contained remap states.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained remap states.
     */
    int countReferencesInRemapStates(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected remap state.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] remapState      The selected remap state.
     *
     *      @return The amount of references made to the selected parameter in the selected remap state.
     */
    int countReferencesInSingleRemapState(QString const& parameterID, QSharedPointer<RemapState> remapState) const;

    /*!
     *  Count the references made to the selected parameter in the selected remap port.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] port            The selected remap port.
     *
     *      @return The amount of references made to the selected parameter in the selected remap port.
     */
    int countReferencesInSingleRemapPort(QString const& parameterID, QSharedPointer<RemapPort> port) const;

    /*!
     *  Count the references made to the selected parameter in the contained modes.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the contained modes.
     */
    int countReferencesInModes(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected mode.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] mode            The selected mode.
     *
     *      @return The amount of references made to the selected parameter in the selected mode.
     */
    int countReferencesInSingleMode(QString const& parameterID, QSharedPointer<Mode> mode) const;

    /*!
     *  Count the references made to the selected parameter in the selected mode condition.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] condition       The selected condition expression.
     *
     *      @return The amount of references made to the selected parameter in the selected expression.
     */
    int countReferencesInModeCondition(QString const& parameterID, QString const& condition) const;

    /*!
     *  Count the references made to the selected parameter in the port slices of the selected mode.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] mode            The selected mode.
     *
     *      @return The amount of references made to the selected parameter in the port slices.
     */
    int countReferencesInPortSlices(QString const& parameterID, QSharedPointer<Mode> mode) const;

     /*!
      *  Count the references made to the selected parameter in the field slices of the selected mode.
      *
      *      @param [in] parameterID    ID of the selected parameter.
      *      @param [in] mode           The selected mode.
      *
      *      @return The amount of references made to the selected parameter in the selected field slice.
      */
    int countReferencesInFieldSlices(QString const& parameterID, QSharedPointer<Mode> mode) const;

    /*!
     *  Count the references made to the selected parameter in the selected port slice.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] portSlice       The selected port slice.
     *
     *      @return The amount of references made to the selected parameter in the port slice.
     */
    int countReferencesInSinglePortSlice(QString const& targetID, QSharedPointer<PortSlice> portSlice) const;
        
    /*!
     *  Count the references made to the selected parameter in the selected field slice.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] fieldSlice      The selected field slice.
     *
     *      @return The amount of references made to the selected parameter in the field slice.
     */
    int countReferencesInSingleFieldSlice(QString const& parameterID, QSharedPointer<FieldSlice> fieldSlice) const;

    /*!
     *  Count the references made to the selected parameter in indirect interfaces.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the indirect interfaces.
     */
    int countReferencesInIndirectInterfaces(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected indirect interface.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] indirectInterface   The selected indirect interface.
     *
     *      @return The amount of references made to the selected parameter in the selected indirect interface.
     */
    int countRefrencesInSingleIndirectInterface(QString const& parameterID,
        QSharedPointer<IndirectInterface> indirectInterface) const;

    /*!
     *  Count the references made to the selected parameter in CPUs.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the CPUs.
     */
    int countReferencesInCpus(QString const& parameterID) const;

    /*!
     *  Count the references made to the selected parameter in the selected CPU.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] cpu                 The selected CPU.
     *
     *      @return The amount of references made to the selected parameter in the selected CPU.
     */
    int countReferencesInSingleCpu(QString const& parameterID, QSharedPointer<Cpu> cpu) const;

    /*!
     *  Count the references made to the selected parameter in CPU regions interfaces.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] regions         The selected CPU regions.
     *
     *      @return The amount of references made to the selected parameter in the CPU regions.
     */
    int countReferencesInRegions(QString const& parameterID, 
        QSharedPointer<QList<QSharedPointer<Region> > > regions) const;
    
    /*!
     *  Count the references made to the selected parameter in the selected CPU region.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] region              The selected CPU region.
     *
     *      @return The amount of references made to the selected parameter in the selected CPU region.
     */
    int countReferencesInSingleRegion(QString const& parameterID, QSharedPointer<Region> region) const;


    /*!
     *  Count the references made to the selected parameter in power domains.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The amount of references made to the selected parameter in the power domains.
     */
    int countReferencesInPowerDomains(QString const& parameterID) const;
        
    /*!
     *  Count the references made to the selected parameter in the selected power domain.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] powerDomain         The selected power domain.
     *
     *      @return The amount of references made to the selected parameter in the selected power domain.
     */
    int countReferencesInSinglePowerDomain(QString const& parameterID, 
        QSharedPointer<PowerDomain> powerDomain) const;

public slots:

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *      @param [in] parameterList       The selected parameters.
     *      @param [in] parameterInterface  Interface for accessing parameters.
     */
    virtual void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        AbstractParameterInterface* parameterInterface) override final;

private:

    /*!
     *  Count the references made to the selected parameter in the selected write constraint.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] writeConstraint     The selected write constraint.
     *
     *      @return The amount of references made to the selected parameter in the selected write constraint.
     */
    int countReferencesInWriteConstraint(QString const& parameterID,
        QSharedPointer<WriteValueConstraint> writeConstraint) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The containing component.
    QSharedPointer<Component> component_;

};

#endif // COMPONENTPARAMETERREFERENCECOUNTER_H
