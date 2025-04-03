#include "InterconnectAddressHelper.h"

#include <QHash>
#include <QString>
#include <QSharedPointer>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Component.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

InterconnectAddressHelper::InterconnectAddressHelper(VLNV designVLNV,
    LibraryHandler* library, MessageMediator* messager)
    : library_(library),
    messager_(messager), 
    expressionParser_(0),
    expressionFormatter_(0),
    accessPolicyInterface_(0),
    busInfInterface_(0),
    fieldInterface_(0),
    modeRefInterface_(0),
    registerInterface_(0),
    resetInterface_(0)
{
    QSharedPointer<Document> designCompDocument = library_->getModel(designVLNV);

    QSharedPointer<Component> designComp = designCompDocument.dynamicCast<Component>();
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > list = designComp->getDesignConfigurationInstantiations();
    QSharedPointer<DesignConfigurationInstantiation> inst = list->at(0);
    QSharedPointer<ConfigurableVLNVReference> vlnv = inst->getDesignConfigurationReference();

    QSharedPointer<Document> designConfDocument = library_->getModel(*vlnv);
    QSharedPointer<DesignConfiguration> designConfig = designConfDocument.dynamicCast<DesignConfiguration>();

    QSharedPointer<Document> designDocument = library_->getModel(designConfig->getDesignRef());
    QSharedPointer<Design> design = designDocument.dynamicCast<Design>();

    instanceInterface_->setComponentInstances(design);
    listParameterFinder_ = QSharedPointer<ListParameterFinder>::create();
    expressionFormatter_ = QSharedPointer<ExpressionFormatter>::create(listParameterFinder_);
    expressionParser_ = QSharedPointer<IPXactSystemVerilogParser>::create(listParameterFinder_);

}

void InterconnectAddressHelper::reset() {
    usedStartAddresses_.clear();
    nextAvailableAddress_ = 0;
}

bool InterconnectAddressHelper::getTargetAddressRange(const QString& instanceName,
    const QString& interfaceName,
    quint64& outStart,
    quint64& outRange)
{
    outStart = 0;
    outRange = 0;
    QSharedPointer<VLNV> compVLNV = instanceInterface_->getComponentReference(instanceName.toStdString());
    if (!compVLNV) return false;

    QSharedPointer<Document> doc = library_->getModel(*compVLNV);
    if (!doc) return false;

    QSharedPointer<QList<QSharedPointer<Parameter>>> parameters = doc->getParameters();
    listParameterFinder_->setParameterList(parameters);

    QSharedPointer<Component> component = doc.dynamicCast<Component>();
    if (!component) return false;

    initialize(component);
    memoryMapInterface_->setMemoryMaps(component);

    std::vector<std::string> mapNames = memoryMapInterface_->getItemNames();
    QString matchingMap;
    for (const std::string& nameStr : mapNames) {
        std::string boundIf = memoryMapInterface_->getInterfaceBinding(nameStr);
        if (boundIf == interfaceName.toStdString()) {
            matchingMap = QString::fromStdString(nameStr);
            break;
        }
    }

    if (matchingMap.isEmpty()) return false;

    MemoryMap* map = memoryMapInterface_->getMapPointer(matchingMap.toStdString());
    if (!map || !map->getMemoryBlocks()) return false;
    addressBlockInterface_->setMemoryBlocks(map->getMemoryBlocks());
    addressBlockInterface_->setAddressUnitBits(memoryMapInterface_->getAddressUnitBitsValue(matchingMap.toStdString()));
    addressBlockInterface_->setupSubInterfaces(component);

    std::vector<std::string> blockNames = addressBlockInterface_->getItemNames();
    quint64 totalRange = 0;

    for (const std::string& blockName : blockNames) {
        QString nameStr = QString::fromStdString(blockName);
        QString blockRangeStr = QString::fromStdString(addressBlockInterface_->getRangeValue(blockName));

        bool ok = false;
        quint64 blockRange = blockRangeStr.toULongLong(&ok, 0);
        if (ok) {
            totalRange += blockRange;
        }
    }

    if (totalRange == 0) return false;

    if (usedStartAddresses_.contains(instanceName)) {
        outStart = usedStartAddresses_.value(instanceName);
    }
    else {
        outStart = nextAvailableAddress_;
        usedStartAddresses_.insert(instanceName, outStart);
        nextAvailableAddress_ += totalRange;
    }

    outRange = totalRange;
    return true;
}

void InterconnectAddressHelper::initialize(QSharedPointer<Component> component) {
    if (component == currentComponent_ && memoryMapInterface_ && addressBlockInterface_) return;
    currentComponent_ = component;
    createValidators(component);
    createInterfaces(component);
}

void InterconnectAddressHelper::createValidators(QSharedPointer<Component> component) {
    parameterValidator_ = QSharedPointer<ParameterValidator>::create(
        expressionParser_, component->getChoices(), component->getRevision());

    enumValidator_ = QSharedPointer<EnumeratedValueValidator>::create(expressionParser_);

    fieldValidator_ = QSharedPointer<FieldValidator>::create(
        expressionParser_, enumValidator_, parameterValidator_);

    registerValidator_ = QSharedPointer<RegisterValidator>::create(
        expressionParser_, fieldValidator_, parameterValidator_);

    registerFileValidator_ = QSharedPointer<RegisterFileValidator>::create(
        expressionParser_, registerValidator_, parameterValidator_, component->getRevision());

    addressBlockValidator_ = QSharedPointer<AddressBlockValidator>::create(
        expressionParser_, registerValidator_, registerFileValidator_, 
        parameterValidator_, component->getRevision());

    subspaceValidator_ = QSharedPointer<SubspaceMapValidator>::create(
        expressionParser_, parameterValidator_, component->getRevision());

    memoryMapValidator_ = QSharedPointer<MemoryMapValidator>::create(
        expressionParser_, addressBlockValidator_, subspaceValidator_, component);
    memoryMapValidator_->componentChange(component);

    portMapValidator_ = QSharedPointer<PortMapValidator>::create(expressionParser_, component->getPorts(), library_);

    busValidator_ = QSharedPointer<BusInterfaceValidator>::create(expressionParser_,
        component->getChoices(), component->getViews(), component->getPorts(), component->getAddressSpaces(),
        component->getMemoryMaps(), component->getBusInterfaces(), component->getFileSets(), component->getRemapStates(),
        component->getModes(),
        portMapValidator_, parameterValidator_, library_);

}

void InterconnectAddressHelper::createInterfaces(QSharedPointer<Component> component) 
{
    resetInterface_ = new ResetInterface(fieldValidator_, expressionParser_, expressionFormatter_);
    fieldInterface_ = new FieldInterface(fieldValidator_, expressionParser_, expressionFormatter_, resetInterface_);

    modeRefInterface_ = new ModeReferenceInterface();
    modeRefInterface_->setComponentModes(component->getModes());

    accessPolicyInterface_ = new AccessPolicyInterface(modeRefInterface_);
    accessPolicyInterface_->setComponentModes(component->getModes());

    busInfInterface_ = new BusInterfaceInterface(busValidator_, expressionParser_, expressionFormatter_);

    registerInterface_ = new RegisterInterface(
        registerValidator_, expressionParser_, expressionFormatter_, fieldInterface_, accessPolicyInterface_);

    parameterInterface_ = QSharedPointer<ParametersInterface>::create(
        parameterValidator_, expressionParser_, expressionFormatter_);

    addressBlockInterface_ = QSharedPointer<AddressBlockInterface>::create(
        addressBlockValidator_, expressionParser_, expressionFormatter_, 
        busInfInterface_, registerInterface_, parameterInterface_.data());

    memoryMapInterface_ = QSharedPointer<MemoryMapInterface>::create(
        memoryMapValidator_, expressionParser_, expressionFormatter_);
    memoryMapInterface_->setAddressBlockInterface(addressBlockInterface_.data());
}
