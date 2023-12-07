//-----------------------------------------------------------------------------
// File: ComponentValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2015
//
// Description:
// Validator for the ipxact:component.
//-----------------------------------------------------------------------------

#include "ComponentValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/Component/ComponentGenerator.h>
#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>
#include <IPXACTmodels/Component/ResetType.h>

#include <IPXACTmodels/common/validators/AssertionValidator.h>
#include <IPXACTmodels/common/validators/ChoiceValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/ChannelValidator.h>
#include <IPXACTmodels/Component/validators/RemapStateValidator.h>
#include <IPXACTmodels/Component/validators/ModeValidator.h>
#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/ViewValidator.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/PowerDomainValidator.h>
#include <IPXACTmodels/Component/validators/ComponentGeneratorValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>
#include <IPXACTmodels/Component/validators/OtherClockDriverValidator.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ComponentValidator::ComponentValidator()
//-----------------------------------------------------------------------------
ComponentValidator::ComponentValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library, Document::Revision docRevision) :
component_(),
busInterfaceValidator_(),
indirectInterfaceValidator_(),
channelValidator_(),
remapStateValidator_(),
modeValidator_(),
addressSpaceValidator_(),
memoryMapValidator_(),
viewValidator_(),
instantiationsValidator_(),
portValidator_(),
generatorValidator_(),
choiceValidator_(),
fileSetValidator_(),
cpuValidator_(),
otherClockDriverValidator_(),
parameterValidator_(),
assertionValidator_()
{
    parameterValidator_ = QSharedPointer<ParameterValidator>(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), docRevision));

    QSharedPointer<PortMapValidator> portMapvalidator(
        new PortMapValidator(parser, QSharedPointer<QList<QSharedPointer<Port> > > (), library));

    busInterfaceValidator_ = QSharedPointer<BusInterfaceValidator>(new BusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), QSharedPointer<QList<QSharedPointer<View> > > (),
        QSharedPointer<QList<QSharedPointer<Port> > > (), QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (), 
        nullptr,
        portMapvalidator, parameterValidator_, library));

    indirectInterfaceValidator_ = QSharedPointer<IndirectInterfaceValidator>(
        new IndirectInterfaceValidator(component_, parser, parameterValidator_));

    channelValidator_ = QSharedPointer<ChannelValidator>(
        new ChannelValidator(parser, QSharedPointer<QList<QSharedPointer<BusInterface> > > ()));

    remapStateValidator_ = QSharedPointer<RemapStateValidator>(
        new RemapStateValidator(parser, QSharedPointer<QList<QSharedPointer<Port> > > ()));
  
    modeValidator_ = QSharedPointer<ModeValidator>(new ModeValidator(component_, parser));

    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator_, docRevision));
    QSharedPointer<RegisterValidator> registerValidator (new RegisterValidator(parser, fieldValidator, parameterValidator_, docRevision));
    QSharedPointer<RegisterFileValidator> registerFileValidator (new RegisterFileValidator(parser, registerValidator, parameterValidator_, docRevision));

    QSharedPointer<AddressBlockValidator> addressBlockValidator(new AddressBlockValidator(
        parser, registerValidator, registerFileValidator, parameterValidator_, docRevision));

    QSharedPointer<SubspaceMapValidator> subspaceValidator(new SubspaceMapValidator(parser, parameterValidator_, docRevision));

    QSharedPointer<MemoryMapBaseValidator> localMapValidator(new MemoryMapBaseValidator(
        parser, addressBlockValidator, subspaceValidator));

    addressSpaceValidator_ = QSharedPointer<AddressSpaceValidator>
        (new AddressSpaceValidator(parser, localMapValidator, parameterValidator_));

    memoryMapValidator_ = QSharedPointer<MemoryMapValidator>(new MemoryMapValidator(
        parser, addressBlockValidator, subspaceValidator, QSharedPointer<Component>()));

    viewValidator_ = QSharedPointer<ViewValidator>(new ViewValidator(parser, QSharedPointer<Model> ()));

    instantiationsValidator_ = QSharedPointer<InstantiationsValidator>(new InstantiationsValidator(
        parser, QSharedPointer<QList<QSharedPointer<FileSet> > > (), parameterValidator_, library));

    portValidator_ =
        QSharedPointer<PortValidator>(new PortValidator(parser, QSharedPointer<QList<QSharedPointer<View> > > ()));

    generatorValidator_ =
        QSharedPointer<ComponentGeneratorValidator>(new ComponentGeneratorValidator(parser, parameterValidator_));

    choiceValidator_ = QSharedPointer<ChoiceValidator>(new ChoiceValidator(parser));

    QSharedPointer<FileValidator> fileValidator (new FileValidator(parser));
    fileSetValidator_ = QSharedPointer<FileSetValidator>(new FileSetValidator(fileValidator, parser));

    cpuValidator_ = QSharedPointer<CPUValidator>(
        new CPUValidator(parameterValidator_, parser, nullptr, nullptr, Document::Revision::Unknown));

    otherClockDriverValidator_ = QSharedPointer<OtherClockDriverValidator>(new OtherClockDriverValidator(parser));

    powerDomainValidator_ = QSharedPointer<PowerDomainValidator>(new PowerDomainValidator(nullptr,
        parser, parameterValidator_));

    assertionValidator_ = QSharedPointer<AssertionValidator>(new AssertionValidator(parser));
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::validate()
//-----------------------------------------------------------------------------
bool ComponentValidator::validate(QSharedPointer<Component> component)
{
    changeComponent(component);

    return hasValidVLNV(component) && hasValidBusInterfaces(component) && 
        hasValidIndirectInterfaces(component) && hasValidChannels(component) &&
        hasValidRemapStates(component) && hasValidModes(component) &&
        hasValidAddressSpaces(component) && hasValidMemoryMaps(component) &&
        hasValidViews(component) && hasValidComponentInstantiations(component) &&
        hasValidDesignInstantiations(component) && hasValidDesignConfigurationInstantiations(component) &&
        hasValidPorts(component) && hasValidComponentGenerators(component) && hasValidChoices(component) &&
        hasValidFileSets(component) && hasValidCPUs(component) && hasValidOtherClockDrivers(component) &&
        hasValidPowerDomains(component) &&
        hasValidResetTypes(component) && hasValidParameters(component) && hasValidAssertions(component);
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidVLNV()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidVLNV(QSharedPointer<Component> component) const
{
    return component->getVlnv().getType() == VLNV::COMPONENT && component->getVlnv().isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidBusInterfaces()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidBusInterfaces(QSharedPointer<Component> component)
{
    changeComponent(component);

    QVector<QString> busInterfaceNames;
    for (QSharedPointer<BusInterface> bus : *component->getBusInterfaces())
    {
        if (busInterfaceNames.contains(bus->name()) || 
            busInterfaceValidator_->validate(bus, component->getRevision()) == false)
        {
            return false;
        }
        
        busInterfaceNames.append(bus->name());
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidIndirectInterfaces()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidIndirectInterfaces(QSharedPointer<Component> component)
{
    changeComponent(component);

    QVector<QString> interfaceNames;
    for (QSharedPointer<IndirectInterface> indirectInterface : *component->getIndirectInterfaces())
    {
        if (interfaceNames.contains(indirectInterface->name()) || 
            !indirectInterfaceValidator_->validate(indirectInterface))
        {
            return false;
        }

        interfaceNames.append(indirectInterface->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidChannels()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidChannels(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getChannels()->isEmpty())
    {
        QVector<QString> channelNames;
        QVector<QString> interfaceNames;
        for (QSharedPointer<Channel> channel : *component->getChannels())
        {
            if (channelNames.contains(channel->name()) || !channelValidator_->validate(channel))
            {
                return false;
            }
            else
            {
                channelNames.append(channel->name());
                for (auto const& busInterface : *channel->getInterfaces())
                {
                    if (interfaceNames.contains(busInterface->localName_))
                    {
                        return false;
                    }
                                    
                    interfaceNames.append(busInterface->localName_);
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidRemapStates()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidRemapStates(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getRemapStates()->isEmpty())
    {
        QVector<QString> stateNames;
        for (QSharedPointer<RemapState> remapState : *component->getRemapStates())
        {
            if (stateNames.contains(remapState->name()) || !remapStateValidator_->validate(remapState))
            {
                return false;
            }
            else
            {
                stateNames.append(remapState->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidModes()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidModes(QSharedPointer<Component> component)
{
    changeComponent(component);

    QVector<QString> modeNames;
    for (QSharedPointer<Mode> mode : *component->getModes())
    {
        if (modeNames.contains(mode->name()) || !modeValidator_->validate(mode))
        {
            return false;
        }

        modeNames.append(mode->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidAddressSpaces()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidAddressSpaces(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getAddressSpaces()->isEmpty())
    {
        QVector<QString> spaceNames;
        for (QSharedPointer<AddressSpace> space : *component->getAddressSpaces())
        {
            if (spaceNames.contains(space->name()) || !addressSpaceValidator_->validate(space))
            {
                return false;
            }
            else
            {
                spaceNames.append(space->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidMemoryMaps()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidMemoryMaps(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getMemoryMaps()->isEmpty())
    {
        QVector<QString> mapNames;
        for (QSharedPointer<MemoryMap> memoryMap : *component->getMemoryMaps())
        {
            if (mapNames.contains(memoryMap->name()) || !memoryMapValidator_->validate(memoryMap))
            {
                return false;
            }
            else
            {
                mapNames.append(memoryMap->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidViews()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidViews(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getViews()->isEmpty())
    {
        QVector<QString> viewNames;
        for (QSharedPointer<View> view : *component->getViews())
        {
            if (viewNames.contains(view->name()) || !viewValidator_->validate(view))
            {
                return false;
            }
            else
            {
                viewNames.append(view->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidComponentInstantiations()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidComponentInstantiations(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getComponentInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        for (QSharedPointer<ComponentInstantiation> instantiation : *component->getComponentInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) ||
                !instantiationsValidator_->validateComponentInstantiation(instantiation, component->getRevision()))
            {
                return false;
            }
            else
            {
                instantiationNames.append(instantiation->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidDesignInstantiations()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidDesignInstantiations(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getDesignInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        for (QSharedPointer<DesignInstantiation> instantiation : *component->getDesignInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) ||
                !instantiationsValidator_->validateDesignInstantiation(instantiation))
            {
                return false;
            }
            else
            {
                instantiationNames.append(instantiation->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidDesignConfigurationInstantiations(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getDesignConfigurationInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        for (QSharedPointer<DesignConfigurationInstantiation> instantiation :
            *component->getDesignConfigurationInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) ||
                !instantiationsValidator_->validateDesignConfigurationInstantiation(instantiation))
            {
                return false;
            }
            else
            {
                instantiationNames.append(instantiation->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidPorts()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidPorts(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getPorts()->isEmpty())
    {
        QVector<QString> portNames;
        for (QSharedPointer<Port> port : *component->getPorts())
        {
            if (portNames.contains(port->name()) || !portValidator_->validate(port))
            {
                return false;
            }
            else
            {
                portNames.append(port->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidComponentGenerators()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidComponentGenerators(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getComponentGenerators()->isEmpty())
    {
        QVector<QString> generatorNames;
        for (QSharedPointer<ComponentGenerator> generator : *component->getComponentGenerators())
        {
            if (generatorNames.contains(generator->name()) || !generatorValidator_->validate(generator))
            {
                return false;
            }
            else
            {
                generatorNames.append(generator->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidChoices()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidChoices(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getChoices()->isEmpty())
    {
        QVector<QString> choiceNames;
        for (QSharedPointer<Choice> choice : *component->getChoices())
        {
            if (choiceNames.contains(choice->name()) || !choiceValidator_->validate(choice))
            {
                return false;
            }
            else
            {
                choiceNames.append(choice->name());
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidFileSets()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidFileSets(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getFileSets()->isEmpty())
    {
        QVector<QString> fileSetNames;
        for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
        {
            if (fileSetNames.contains(fileSet->name()) || !fileSetValidator_->validate(fileSet))
            {
                return false;
            }
            else
            {
                fileSetNames.append(fileSet->name());
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidCPUs()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidCPUs(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getCpus()->isEmpty())
    {
        QVector<QString> cpuNames;
        for (QSharedPointer<Cpu> cpu : *component->getCpus())
        {
            if (cpuNames.contains(cpu->name()) || !cpuValidator_->validate(cpu))
            {
                return false;
            }
            else
            {
                cpuNames.append(cpu->name());
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidOtherClockDrivers()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidOtherClockDrivers(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getOtherClockDrivers()->isEmpty())
    {
        QVector<QString> driverNames;
        for (QSharedPointer<OtherClockDriver> otherClockDriver : *component->getOtherClockDrivers())
        {
            if (driverNames.contains(otherClockDriver->getClockName()) ||
                !otherClockDriverValidator_->validate(otherClockDriver))
            {
                return false;
            }
            else
            {
                driverNames.append(otherClockDriver->getClockName());
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidPowerDomains()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidPowerDomains(QSharedPointer<Component> component)
{
    changeComponent(component);

    QVector<QString> domainNames;
    for (QSharedPointer<PowerDomain> domain : *component->getPowerDomains())
    {
        if (domainNames.contains(domain->name()) ||
            !powerDomainValidator_->validate(domain))
        {
            return false;
        }
        else
        {
            domainNames.append(domain->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidResetTypes()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidResetTypes(QSharedPointer<Component> component)
{
    if (!component->getResetTypes()->isEmpty())
    {
        for(QSharedPointer<ResetType> resetType : *component->getResetTypes())
        {
            if (!singleResetTypeIsValid(resetType, component->getResetTypes()))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::singleResetTypeIsValid()
//-----------------------------------------------------------------------------
bool ComponentValidator::singleResetTypeIsValid(QSharedPointer<ResetType> resetType,
    QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes) const
{
    QString currentName = resetType->name();
    if (currentName.isEmpty() || currentName.toUpper().compare(QLatin1String("HARD")) == 0)
    {
        return false;
    }

    for(QSharedPointer<ResetType> comparisonReset : *resetTypes)
    {
        if (resetType != comparisonReset && currentName == comparisonReset->name())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidParameters(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getParameters()->isEmpty())
    {
        QVector<QString> parameterNames;
        for (QSharedPointer<Parameter> parameter : *component->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::hasValidAssertions()
//-----------------------------------------------------------------------------
bool ComponentValidator::hasValidAssertions(QSharedPointer<Component> component)
{
    changeComponent(component);

    if (!component->getAssertions()->isEmpty())
    {
        QVector<QString> assertionNames;
        for (QSharedPointer<Assertion> assertion : *component->getAssertions())
        {
            if (assertionNames.contains(assertion->name()) || !assertionValidator_->validate(assertion))
            {
                return false;
            }
            else
            {
                assertionNames.append(assertion->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Component> component)
{
    changeComponent(component);

    QString context = QObject::tr("component %1").arg(component->getVlnv().toString());

    findErrorsInVLNV(errors, component);
    findErrorsInBusInterface(errors, component, context);
    findErrorsInIndirectInterfaces(errors, component, context);
    findErrorsInChannels(errors, component, context);
    findErrorsInRemapStates(errors, component, context);
    findErrorsInModes(errors, component, context);
    findErrorsInAddressSpaces(errors, component, context);
    findErrorsInMemoryMaps(errors, component, context);
    findErrorsInViews(errors, component, context);
    findErrorsInComponentInstantiations(errors, component, context);
    findErrorsInDesignInstantiations(errors, component, context);
    findErrorsInDesignConfigurationInstantiations(errors, component, context);
    findErrorsInPorts(errors, component, context);
    findErrorsInComponentGenerators(errors, component, context);
    findErrorsInChoices(errors, component, context);
    findErrorsInFileSets(errors, component, context);
    findErrorsInCPUs(errors, component, context);
    findErrorsInOtherClockDrivers(errors, component, context);
    findErrorsInPowerDomains(errors, component, context);
    findErrorsInResetTypes(errors, component, context);
    findErrorsInParameters(errors, component, context);
    findErrorsInAssertions(errors, component, context);
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInVLNV()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInVLNV(QVector<QString>& errors, QSharedPointer<Component> component) const
{
    component->getVlnv().isValid(errors, QLatin1String("component"));
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInBusInterface()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInBusInterface(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getBusInterfaces()->isEmpty())
    {
        QVector<QString> busInterfaceNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<BusInterface> bus : *component->getBusInterfaces())
        {
            if (busInterfaceNames.contains(bus->name()) && !duplicateNames.contains(bus->name()))
            {
                errors.append(QObject::tr("Bus interface name %1 within %2 is not unique.")
                    .arg(bus->name()).arg(context));
                duplicateNames.append(bus->name());
            }

            busInterfaceNames.append(bus->name());
            busInterfaceValidator_->findErrorsIn(errors, bus, context, component->getRevision());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInIndirectInterfaces()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInIndirectInterfaces(QVector<QString>& errors, QSharedPointer<Component> component, QString const& context)
{
    QVector<QString> interfaceNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<IndirectInterface> indirectInterface : *component->getIndirectInterfaces())
    {
        if (interfaceNames.contains(indirectInterface->name()) && !duplicateNames.contains(indirectInterface->name()))
        {
            errors.append(QObject::tr("Indirect interface name '%1' within %2 is not unique.").arg(
                indirectInterface->name()).arg(context));
            duplicateNames.append(indirectInterface->name());
        }

        interfaceNames.append(indirectInterface->name());
        indirectInterfaceValidator_->findErrorsIn(errors, indirectInterface, context);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInChannels()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInChannels(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getChannels()->isEmpty())
    {
        QVector<QString> channelNames;
        QVector<QString> duplicateChannelNames;

        QVector<QString> interfaceNames;
        QVector<QString> duplicateInterfaceNames;
        for(QSharedPointer<Channel> channel : *component->getChannels())
        {
            if (channelNames.contains(channel->name()) && !duplicateChannelNames.contains(channel->name()))
            {
                errors.append(QObject::tr("Channel name '%1' within %2 is not unique.")
                    .arg(channel->name()).arg(context));
                duplicateChannelNames.append(channel->name());
            }

            channelNames.append(channel->name());
            channelValidator_->findErrorsIn(errors, channel, context);

            for (auto const& busInterface : *channel->getInterfaces())
            {
                if (interfaceNames.contains(busInterface->localName_) && !duplicateInterfaceNames.contains(busInterface->localName_))
                {
                    errors.append(QObject::tr("Bus interface '%1' is not unique in channels within %2.")
                        .arg(busInterface->localName_).arg(context));
                    duplicateInterfaceNames.append(busInterface->localName_);
                }

                interfaceNames.append(busInterface->localName_);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInRemapStates()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInRemapStates(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getRemapStates()->isEmpty())
    {
        QVector<QString> stateNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<RemapState> remapState : *component->getRemapStates())
        {
            if (stateNames.contains(remapState->name()) && !duplicateNames.contains(remapState->name()))
            {
                errors.append(QObject::tr("Remap state name %1 within %2 is not unique.")
                    .arg(remapState->name()).arg(context));
                duplicateNames.append(remapState->name());
            }

            stateNames.append(remapState->name());
            remapStateValidator_->findErrorsIn(errors, remapState, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInModes()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInModes(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    QStringList modeNames;
    QStringList duplicateNames;
    for (auto const& mode : *component->getModes())
    {
        if (modeNames.contains(mode->name()) && !duplicateNames.contains(mode->name()))
        {
            errors.append(QObject::tr("Mode name %1 within %2 is not unique.")
                .arg(mode->name()).arg(context));
            duplicateNames.append(mode->name());
        }

        modeNames.append(mode->name());
        modeValidator_->findErrorsIn(errors, mode, context);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInAddressSpaces()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInAddressSpaces(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getAddressSpaces()->isEmpty())
    {
        QVector<QString> spaceNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<AddressSpace> addressSpace : *component->getAddressSpaces())
        {
            if (spaceNames.contains(addressSpace->name()) && !duplicateNames.contains(addressSpace->name()))
            {
                errors.append(QObject::tr("Address space name %1 within %2 is not unique.")
                    .arg(addressSpace->name()).arg(context));
                duplicateNames.append(addressSpace->name());
            }

            spaceNames.append(addressSpace->name());
            addressSpaceValidator_->findErrorsIn(errors, addressSpace, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInMemoryMaps()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInMemoryMaps(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getMemoryMaps()->isEmpty())
    {
        QVector<QString> mapNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<MemoryMap> memoryMap : *component->getMemoryMaps())
        {
            if (mapNames.contains(memoryMap->name()) && !duplicateNames.contains(memoryMap->name()))
            {
                errors.append(QObject::tr("Memory map name %1 within %2 is not unique.")
                    .arg(memoryMap->name()).arg(context));
                duplicateNames.append(memoryMap->name());
            }

            mapNames.append(memoryMap->name());
            memoryMapValidator_->findErrorsIn(errors, memoryMap, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInViews()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInViews(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getViews()->isEmpty())
    {
        QVector<QString> viewNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<View> view : *component->getViews())
        {
            if (viewNames.contains(view->name()) && !duplicateNames.contains(view->name()))
            {
                errors.append(QObject::tr("View name %1 within %2 is not unique.").arg(view->name()).arg(context));
                duplicateNames.append(view->name());
            }
            
            viewNames.append(view->name());
            viewValidator_->findErrorsIn(errors, view, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInComponentInstantiations()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInComponentInstantiations(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    if (!component->getComponentInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<ComponentInstantiation> instantiation : *component->getComponentInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) &&
                !duplicateNames.contains(instantiation->name()))
            {
                errors.append(QObject::tr("Component instantiation name %1 within %2 is not unique.")
                    .arg(instantiation->name()).arg(context));
                duplicateNames.append(instantiation->name());
            }

            instantiationNames.append(instantiation->name());
            instantiationsValidator_->findErrorsInComponentInstantiation(errors, instantiation, context,
                component->getRevision());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInDesignInstantiations()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInDesignInstantiations(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    if (!component->getDesignInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<DesignInstantiation> instantiation : *component->getDesignInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) &&
                !duplicateNames.contains(instantiation->name()))
            {
                errors.append(QObject::tr("Design instantiation name %1 within %2 is not unique.")
                    .arg(instantiation->name()).arg(context));
                duplicateNames.append(instantiation->name());
            }

            instantiationNames.append(instantiation->name());
            instantiationsValidator_->findErrorsInDesignInstantiation(errors, instantiation, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInDesignConfigurationInstantiations(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    if (!component->getDesignConfigurationInstantiations()->isEmpty())
    {
        QVector<QString> instantiationNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<DesignConfigurationInstantiation> instantiation :
            *component->getDesignConfigurationInstantiations())
        {
            if (instantiationNames.contains(instantiation->name()) &&
                !duplicateNames.contains(instantiation->name()))
            {
                errors.append(QObject::tr("Design configuration instantiation name %1 within %2 is not unique.")
                    .arg(instantiation->name()).arg(context));
                duplicateNames.append(instantiation->name());
            }

            instantiationNames.append(instantiation->name());
            instantiationsValidator_->findErrorsInDesignConfigurationInstantiation(errors, instantiation, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInPorts()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInPorts(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getPorts()->isEmpty())
    {
        QVector<QString> portNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<Port> port : *component->getPorts())
        {
            if (portNames.contains(port->name()) && !duplicateNames.contains(port->name()))
            {
                errors.append(QObject::tr("Port name %1 within %2 is not unique.").arg(port->name()).arg(context));
                duplicateNames.append(port->name());
            }

            portNames.append(port->name());
            portValidator_->findErrorsIn(errors, port, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInComponentGenerators()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInComponentGenerators(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    if (!component->getComponentGenerators()->isEmpty())
    {
        QVector<QString> generatorNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<ComponentGenerator> generator : *component->getComponentGenerators())
        {
            if (generatorNames.contains(generator->name()) && !duplicateNames.contains(generator->name()))
            {
                errors.append(QObject::tr("Component generator name '%1' within %2 is not unique.")
                    .arg(generator->name()).arg(context));
                duplicateNames.append(generator->name());
            }

            generatorNames.append(generator->name());
            generatorValidator_->findErrorsIn(errors, generator, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInChoices()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInChoices(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getChoices()->isEmpty())
    {
        QVector<QString> choiceNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<Choice> choice : *component->getChoices())
        {
            if (choiceNames.contains(choice->name()) && !duplicateNames.contains(choice->name()))
            {
                errors.append(QObject::tr("Choice name %1 within %2 is not unique.")
                    .arg(choice->name()).arg(context));
                duplicateNames.append(choice->name());
            }

            choiceNames.append(choice->name());
            choiceValidator_->findErrorsIn(errors, choice, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInFileSets()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInFileSets(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getFileSets()->isEmpty())
    {
        QVector<QString> fileSetNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
        {
            if (fileSetNames.contains(fileSet->name()) && !duplicateNames.contains(fileSet->name()))
            {
                errors.append(QObject::tr("File set name %1 within %2 is not unique.")
                    .arg(fileSet->name()).arg(context));
                duplicateNames.append(fileSet->name());
            }

            fileSetNames.append(fileSet->name());
            fileSetValidator_->findErrorsIn(errors, fileSet, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInCPUs()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInCPUs(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getCpus()->isEmpty())
    {
        QVector<QString> cpuNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<Cpu> cpu : *component->getCpus())
        {
            if (cpuNames.contains(cpu->name()) && !duplicateNames.contains(cpu->name()))
            {
                errors.append(QObject::tr("CPU name %1 within %2 is not unique.").arg(cpu->name()).arg(context));
                duplicateNames.append(cpu->name());
            }

            cpuNames.append(cpu->name());
            cpuValidator_->findErrorsIn(errors, cpu, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInOtherClockDrivers()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInOtherClockDrivers(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    if (!component->getOtherClockDrivers()->isEmpty())
    {
        QVector<QString> clockDriverNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<OtherClockDriver> clockDriver : *component->getOtherClockDrivers())
        {
            if (clockDriverNames.contains(clockDriver->getClockName()) &&
                !duplicateNames.contains(clockDriver->getClockName()))
            {
                errors.append(QObject::tr("Other clock driver name %1 within %2 is not unique.")
                    .arg(clockDriver->getClockName()).arg(context));
                duplicateNames.append(clockDriver->getClockName());
            }

            clockDriverNames.append(clockDriver->getClockName());
            otherClockDriverValidator_->findErrorsIn(errors, clockDriver, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInPowerDomains()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInPowerDomains(QVector<QString>& errors,
    QSharedPointer<Component> component, QString const& context) const
{
    QVector<QString> domainNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<PowerDomain> domain : *component->getPowerDomains())
    {
        if (domainNames.contains(domain->name()) && !duplicateNames.contains(domain->name()))
        {
            errors.append(QObject::tr("Power domain name %1 within %2 is not unique.").arg(domain->name(), context));
            duplicateNames.append(domain->name());
        }

        domainNames.append(domain->name());
        powerDomainValidator_->findErrorsIn(errors, domain, context);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInResetTypes()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInResetTypes(QVector<QString>&errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getResetTypes()->isEmpty())
    {
        QVector<QString> resetTypeNames;
        QVector<QString> duplicateNames;
        for(QSharedPointer<ResetType> resetType : *component->getResetTypes())
        {
            if (resetType->name().isEmpty())
            {
                errors.append(QObject::tr("Invalid name '%1' set for reset type within %2")
                    .arg(resetType->name()).arg(context));
            }

            if (resetType->name().compare(QLatin1String("HARD"), Qt::CaseInsensitive) == 0)
            {
                errors.append(QObject::tr
                ("Invalid name '%1' set for reset type within %2, HARD is reserved for the default reset type.")
                    .arg(resetType->name()).arg(context));
            }

            if (resetTypeNames.contains(resetType->name()) && !duplicateNames.contains(resetType->name()))
            {
                errors.append(QObject::tr("Reset type name '%1' within %2 is not unique.")
                    .arg(resetType->name()).arg(context));
                duplicateNames.append(resetType->name());
            }

            resetTypeNames.append(resetType->name());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInParameters(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getParameters()->isEmpty())
    {
        QVector<QString> parameterNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<Parameter> parameter : *component->getParameters())
        {
            if (parameterNames.contains(parameter->name()) && !duplicateNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Parameter name %1 within %2 is not unique.")
                    .arg(parameter->name()).arg(context));
                duplicateNames.append(parameter->name());
            }

            parameterNames.append(parameter->name());
            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::findErrorsInAssertions()
//-----------------------------------------------------------------------------
void ComponentValidator::findErrorsInAssertions(QVector<QString>& errors, QSharedPointer<Component> component,
    QString const& context) const
{
    if (!component->getAssertions()->isEmpty())
    {
        QVector<QString> assertionNames;
        QVector<QString> duplicateNames;
        for (QSharedPointer<Assertion> assertion : *component->getAssertions())
        {
            if (assertionNames.contains(assertion->name()) && !duplicateNames.contains(assertion->name()))
            {
                errors.append(QObject::tr("Assertion name %1 within %2 is not unique.")
                    .arg(assertion->name()).arg(context));
                duplicateNames.append(assertion->name());
            }

            assertionNames.append(assertion->name());
            assertionValidator_->findErrorsIn(errors, assertion, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentValidator::updateComponent()
//-----------------------------------------------------------------------------
void ComponentValidator::changeComponent(QSharedPointer<Component> newComponent)
{
    if (newComponent && (!component_ || component_ != newComponent))
    {
        busInterfaceValidator_->componentChange(newComponent->getChoices(), newComponent->getViews(),
            newComponent->getPorts(), newComponent->getAddressSpaces(), newComponent->getMemoryMaps(),
            newComponent->getBusInterfaces(), newComponent->getFileSets(), newComponent->getRemapStates(),
            newComponent->getModes());
        indirectInterfaceValidator_->componentChange(newComponent);
        parameterValidator_->componentChange(newComponent->getChoices(), newComponent->getRevision());
        channelValidator_->componentChange(newComponent->getBusInterfaces());
        remapStateValidator_->componentChange(newComponent->getPorts());
        modeValidator_->componentChange(newComponent);
        memoryMapValidator_->componentChange(newComponent);
        viewValidator_->componentChange(newComponent->getModel());
        instantiationsValidator_->componentChange(newComponent->getFileSets());
        portValidator_->componentChange(newComponent->getViews());
        cpuValidator_->componentChange(newComponent->getAddressSpaces(), newComponent->getMemoryMaps(),
            newComponent->getRevision());
        powerDomainValidator_->componentChange(newComponent->getPowerDomains());
        addressSpaceValidator_->componentChange(newComponent);

        component_ = newComponent;
    }
}
