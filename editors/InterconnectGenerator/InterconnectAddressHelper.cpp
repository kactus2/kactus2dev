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
    usedAddressRanges_.clear();
    nextAvailableAddress_ = 0;
}

bool InterconnectAddressHelper::getTargetAddressRange(const QString& instanceName,
    const QString& interfaceName,
    quint64& outStart,
    quint64& outRange)
{
    outStart = 0;
    outRange = 0;

    QSharedPointer<Component> component = loadComponentForInstance(instanceName);
    if (!component) return false;

    QString matchingMap = findMatchingMemoryMap(component, interfaceName);
    if (matchingMap.isEmpty()) return false;

    quint64 totalRange = calculateTotalRange(component, matchingMap);
    if (totalRange == 0) return false;

    return assignAddressRange(instanceName, totalRange, outStart, outRange);
}

QSharedPointer<Component> InterconnectAddressHelper::loadComponentForInstance(const QString& instanceName)
{
    QSharedPointer<VLNV> compVLNV = instanceInterface_->getComponentReference(instanceName.toStdString());
    if (!compVLNV) return nullptr;

    QSharedPointer<Document> doc = library_->getModel(*compVLNV);
    if (!doc) return nullptr;

    QSharedPointer<QList<QSharedPointer<Parameter>>> parameters = doc->getParameters();
    listParameterFinder_->setParameterList(parameters);

    QSharedPointer<Component> component = doc.dynamicCast<Component>();
    if (!component) return nullptr;

    initialize(component);
    memoryMapInterface_->setMemoryMaps(component);

    return component;
}

QString InterconnectAddressHelper::findMatchingMemoryMap(QSharedPointer<Component> component, const QString& interfaceName)
{
    std::vector<std::string> mapNames = memoryMapInterface_->getItemNames();

    for (const std::string& nameStr : mapNames) {
        std::string boundIf = memoryMapInterface_->getInterfaceBinding(nameStr);
        if (boundIf == interfaceName.toStdString()) {
            return QString::fromStdString(nameStr);
        }
    }

    return QString();
}

quint64 InterconnectAddressHelper::calculateTotalRange(QSharedPointer<Component> component, const QString& mapName)
{
    MemoryMap* map = memoryMapInterface_->getMapPointer(mapName.toStdString());
    if (!map || !map->getMemoryBlocks()) return 0;

    addressBlockInterface_->setMemoryBlocks(map->getMemoryBlocks());
    addressBlockInterface_->setAddressUnitBits(memoryMapInterface_->getAddressUnitBitsValue(mapName.toStdString()));
    addressBlockInterface_->setupSubInterfaces(component);

    std::vector<std::string> blockNames = addressBlockInterface_->getItemNames();
    quint64 totalRange = 0;

    for (const std::string& blockName : blockNames) {
        QString blockRangeStr = QString::fromStdString(addressBlockInterface_->getRangeValue(blockName));

        bool ok = false;
        quint64 blockRange = blockRangeStr.toULongLong(&ok, 0);
        if (ok) {
            totalRange += blockRange;
        }
    }

    return totalRange;
}

bool InterconnectAddressHelper::assignAddressRange(const QString& instanceName, quint64 totalRange,
    quint64& outStart, quint64& outRange)
{
    if (usedAddressRanges_.contains(instanceName)) {
        const auto& pair = usedAddressRanges_.value(instanceName);
        outStart = pair.first;
        outRange = pair.second;
        return true;
    }

    std::sort(freeAddressSpaces_.begin(), freeAddressSpaces_.end());
    for (int i = 0; i < freeAddressSpaces_.size(); ++i) {
        QPair<quint64, quint64> free = freeAddressSpaces_.at(i);
        if (free.second >= totalRange) {
            outStart = free.first;
            outRange = totalRange;
            usedAddressRanges_.insert(instanceName, qMakePair(outStart, outRange));

            if (free.second == totalRange) {
                freeAddressSpaces_.removeAt(i);
            }
            else {
                freeAddressSpaces_[i].first += totalRange;
                freeAddressSpaces_[i].second -= totalRange;
            }
            return true;
        }
    }

    outStart = nextAvailableAddress_;
    outRange = totalRange;
    usedAddressRanges_.insert(instanceName, qMakePair(outStart, outRange));
    nextAvailableAddress_ += totalRange;
    return true;
}


void InterconnectAddressHelper::releaseTargetAddress(const QString& instanceName) {
    if (!usedAddressRanges_.contains(instanceName)) return;

    auto released = usedAddressRanges_.take(instanceName);
    quint64 releasedStart = released.first;
    quint64 releasedRange = released.second;

    freeAddressSpaces_.append(qMakePair(releasedStart, releasedRange));

    mergeFreeSpaces();
}

void InterconnectAddressHelper::mergeFreeSpaces() {
    std::sort(freeAddressSpaces_.begin(), freeAddressSpaces_.end());

    QList<QPair<quint64, quint64>> merged;
    for (const auto& block : freeAddressSpaces_) {
        if (merged.isEmpty()) {
            merged.append(block);
        }
        else {
            QPair<quint64, quint64>& last = merged.last();
            if (last.first + last.second == block.first) {
                last.second += block.second;
            }
            else {
                merged.append(block);
            }
        }
    }
    freeAddressSpaces_ = merged;
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
