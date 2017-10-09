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

#include <IPXACTmodels/Component/MirroredSlaveInterface.h>

class Component;
class FileBuilder;
class FileSet;
class File;
class BuildCommand;
class MemoryMap;
class MemoryMapBase;
class AddressBlock;
class Register;
class Field;
class WriteValueConstraint;
class Segment;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class MasterInterface;
class ModuleParameter;
class Port;
class AddressSpace;
class RegisterBase;
class BusInterface;
class RemapState;
class RemapPort;

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
    ~ComponentParameterReferenceCounter();

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
     *  Count the references made to the selected parameter in the selected register.
     *
     *      @param [in] parameterID         ID of the selected parameter.
     *      @param [in] targetRegistesr     The selected registers.
     *
     *      @return The amount of references made to the selected parameter in the selected register.
     */
    int countReferencesInSingleRegister(QString const& parameterID, QSharedPointer<Register> targetRegister) const;

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
     *      @param [in] instantiations  The selected design instantiations.
     *
     *      @return The amount of references made to the selected parameter in the selected design instantiations.
     */
    int countReferencesInDesignInstantiations(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > instantiations) const;

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
        QSharedPointer<MirroredSlaveInterface> mirroredSlave) const;

    /*!
     *  Count the references made to the selected parameter in the selected master bus interface.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] master          The selected master bus interface.
     *
     *      @return The amount of references made to the selected parameter in the selected master bus interface.
     */
    int countReferencesInMasterInterface(QString const& parameterID, QSharedPointer<MasterInterface> master) const;

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
        QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const;

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

public slots:

    /*!
     *  Recalculate the references made to the selected parameters.
     *
     *      @param [in] parameterList   The selected parameters.
     */
    virtual void recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> > parameterList);

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

	//! No copying. No assignment.
    ComponentParameterReferenceCounter(const ComponentParameterReferenceCounter& other);
    ComponentParameterReferenceCounter& operator=(const ComponentParameterReferenceCounter& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The containing component.
    QSharedPointer<Component> component_;
};

#endif // COMPONENTPARAMETERREFERENCECOUNTER_H
