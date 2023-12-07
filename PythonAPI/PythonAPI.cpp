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


#include <KactusAPI/KactusAPI.h>

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/IGeneratorPlugin.h>
#include <KactusAPI/include/CLIGenerator.h>

#include <KactusAPI/include/BusInterfaceUtilities.h>

#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/AccessPolicyInterface.h>
#include <KactusAPI/include/ModeReferenceInterface.h>
#include <KactusAPI/include/ResetInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/SubspaceMapInterface.h>
#include <KactusAPI/include/FileSetInterface.h>
#include <KactusAPI/include/FileInterface.h>
#include <KactusAPI/include/FileBuilderInterface.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
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
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>

#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Design.h>


#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: PythonAPI::PythonAPI()
//-----------------------------------------------------------------------------
PythonAPI::PythonAPI()
{
    constructMemoryValidators();
    constructMemoryInterface();
    constructFileSetInterface();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setupLibrary()
//-----------------------------------------------------------------------------
void PythonAPI::setupLibrary(std::string const& /*settingsFileString*/)
{
    library_->searchForIPXactFiles();
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

    std::vector<std::string> paths;
    for (auto const& path : locations)
    {
        paths.push_back(path.toStdString());
    }
  
    return paths;
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
// Function: PythonAPI::getMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* PythonAPI::getMapInterface()
{
    return mapInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* PythonAPI::getFileSetInterface()
{
    return fileSetInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* PythonAPI::getBusInterface()
{
    return busInterface_;
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
int PythonAPI::importFile(std::string const& path, std::string const& vlnv, bool overwrite /*= false*/) const
{
    VLNV targetVLNV(VLNV::COMPONENT, QString::fromStdString(vlnv));

    return KactusAPI::importFile(QString::fromStdString(path), targetVLNV, overwrite);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::generate()
//-----------------------------------------------------------------------------
void PythonAPI::generate(std::string const& format, std::string const& vlnv, std::string const& viewName,
    std::string const& outputDirectory) const
{
    VLNV componentVLNV(VLNV::COMPONENT, QString::fromStdString(vlnv));

    QString fileFormat = QString::fromStdString(format).toLower();

    QStringList availableFormats;

    IGeneratorPlugin* generator = nullptr;
    for (auto plugin : KactusAPI::getPlugins())
    {
        CLIGenerator* runnable = dynamic_cast<CLIGenerator*>(plugin);
        if (runnable != 0)
        {
            if (runnable->getOutputFormat().toLower() == fileFormat)
            {
                generator = dynamic_cast<IGeneratorPlugin*>(plugin);
            }

            availableFormats.append(runnable->getOutputFormat());
        }
    }
    
    if (generator != nullptr)
    {
        KactusAPI::runGenerator(generator, componentVLNV, QString::fromStdString(viewName),
            QString::fromStdString(outputDirectory), nullptr);
    }
    else
    {
        availableFormats.sort(Qt::CaseInsensitive);
        messager_->showError(QStringLiteral("No generator found for format %1. Available options are: %2").arg(
            fileFormat, availableFormats.join(',')));
    }
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
std::vector<std::string> PythonAPI::listVLNVs(std::string const& vendor) const
{
    std::vector<std::string> vlnvStrings;

    for (auto const& itemVLNV : library_->getAllVLNVs())
    {
        vlnvStrings.push_back(itemVLNV.toString().toStdString());
    }

    return vlnvStrings;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::listComponentVLNVs()
//-----------------------------------------------------------------------------
std::vector<std::string> PythonAPI::listComponentVLNVs() const
{
    std::vector<std::string> componentVLNVs;

    for (auto const& itemVLNV : library_->getAllVLNVs())
    {
        if (itemVLNV.getType() == VLNV::COMPONENT)
        {
            componentVLNVs.push_back(itemVLNV.toString().toStdString());
        }
    }

    return componentVLNVs;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::vlnvExistsInLibrary()
//-----------------------------------------------------------------------------
bool PythonAPI::vlnvExistsInLibrary(std::string const& vendor, std::string const& library, std::string const& name,
    std::string const& version) const
{
    if (vendor.empty() || library.empty() || name.empty() || version.empty())
    {
        messager_->showError("Error in given VLNV.");
        return false;
    }

    return library_->contains(VLNV(VLNV::INVALID,
        QString::fromStdString(vendor),
        QString::fromStdString(library),
        QString::fromStdString(name),
        QString::fromStdString(version)));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createComponent()
//-----------------------------------------------------------------------------
bool PythonAPI::createComponent(std::string const& vendor, std::string const& library, std::string const& name,
    std::string const& version)
{
    if (vendor.empty() || library.empty() || name.empty() || version.empty())
    {
        messager_->showError("Error in given VLNV.");
        return false;
    }

    VLNV newComponentVLNV;
    newComponentVLNV.setVendor(QString::fromStdString(vendor));
    newComponentVLNV.setLibrary(QString::fromStdString(library));
    newComponentVLNV.setName(QString::fromStdString(name));
    newComponentVLNV.setVersion(QString::fromStdString(version));
    newComponentVLNV.setType(VLNV::COMPONENT);

    if (library_->contains(newComponentVLNV))
    {
        return false;
    }

    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(newComponentVLNV, Document::Revision::Std14));

    component->setHierarchy(KactusAttribute::FLAT);
    component->setFirmness(KactusAttribute::MUTABLE);
    component->setImplementation(KactusAttribute::HW);
    component->setVersion(KactusAPI::getVersionFileString());

    QString directory = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + newComponentVLNV.getVendor() + "/" + newComponentVLNV.getLibrary() + "/" +
        newComponentVLNV.getName() + "/" + newComponentVLNV.getVersion();

    directory += vlnvDir;

    if (!library_->writeModelToFile(directory, component))
    {
        messager_->showError("Error saving file to disk.");
        return false;
    }

    openComponent(newComponentVLNV.toString().toStdString());

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getVLNVDirectory()
//-----------------------------------------------------------------------------
std::string PythonAPI::getVLNVDirectory(std::string const& vendor, std::string const& library,
    std::string const& name, std::string const& version) const
{
    VLNV documentVLNV;
    documentVLNV.setVendor(QString::fromStdString(vendor));
    documentVLNV.setLibrary(QString::fromStdString(library));
    documentVLNV.setName(QString::fromStdString(name));
    documentVLNV.setVersion(QString::fromStdString(version));

    return KactusAPI::getDocumentFilePath(documentVLNV).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getFirstViewName()
//-----------------------------------------------------------------------------
std::string PythonAPI::getFirstViewName() const
{
    if (activeComponent_)
    {
        QStringList componentNames = activeComponent_->getViewNames();
        if (!componentNames.isEmpty())
        {
            return componentNames.first().toStdString();
        }
        else
        {
            QSharedPointer<View> newView(new View("mockView"));
            activeComponent_->getViews()->append(newView);
            return newView->name().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::openComponent()
//-----------------------------------------------------------------------------
bool PythonAPI::openComponent(std::string const& vlnvString)
{
    QString componentVLNV = QString::fromStdString(vlnvString);
    QSharedPointer<Document> componentDocument = getDocument(componentVLNV);
    if (componentDocument)
    {
        QSharedPointer<Component> component = componentDocument.dynamicCast<Component>();
        if (component)
        {
            parameterFinder_->setComponent(component);
            portValidator_->componentChange(component->getViews());
            portsInterface_->setPorts(component->getPorts());

            parameterValidator_->componentChange(component->getChoices(), component->getRevision());
            componentParameterInterface_->setParameters(component->getParameters());
            componentParameterInterface_->setChoices(component->getChoices());

            mapValidator_->componentChange(component);
            mapInterface_->setMemoryMaps(component);

            fileSetInterface_->setFileSets(component->getFileSets());

            busInterface_->setBusInterfaces(component);

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
// Function: PythonAPI::getDocument()
//-----------------------------------------------------------------------------
QSharedPointer<Document> PythonAPI::getDocument(QString const& vlnvString) const
{
    QStringList vlnvArray = vlnvString.split(':');
    if (vlnvArray.size() != 4)
    {
        messager_->showError(QString("The given VLNV %1 is not valid").arg(vlnvString));
        return QSharedPointer<Document>();
    }

    VLNV targetVLNV(VLNV::COMPONENT, vlnvArray.at(0), vlnvArray.at(1), vlnvArray.at(2), vlnvArray.at(3));
    return library_->getModel(targetVLNV);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::closeOpenComponent()
//-----------------------------------------------------------------------------
void PythonAPI::closeOpenComponent()
{
    if (activeComponent_)
    {
        messager_->showMessage(QString("Component %1 is closed").arg(activeComponent_->getVlnv().toString()));
    }

    activeComponent_ = QSharedPointer<Component>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentName()
//-----------------------------------------------------------------------------
std::string PythonAPI::getComponentName() const
{
    if (activeComponent_)
    {
        return activeComponent_->getVlnv().getName().toStdString();
    }
    else
    {
        return std::string();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentDescription()
//-----------------------------------------------------------------------------
std::string PythonAPI::getComponentDescription() const
{
    if (activeComponent_)
    {
        return activeComponent_->getDescription().toStdString();
    }
    else
    {
        return std::string();
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::saveComponent()
//-----------------------------------------------------------------------------
void PythonAPI::saveComponent()
{
    if (activeComponent_)
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
        new RegisterFileValidator(expressionParser_, registerValidator, parameterValidator_,
            Document::Revision::Unknown));

    QSharedPointer<AddressBlockValidator> blockValidator(new AddressBlockValidator(
        expressionParser_, registerValidator, registerFileValidator, parameterValidator_, 
        Document::Revision::Unknown));

    QSharedPointer<SubspaceMapValidator> subspaceValidator(
        new SubspaceMapValidator(expressionParser_, parameterValidator_, Document::Revision::Unknown));

    QSharedPointer<MemoryMapValidator> mapValidator(
        new MemoryMapValidator(expressionParser_, blockValidator, subspaceValidator, QSharedPointer<Component>()));

    mapValidator_ = mapValidator;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::constructMemoryInterface()
//-----------------------------------------------------------------------------
void PythonAPI::constructMemoryInterface()
{
    QSharedPointer<SubspaceMapValidator> subspaceValidator = mapValidator_->getSubspaceValidator();
    QSharedPointer<AddressBlockValidator> blockValidator = mapValidator_->getAddressBlockValidator();
    QSharedPointer<RegisterValidator> registerValidator = blockValidator->getRegisterValidator();
    QSharedPointer<FieldValidator> fieldValidator = registerValidator->getFieldValidator();

    ResetInterface* resetInterface(new ResetInterface(fieldValidator, expressionParser_, expressionFormatter_));
    FieldInterface* fieldInterface(
        new FieldInterface(fieldValidator, expressionParser_, expressionFormatter_, resetInterface));

    ModeReferenceInterface* modeReferenceInterface(new ModeReferenceInterface());

    AccessPolicyInterface* accessPolicyInterface(new AccessPolicyInterface(modeReferenceInterface));

    RegisterInterface* registerInterface(
        new RegisterInterface(registerValidator, expressionParser_, expressionFormatter_, fieldInterface, 
            accessPolicyInterface));

    AddressBlockInterface* blockInterface(new AddressBlockInterface(blockValidator, expressionParser_,
        expressionFormatter_, busInterface_, registerInterface, componentParameterInterface_));
    SubspaceMapInterface* subspaceInterface(new SubspaceMapInterface(
        subspaceValidator, expressionParser_, expressionFormatter_, busInterface_, componentParameterInterface_));

    mapInterface_ = new MemoryMapInterface(mapValidator_, expressionParser_, expressionFormatter_);

    mapInterface_->setAddressBlockInterface(blockInterface);
    mapInterface_->setSubspaceMapInterface(subspaceInterface);
    mapInterface_->setModeReferenceInterface(modeReferenceInterface);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setBlocksForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setBlocksForInterface(std::string const& mapName)
{
    AddressBlockInterface* blockInterface = mapInterface_->getAddressBlockInterface();

    QString mapNameQT = QString::fromStdString(mapName);
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapNameQT);
    if (!containingMap)
    {
        sendMemoryMapNotFoundError(mapNameQT);
        return;
    }

    blockInterface->setMemoryBlocks(containingMap->getMemoryBlocks());
    blockInterface->setAddressUnitBits(containingMap->getAddressUnitBits().toStdString());
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setRegistersForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setRegistersForInterface(std::string const& mapName, std::string const& blockName)
{
    RegisterInterface* interfacePointer = mapInterface_->getAddressBlockInterface()->getSubInterface();

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
    FieldInterface* interfacePointer =
        mapInterface_->getAddressBlockInterface()->getSubInterface()->getSubInterface();

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
        mapInterface_->getAddressBlockInterface()->getSubInterface()->getSubInterface()->getSubInterface();

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
// Function: PythonAPI::constructFileSetInterface()
//-----------------------------------------------------------------------------
void PythonAPI::constructFileSetInterface()
{
    QSharedPointer<FileValidator> newFileValidator(new FileValidator(expressionParser_));
    QSharedPointer<FileSetValidator> newFileSetValidator(
        new FileSetValidator(newFileValidator, expressionParser_));

    FileInterface* fileInterface = new FileInterface(newFileValidator, expressionParser_, expressionFormatter_);

    FileBuilderInterface* fileBuilderInterface = new FileBuilderInterface(expressionParser_, expressionFormatter_);

    fileSetInterface_ = new FileSetInterface(
        newFileSetValidator, expressionParser_, expressionFormatter_, fileInterface, fileBuilderInterface);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setFilesForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setFilesForInterface(std::string const& setName)
{
    FileInterface* fileInterface = fileSetInterface_->getFileInterface();

    QString fileSetNameQT = QString::fromStdString(setName);
    QSharedPointer<FileSet> containingFileSet = getFileSet(fileSetNameQT);
    if (!containingFileSet)
    {
        sendFileSetNotFoundError(fileSetNameQT);
        return;
    }

    fileInterface->setFiles(containingFileSet->getFiles());
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> PythonAPI::getFileSet(QString const& setName) const
{
    for (auto fileSet : *activeComponent_->getFileSets())
    {
        if (fileSet->name() == setName)
        {
            return fileSet;
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setFileBuildersForInterface()
//-----------------------------------------------------------------------------
void PythonAPI::setFileBuildersForInterface(std::string const& setName)
{
    FileBuilderInterface* builderInterface = fileSetInterface_->getFileBuilderInterface();

    QString fileSetNameQT = QString::fromStdString(setName);
    QSharedPointer<FileSet> containingFileSet = getFileSet(fileSetNameQT);
    if (!containingFileSet)
    {
        sendFileSetNotFoundError(fileSetNameQT);
        return;
    }

    builderInterface->setFileBuilders(containingFileSet->getDefaultFileBuilders());
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createDesign()
//-----------------------------------------------------------------------------
bool PythonAPI::createDesign(std::string const& vendor, std::string const& library, 
    std::string const& name, std::string const& version)
{
    if (vendor.empty() || library.empty() || name.empty() || version.empty())
    {
        messager_->showError("Error in given VLNV.");
        return false;
    }

    VLNV componentVLNV(VLNV::DESIGN, 
        QString::fromStdString(vendor), 
        QString::fromStdString(library),
        QString::fromStdString(name),
        QString::fromStdString(version));

    if (createComponent(vendor, library, name, version) == false)
    {
        messager_->showError("Error in creating containing component.");
        return false;
    }

    VLNV designVLNV(VLNV::DESIGN, componentVLNV.getVendor(), componentVLNV.getLibrary(),
        componentVLNV.getName() + ".design", componentVLNV.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, componentVLNV.getVendor(), componentVLNV.getLibrary(),
        componentVLNV.getName()+ ".designcfg", componentVLNV.getVersion());

    if (library_->contains(designVLNV))
    {
        return false;
    }

    QSharedPointer<DesignConfigurationInstantiation> hierarchicalInstantiation
    (new DesignConfigurationInstantiation(desConfVLNV.getName() + "_" + desConfVLNV.getVersion()));

    QSharedPointer<ConfigurableVLNVReference> tempReference(new ConfigurableVLNVReference(desConfVLNV));
    hierarchicalInstantiation->setDesignConfigurationReference(tempReference);

    QSharedPointer<View> newHierarchicalView(new View(QStringLiteral("hierarchical")));
    newHierarchicalView->setDesignConfigurationInstantiationRef(hierarchicalInstantiation->name());

    activeComponent_->getDesignConfigurationInstantiations()->append(hierarchicalInstantiation);
    activeComponent_->getViews()->append(newHierarchicalView);

    auto design = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));
    design->setVersion(KactusAPI::getVersionFileString());
    design->setDesignImplementation(KactusAttribute::HW);


    auto designConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(desConfVLNV, Document::Revision::Std14));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::HW);
    designConf->setVersion(KactusAPI::getVersionFileString());

    if (QString directory = KactusAPI::getDefaultLibraryPath() + "/" + designVLNV.toString(QStringLiteral("/"));        
        !library_->writeModelToFile(activeComponent_) ||
        !library_->writeModelToFile(directory, design) ||
        !library_->writeModelToFile(directory, designConf))
    {
        messager_->showError("Error saving file to disk.");
        return false;
    }

    openDesign(designVLNV.toString().toStdString());

    return true;
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

//-----------------------------------------------------------------------------
// Function: PythonAPI::sendFileSetNotFoundError()
//-----------------------------------------------------------------------------
void PythonAPI::sendFileSetNotFoundError(QString const& setName) const
{
    messager_->showError(QString("Could not find file set %1 within component %2").
        arg(setName, activeComponent_->getVlnv().toString()));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::openDesign()
//-----------------------------------------------------------------------------
bool PythonAPI::openDesign(std::string const& vlnvString)
{
    QString designVLNV = QString::fromStdString(vlnvString);
    QSharedPointer<Document> designDocument = getDocument(designVLNV);
    if (designDocument)
    {
        QSharedPointer<Design> design = designDocument.dynamicCast<Design>();
        if (design)
        {
            activeDesign_ = design;
            messager_->showMessage(QString("Design %1 is open").arg(designVLNV));

            instanceInterface_->setComponentInstances(activeDesign_);
            connectionInterface_->setInterconnections(activeDesign_);
            adhocConnectionInterface_->setConnections(activeDesign_);

            return true;
        }
        else
        {
            messager_->showError(QString("The given VLNV %1 is not a design").arg(designVLNV));
            return false;
        }
    }
    else
    {
        messager_->showError(QString("Could not open document with VLNV %1").arg(designVLNV));
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::closeOpenDesign()
//-----------------------------------------------------------------------------
void PythonAPI::closeOpenDesign()
{
    if (activeDesign_)
    {
        messager_->showMessage(QString("Design %1 is closed").arg(activeDesign_->getVlnv().toString()));
    }

    activeDesign_ = QSharedPointer<Design>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::saveDesign()
//-----------------------------------------------------------------------------
void PythonAPI::saveDesign()
{
    if (activeDesign_)
    {
        messager_->showMessage(QString("Saving design %1 ...").arg(activeDesign_->getVlnv().toString()));

        if (library_->writeModelToFile(activeDesign_))
        {
            messager_->showMessage(QString("Save complete"));
        }
        else
        {
            messager_->showError(QString("Could not save design %1").arg(activeDesign_->getVlnv().toString()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::addComponentInstance()
//-----------------------------------------------------------------------------
bool PythonAPI::addComponentInstance(std::string const& vlnvString, std::string const& instanceName)
{
    if (!activeDesign_)
    {
        messager_->showMessage(QString("No open design"));
        return false;
    }

    QString combinedVLNV = QString::fromStdString(vlnvString);
    QSharedPointer<Document> instanceDocument = getDocument(combinedVLNV);
    if (!instanceDocument)
    {
        messager_->showMessage(QString("Could not find document %1").arg(combinedVLNV));
        return false;
    }

    QSharedPointer<Component> instanceComponent = instanceDocument.dynamicCast<Component>();
    if (!instanceComponent)
    {
        messager_->showMessage(QString("%1 is not a component").arg(combinedVLNV));
        return false;
    }

    QStringList vlnvList = combinedVLNV.split(":");
    if (vlnvList.size() < 4)
    {
        messager_->showMessage(QString("The VLNV %1 is not correct").arg(combinedVLNV));
        return false;
    }

    std::string newVendor = vlnvList.at(0).toStdString();
    std::string newLibrary = vlnvList.at(1).toStdString();
    std::string newName = vlnvList.at(2).toStdString();
    std::string newVersion = vlnvList.at(3).toStdString();

    instanceInterface_->addComponentInstance(instanceName);
    return instanceInterface_->setComponentReference(instanceName, newVendor, newLibrary, newName, newVersion);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeComponentInstance()
//-----------------------------------------------------------------------------
bool PythonAPI::removeComponentInstance(std::string const& instanceName)
{
    if (!activeDesign_)
    {
        messager_->showMessage(QString("No open design"));
        return false;
    }

    return instanceInterface_->removeComponentInstance(instanceName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeInstanceConnections()
//-----------------------------------------------------------------------------
bool PythonAPI::removeInstanceConnections(std::string const& instanceName)
{
    return connectionInterface_->removeInstanceInterconnections(instanceName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeInstanceAdHocConnections()
//-----------------------------------------------------------------------------
bool PythonAPI::removeInstanceAdHocConnections(std::string const& instanceName)
{
    return adhocConnectionInterface_->removeInstanceAdHocConnections(instanceName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::renameInstance()
//-----------------------------------------------------------------------------
bool PythonAPI::renameInstance(std::string const& currentName, std::string const& newName)
{
    return instanceInterface_->setName(currentName, newName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::createConnection(std::string const& startInstanceName, std::string const& startBus,
    std::string const& endInstanceName, std::string const& endBus)
{
    QString startInstanceNameQ = QString::fromStdString(startInstanceName);
    QString endInstanceNameQ = QString::fromStdString(endInstanceName);
    QString startBusNameQ = QString::fromStdString(startBus);
    QString endBusNameQ = QString::fromStdString(endBus);

    if (!connectionEndsCheck(startInstanceNameQ, startBusNameQ, endInstanceNameQ, endBusNameQ, false))
    {
        messager_->showMessage(QString("Could not create connection"));
        return false;
    }

    connectionInterface_->addInterconnection(startInstanceName, startBus, endInstanceName, endBus);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createHierarchicalConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::createHierarchicalConnection(std::string const& instanceName, std::string const& instanceBus,
    std::string const& topBus)
{
    QString instanceNameQ = QString::fromStdString(instanceName);
    QString instanceBusNameQ = QString::fromStdString(instanceBus);
    QString topBusQ = QString::fromStdString(topBus);

    if (!instanceExists(instanceNameQ))
    {
        messager_->showMessage(QString("Could not create connection"));
        return false;
    }

    connectionInterface_->addHierarchicalInterconnection(instanceName, instanceBus, topBus);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::connectionExistenceCheck()
//-----------------------------------------------------------------------------
bool PythonAPI::connectionEndsCheck(QString const& startInstanceName, QString const& startBus,
    QString const& endInstanceName, QString const& endBus, bool isAdHocConnection)
{
    if (instanceExists(startInstanceName) == false || instanceExists(endInstanceName) == false)
    {
        return false;
    }

    QSharedPointer<ConfigurableVLNVReference> startComponenReference =
        instanceInterface_->getComponentReference(startInstanceName.toStdString());
    QSharedPointer<ConfigurableVLNVReference> endComponentReference =
        instanceInterface_->getComponentReference(endInstanceName.toStdString());
    QSharedPointer<const Document> startDocument = library_->getModelReadOnly(*startComponenReference.data());
    QSharedPointer<const Document> endDocument = library_->getModelReadOnly(*endComponentReference.data());
    QSharedPointer<const Component> startComponent = startDocument.dynamicCast<const Component>();
    QSharedPointer<const Component> endComponent = endDocument.dynamicCast<const Component>();


    if (isAdHocConnection)
    {
        return endsCheckForAdHoc(
            startComponent, startBus, startInstanceName, endComponent, endBus, endInstanceName);
    }
    else
    {
        return endsCheckForInterconnection(
            startComponent, startBus, startInstanceName, endComponent, endBus, endInstanceName);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::instanceExists()
//-----------------------------------------------------------------------------
bool PythonAPI::instanceExists(QString const& instanceName) const
{
    if (!instanceInterface_->instanceExists(instanceName.toStdString()))
    {
        messager_->showMessage(QString("Could not find component instance %1 within %2").
            arg(instanceName, activeDesign_->getVlnv().toString()));
        return false;
    }

    QSharedPointer<ConfigurableVLNVReference> instanceComponenReference =
        instanceInterface_->getComponentReference(instanceName.toStdString());

    if (!instanceComponenReference)
    {
        messager_->showMessage(QString("Component instance %1 does not have a component reference").
            arg(instanceName));
        return false;
    }

    QSharedPointer<const Document> instanceDocument =
        library_->getModelReadOnly(*instanceComponenReference.data());
    if (!instanceDocument)
    {
        messager_->showMessage(QString("Component instance %1 references a non-existing document %2").
            arg(instanceName, instanceComponenReference->toString()));
        return false;
    }

    QSharedPointer<const Component> instanceComponent = instanceDocument.dynamicCast<const Component>();
    if (!instanceComponent)
    {
        messager_->showMessage(QString("Component instance %1 component reference %1 is not a component").
            arg(instanceName, instanceComponenReference->toString()));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::endsCheckForAdHoc()
//-----------------------------------------------------------------------------
bool PythonAPI::endsCheckForAdHoc(QSharedPointer<const Component> startComponent, QString const& startBus,
    QString const& startInstanceName, QSharedPointer<const Component> endComponent, QString const& endBus,
    QString const& endInstanceName)
{
    openComponent(startComponent->getVlnv().toString().toStdString());

    DirectionTypes::Direction startDirection =
        DirectionTypes::str2Direction(QString::fromStdString(portsInterface_->getDirection(
            startBus.toStdString())), DirectionTypes::DIRECTION_INVALID);

    if (!portsInterface_->portExists(startBus.toStdString()))
    {
        messager_->showMessage(QString("Could not find port %1 within component instance %2.").
            arg(startBus, startInstanceName));
        return false;
    }

    openComponent(endComponent->getVlnv().toString().toStdString());
    DirectionTypes::Direction endDirection = DirectionTypes::str2Direction(QString::fromStdString(
        portsInterface_->getDirection(endBus.toStdString())), DirectionTypes::DIRECTION_INVALID);

    if (!portsInterface_->portExists(endBus.toStdString()))
    {
        messager_->showMessage(QString("Could not find port %1 within component instance %2.").
            arg(endBus, endInstanceName));
        return false;
    }

    closeOpenComponent();

    if (startDirection == DirectionTypes::DIRECTION_INVALID ||
        endDirection == DirectionTypes::DIRECTION_INVALID ||
        (startDirection == DirectionTypes::IN && endDirection == DirectionTypes::IN) ||
        (startDirection == DirectionTypes::OUT && endDirection == DirectionTypes::OUT))
    {
        messager_->showMessage(QString("Ports %1 in %2 and %3 in %4 have incompatible directions %5 and %6.").
            arg(startBus, startInstanceName, endBus, endInstanceName,
                DirectionTypes::direction2Str(startDirection), DirectionTypes::direction2Str(endDirection)));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::endsCheckForInterconnection()
//-----------------------------------------------------------------------------
bool PythonAPI::endsCheckForInterconnection(QSharedPointer<const Component> startComponent,
    QString const& startBus, QString const& startInstanceName, QSharedPointer<const Component> endComponent,
    QString const& endBus, QString const& endInstanceName)
{
    openComponent(startComponent->getVlnv().toString().toStdString());

    ConfigurableVLNVReference startBusType = busInterface_->getBusType(startBus.toStdString());
    General::InterfaceMode startMode = busInterface_->getMode(startBus.toStdString());
    General::InterfaceMode startMonitor = busInterface_->getMonitorMode(startBus.toStdString());

    if (!busInterface_->busInterfaceExists(startBus.toStdString()))
    {
        messager_->showMessage(QString("Could not find bus interface %1 within component instance %2.").
            arg(startBus, startInstanceName));
        return false;
    }

    openComponent(endComponent->getVlnv().toString().toStdString());

    ConfigurableVLNVReference endBusType = busInterface_->getBusType(endBus.toStdString());
    General::InterfaceMode endMode = busInterface_->getMode(endBus.toStdString());
    General::InterfaceMode endMonitor = busInterface_->getMonitorMode(endBus.toStdString());

    if (!busInterface_->busInterfaceExists(endBus.toStdString()))
    {
        messager_->showMessage(QString("Could not find bus interface %1 within component instance %2.").
            arg(endBus, endInstanceName));
        return false;
    }

    closeOpenComponent();

    if (!BusInterfaceUtilities::busDefinitionVLNVsMatch(startBusType, endBusType, library_))
    {
        messager_->showMessage(QString("Bus interfaces %1 in %2 and %3 in %4 are not of the same bus type").
            arg(startBus, startInstanceName, endBus, endInstanceName));
        return false;
    }

    QVector<General::InterfaceMode> startCompatibleModes =
        General::getCompatibleInterfaceModesForActiveInterface(startMode);

    if ((startMode == General::MONITOR && startMonitor != endMode) ||
        (endMode == General::MONITOR && endMonitor != startMode) ||
        !startCompatibleModes.contains(endMode))
    {
        QString startModeString = General::interfaceMode2Str(startMode);
        QString endModeString = General::interfaceMode2Str(endMode);
        if (startMode == General::MONITOR)
        {
            startModeString = General::interfaceMode2Str(startMonitor);
        }
        if (endMode == General::MONITOR)
        {
            endModeString = General::interfaceMode2Str(endMonitor);
        }

        messager_->showMessage(QString(
            "Bus interface modes of %1 in %2 and %3 in %4 have incompatible bus interface modes %5 and %6.").
            arg(startBus, startInstanceName, endBus, endInstanceName, startModeString, endModeString));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeInstanceConnection(std::string const& startInstanceName, std::string const& startBus,
    std::string const& endInstanceName, std::string const& endBus)
{
    std::string connectionName =
        connectionInterface_->getConnectionName(startInstanceName, startBus, endInstanceName, endBus);

    return removeConnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeHierarchicalConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeHierarchicalConnection(std::string const& instanceName, std::string const& instanceBus,
    std::string const& topBus)
{
    std::string connectionName =
        connectionInterface_->getHierarchicalConnectionName(instanceName, instanceBus, topBus);

    return removeConnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeConnection(std::string const& connectionName)
{
    bool success = connectionInterface_->removeInterconnection(connectionName);

    if (success == false)
    {
        messager_->showMessage(QString("Could not find connection %1.").
            arg(QString::fromStdString(connectionName)));
    }

    return success;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::renameConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::renameConnection(std::string const& currentName, std::string const& newName)
{
    return connectionInterface_->setName(currentName, newName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::createAdHocConnection(std::string const& startInstanceName, std::string const& startPort,
    std::string const& endInstanceName, std::string const& endPort)
{
    QString startInstanceNameQ = QString::fromStdString(startInstanceName);
    QString endInstanceNameQ = QString::fromStdString(endInstanceName);
    QString startPortNameQ = QString::fromStdString(startPort);
    QString endPortNameQ = QString::fromStdString(endPort);

    if (!connectionEndsCheck(startInstanceNameQ, startPortNameQ, endInstanceNameQ, endPortNameQ, true))
    {
        messager_->showMessage(QString("Could not create connection"));
        return false;
    }

    adhocConnectionInterface_->addAdHocConnection(startInstanceName, startPort, endInstanceName, endPort);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::createHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::createHierarchicalAdHocConnection(std::string const& instanceName, std::string const& instancePort,
    std::string const& topPort)
{
    QString instanceNameQ = QString::fromStdString(instanceName);
    QString instancePortQ = QString::fromStdString(instancePort);
    QString topPortQ = QString::fromStdString(topPort);

    if (!instanceExists(instanceNameQ))
    {
        messager_->showMessage(QString("Could not create connection"));
        return false;
    }

    adhocConnectionInterface_->addHierarchicalAdHocConnection(instanceName, instancePort, topPort);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeInstanceAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeInstanceAdHocConnection(std::string const& startInstanceName, std::string const& startPort,
    std::string const& endInstanceName, std::string const& endPort)
{
    std::string connectionName =
        adhocConnectionInterface_->getConnectionName(startInstanceName, startPort, endInstanceName, endPort);

    return removeAdHocConnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeAdHocConnection(std::string const& connectionName)
{
    bool success = adhocConnectionInterface_->removeAdHocConnection(connectionName);
    if (success == false)
    {
        messager_->showMessage(QString("Could not find connection %1.").
            arg(QString::fromStdString(connectionName)));
    }

    return success;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::removeHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::removeHierarchicalAdHocConnection(std::string const& instanceName, std::string const& instancePort,
    std::string const& topPort)
{
    std::string connectionName =
        adhocConnectionInterface_->getHierarchicalConnectionName(instanceName, instancePort, topPort);

    return removeAdHocConnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::renameAdHocConnection()
//-----------------------------------------------------------------------------
bool PythonAPI::renameAdHocConnection(std::string const& currentName, std::string const& newName)
{
    return adhocConnectionInterface_->setName(currentName, newName);
}
