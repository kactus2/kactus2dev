//-----------------------------------------------------------------------------
// File: PythonAPI.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Interface for accessing Kactus2 data using Python.
//-----------------------------------------------------------------------------

#include "PythonAPI.h"

#include <library/LibraryHandler.h>

#include <common/KactusAPI.h>

#include <Plugins/PluginSystem/IPlugin.h>

#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>
#include <editors/ComponentEditor/parameters/ParametersInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>

#include <editors/ComponentEditor/common/ComponentAndInstantiationsParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

//-----------------------------------------------------------------------------
// Function: PythonAPI::PythonAPI()
//-----------------------------------------------------------------------------
PythonAPI::PythonAPI():
library_(KactusAPI::getLibrary()),
messager_(KactusAPI::getMessageChannel()),
activeComponent_(),
portsInterface_(),
componentParameterInterface_(),
mapInterface_(),
parameterFinder_(new ComponentAndInstantiationsParameterFinder(QSharedPointer<Component>())),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder_)),
expressionFormatter_(new ExpressionFormatter(parameterFinder_)),
portValidator_(new PortValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<View> > >())),
parameterValidator_(new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >())),
mapValidator_()
{
    portsInterface_ = new PortsInterface(portValidator_, expressionParser_, expressionFormatter_);
    componentParameterInterface_ =
        new ParametersInterface(parameterValidator_, expressionParser_, expressionFormatter_);

    constructMemoryValidators();
    constructMemoryInterface();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getVersion()
//-----------------------------------------------------------------------------
std::string PythonAPI::getVersion() const
{
    return KactusAPI::getVersion().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getLibraryPaths()
//-----------------------------------------------------------------------------
std::vector<std::string> PythonAPI::getLibraryPaths() const
{
    QStringList locations = KactusAPI::getLibraryPaths();

    std::vector<std::string> paths(locations.count());
    for (auto const& path : locations)
    {
        paths.push_back(path.toStdString());
    }
  
    return paths;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
void PythonAPI::setLibraryPaths(std::vector<std::string> paths) const
{
    QStringList libraryPaths;
    for (auto const& path : paths)
    {
        libraryPaths.append(QString::fromStdString(path));
    }

    KactusAPI::setLibraryPaths(libraryPaths);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getDefaultLibraryPath()
//-----------------------------------------------------------------------------
std::string PythonAPI::getDefaultLibraryPath() const
{
    return KactusAPI::getDefaultLibraryPath().toStdString();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setDefaultLibraryPath()
//-----------------------------------------------------------------------------
void PythonAPI::setDefaultLibraryPath(std::string const& path) const
{
    KactusAPI::setDefaultLibraryPath(QString::fromStdString(path));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setDefaultLibraryPath()
//-----------------------------------------------------------------------------
int PythonAPI::importFile(std::string const& path, std::string vlnv, bool overwrite /*= false*/) const
{
    VLNV targetVLNV(VLNV::COMPONENT, QString::fromStdString(vlnv));

    return KactusAPI::importFile(QString::fromStdString(path), targetVLNV, overwrite);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getFileCount()
//-----------------------------------------------------------------------------
int PythonAPI::getFileCount() const
{
    return library_->getAllVLNVs().count();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::listVLNVs()
//-----------------------------------------------------------------------------
void PythonAPI::listVLNVs(QString const& vendor /* = QString("") */) const
{
    int vlnvCount = 0;

    for (auto itemVLNV : library_->getAllVLNVs())
    {
        messager_->showMessage(itemVLNV.toString());
        vlnvCount++;
    }

    messager_->showMessage(QString("VLNVs found: %1").arg(QString::number(vlnvCount)));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::listComponentVLNVs()
//-----------------------------------------------------------------------------
void PythonAPI::listComponentVLNVs() const
{
    int vlnvCount = 0;

    for (auto itemVLNV : library_->getAllVLNVs())
    {
        if (itemVLNV.getType() == VLNV::COMPONENT)
        {
            messager_->showMessage(itemVLNV.toString());
            vlnvCount++;
        }
    }

    messager_->showMessage(QString("Components found: %1").arg(QString::number(vlnvCount)));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::openComponent()
//-----------------------------------------------------------------------------
bool PythonAPI::openComponent(QString const& componentVLNV)
{
    QStringList vlnvArray = componentVLNV.split(':');
    if (vlnvArray.size() != 4)
    {
        messager_->showError(QString("The given VLNV %1 is not valid").arg(componentVLNV));
        return false;
    }

    VLNV targetVLNV(VLNV::COMPONENT, vlnvArray.at(0), vlnvArray.at(1), vlnvArray.at(2), vlnvArray.at(3));
    QSharedPointer<Document> componentDocument = library_->getModel(targetVLNV);
    if (componentDocument)
    {
        QSharedPointer<Component> component = componentDocument.dynamicCast<Component>();
        if (component)
        {
            parameterFinder_->setComponent(component);
            portValidator_->componentChange(component->getViews());
            portsInterface_->setPorts(component);

            parameterValidator_->componentChange(component->getChoices());
            componentParameterInterface_->setParameters(component->getParameters());
            componentParameterInterface_->setChoices(component->getChoices());

            mapValidator_->componentChange(component->getRemapStates(), component->getResetTypes());
            mapInterface_->setMemoryMaps(component);

            activeComponent_ = component;
            messager_->showMessage(QString("Component %1 is open").arg(componentVLNV));
            return true;
        }
        else
        {
            messager_->showError(QString("The given VLNV %1 is not a component").arg(componentVLNV));
            return false;
        }
    }
    else
    {
        messager_->showError(QString("Could not open document with VLNV %1").arg(componentVLNV));
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::closeOpenComponent()
//-----------------------------------------------------------------------------
void PythonAPI::closeOpenComponent()
{
    activeComponent_ = QSharedPointer<Component>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentName()
//-----------------------------------------------------------------------------
QString PythonAPI::getComponentName()
{
    return activeComponent_->getVlnv().getName();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentDescription()
//-----------------------------------------------------------------------------
QString PythonAPI::getComponentDescription()
{
    return activeComponent_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::saveComponent()
//-----------------------------------------------------------------------------
void PythonAPI::saveComponent()
{
    messager_->showMessage(QString("Saving component %1 ...").arg(activeComponent_->getVlnv().toString()));

    if (library_->writeModelToFile(activeComponent_))
    {
        messager_->showMessage(QString("Save complete"));
    }
    else
    {
        messager_->showError(QString("Could not save component %1").arg(activeComponent_->getVlnv().toString()));
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getPortsInterface()
//-----------------------------------------------------------------------------
PortsInterface* PythonAPI::getPortsInterface() const
{
    return portsInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentParameterInterface()
//-----------------------------------------------------------------------------
ParametersInterface* PythonAPI::getComponentParameterInterface() const
{
    return componentParameterInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::constructMemoryValidators()
//-----------------------------------------------------------------------------
void PythonAPI::constructMemoryValidators()
{
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator>fieldValidator(new FieldValidator(
        expressionParser_, enumValidator, parameterValidator_));
    QSharedPointer<RegisterValidator> registerValidator (new RegisterValidator(
        expressionParser_, fieldValidator, parameterValidator_));
    QSharedPointer<RegisterFileValidator> registerFileValidator(
        new RegisterFileValidator(expressionParser_, registerValidator, parameterValidator_));

    QSharedPointer<AddressBlockValidator> blockValidator(new AddressBlockValidator(
        expressionParser_, registerValidator, registerFileValidator, parameterValidator_));

    QSharedPointer<MemoryMapValidator> mapValidator(new MemoryMapValidator(expressionParser_, blockValidator,
        QSharedPointer<QList<QSharedPointer<RemapState>>>()));

    mapValidator_ = mapValidator;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::constructMemoryInterface()
//-----------------------------------------------------------------------------
void PythonAPI::constructMemoryInterface()
{
    QSharedPointer<AddressBlockValidator> blockValidator = mapValidator_->getAddressBlockValidator();
    QSharedPointer<RegisterValidator> registerValidator = blockValidator->getRegisterValidator();
    QSharedPointer<FieldValidator> fieldValidator = registerValidator->getFieldValidator();

    ResetInterface* resetInterface(new ResetInterface(fieldValidator, expressionParser_, expressionFormatter_));
    FieldInterface* fieldInterface(
        new FieldInterface(fieldValidator, expressionParser_, expressionFormatter_, resetInterface));
    RegisterInterface* registerInterface(
        new RegisterInterface(registerValidator, expressionParser_, expressionFormatter_, fieldInterface));
    AddressBlockInterface* blockInterface(
        new AddressBlockInterface(blockValidator, expressionParser_, expressionFormatter_, registerInterface));

    mapInterface_ = new MemoryMapInterface(mapValidator_, expressionParser_, expressionFormatter_, blockInterface);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* PythonAPI::getMapInterface()
{
    return mapInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setBlocksForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setBlocksForInterface(std::string const& mapName)
{
    AddressBlockInterface* blockInterface = mapInterface_->getSubInterface();

    QString mapNameQT = QString::fromStdString(mapName);
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapNameQT);
    if (!containingMap)
    {
        sendMemoryMapNotFoundError(mapNameQT);
        return;
    }

    blockInterface->setAddressBlocks(containingMap->getMemoryBlocks());
    blockInterface->setAddressUnitBits(containingMap->getAddressUnitBits().toStdString());
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setRegistersForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setRegistersForInterface(std::string const& mapName, std::string const& blockName)
{
    RegisterInterface* interfacePointer = mapInterface_->getSubInterface()->getSubInterface();

    QString mapNameQT = QString::fromStdString(mapName);
    QString blockNameQT = QString::fromStdString(blockName);

    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapNameQT);
    if (containingMap)
    {
        QSharedPointer<AddressBlock> containingBlock = getAddressBock(containingMap, blockNameQT);
        if (containingBlock)
        {
            interfacePointer->setRegisters(containingBlock->getRegisterData());
            interfacePointer->setAddressUnitBits(
                expressionParser_->parseExpression(containingMap->getAddressUnitBits()).toInt());
        }
        else
        {
            sendAddressBlockNotFoundError(mapNameQT, blockNameQT);
        }
    }
    else
    {
        sendMemoryMapNotFoundError(mapNameQT);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setFieldsForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setFieldsForInterface(std::string const& mapName, std::string const& blockName,
    std::string const& registerName)
{
    FieldInterface* interfacePointer = mapInterface_->getSubInterface()->getSubInterface()->getSubInterface();

    QString mapNameQT = QString::fromStdString(mapName);
    QString blockNameQT = QString::fromStdString(blockName);
    QString registerNameQT = QString::fromStdString(registerName);

    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapNameQT);
    if (containingMap)
    {
        QSharedPointer<AddressBlock> containingBlock = getAddressBock(containingMap, blockNameQT);
        if (containingBlock)
        {
            QSharedPointer<Register> containingRegister = getRegister(containingBlock, registerNameQT);
            if (containingRegister)
            {
                interfacePointer->setFields(containingRegister->getFields());
            }
            else
            {
                sendRegisterNotFoundError(mapNameQT, blockNameQT, registerNameQT);
            }
        }
        else
        {
            sendAddressBlockNotFoundError(mapNameQT, blockNameQT);
        }
    }
    else
    {
        sendMemoryMapNotFoundError(mapNameQT);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setResetsForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setResetsForInterface(std::string const& mapName, std::string const& blockName,
    std::string const& registerName, std::string const& fieldName)
{
    ResetInterface* interfacePointer =
        mapInterface_->getSubInterface()->getSubInterface()->getSubInterface()->getSubInterface();

    QString mapNameQT = QString::fromStdString(mapName);
    QString blockNameQT = QString::fromStdString(blockName);
    QString registerNameQT = QString::fromStdString(registerName);
    QString fieldNameQT = QString::fromStdString(fieldName);

    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapNameQT);
    if (containingMap)
    {
        QSharedPointer<AddressBlock> containingBlock = getAddressBock(containingMap, blockNameQT);
        if (containingBlock)
        {
            QSharedPointer<Register> containingRegister = getRegister(containingBlock, registerNameQT);
            if (containingRegister)
            {
                QSharedPointer<Field> containingField = getField(containingRegister, fieldNameQT);
                if (containingField)
                {
                    interfacePointer->setResets(containingField);
                }
                else
                {
                    sendFieldNotFoundError(mapNameQT, blockNameQT, registerNameQT, fieldNameQT);
                }
            }
            else
            {
                sendRegisterNotFoundError(mapNameQT, blockNameQT, registerNameQT);
            }
        }
        else
        {
            sendAddressBlockNotFoundError(mapNameQT, blockNameQT);
        }
    }
    else
    {
        sendMemoryMapNotFoundError(mapNameQT);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> PythonAPI::getMemoryMap(QString const& mapName) const
{
    for (auto map : *activeComponent_->getMemoryMaps())
    {
        if (map->name() == mapName)
        {
            return map;
        }
    }

    return QSharedPointer<MemoryMap>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getAddressBock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> PythonAPI::getAddressBock(QSharedPointer<MemoryMap> containingMap,
    QString const& blockName) const
{
    for (auto baseBlock : *containingMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> block = baseBlock.dynamicCast<AddressBlock>();
        if (block && block->name() == blockName)
        {
            return block;
        }
    }

    return QSharedPointer<AddressBlock>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> PythonAPI::getRegister(QSharedPointer<AddressBlock> containingBlock,
    QString const& registerName) const
{
    for (auto baseRegister : *containingBlock->getRegisterData())
    {
        QSharedPointer<Register> currentRegister = baseRegister.dynamicCast<Register>();
        if (currentRegister && currentRegister->name() == registerName)
        {
            return currentRegister;
        }
    }

    return QSharedPointer<Register>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> PythonAPI::getField(QSharedPointer<Register> containingRegister, QString const& fieldName)
    const
{
    for (auto currentField : *containingRegister->getFields())
    {
        if (currentField->name() == fieldName)
        {
            return currentField;
        }
    }

    return QSharedPointer<Field>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::sendMemoryMapNotFoundError()
//-----------------------------------------------------------------------------
void PythonAPI::sendMemoryMapNotFoundError(QString const& mapName) const
{
    messager_->showError(QString("Could not find memory map %1 within component %2").
        arg(mapName, activeComponent_->getVlnv().toString()));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::sendAddressBlockNotFoundError()
//-----------------------------------------------------------------------------
void PythonAPI::sendAddressBlockNotFoundError(QString const& mapName, QString const& blockName) const
{
    messager_->showError(QString("Could not find address block %1 within memory map %2 in component %3").
        arg(blockName, mapName, activeComponent_->getVlnv().toString()));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::sendRegisterNotFoundError()
//-----------------------------------------------------------------------------
void PythonAPI::sendRegisterNotFoundError(QString const& mapName, QString const& blockName,
    QString const& registerName) const
{
    messager_->showError(
        QString("Could not find register %1 within address block %2 in memory map %3 in component %4").
        arg(registerName, blockName, mapName, activeComponent_->getVlnv().toString()));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::sendFieldNotFoundError()
//-----------------------------------------------------------------------------
void PythonAPI::sendFieldNotFoundError(QString const& mapName, QString const& blockName,
    QString const& registerName, QString const& fieldName) const
{
    messager_->showError(
        QString("Could not find field %1 within register %2 in address block %3 in memory map %4 in component %5").
        arg(fieldName, registerName, blockName, mapName, activeComponent_->getVlnv().toString()));
}
