#include "InterconnectGeneratorDialog.h"
#include "InterconnectDataModel.h"

#include <QMessageBox>

InterconnectGeneratorDialog::InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
    MessageMediator* messager, QWidget* parent)
    : QDialog(parent),
    library_(library),
    messager_(messager),
    designWidget_(designWidget),
    initiatorsContainerLayout_(new QVBoxLayout()),
    targetsContainerLayout_(new QVBoxLayout()),
    vlnvEditor_(new VLNVEditor(VLNV::BUSDEFINITION, library, this, this))
{
    setMinimumWidth(900);
    setMinimumHeight(900);

    designVLNV_ = designWidget->getEditedComponent()->getVlnv().toString(":");

    QWidget* hiddenWidget = new QWidget(this);
    hiddenWidget->hide();
    componentInstances_ = new InstanceComboBox(hiddenWidget);

    auto parameters = QSharedPointer<QList<QSharedPointer<Parameter>>>::create();
    auto choices = QSharedPointer<QList<QSharedPointer<Choice>>>::create();

    QSharedPointer<ListParameterFinder> listFinder = QSharedPointer<ListParameterFinder>::create();
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(listFinder));

    parameterGroupBox_ = new ParameterGroupBox(parameters, choices, listFinder, expressionFormatter,
        designWidget_->getEditedComponent()->getRevision(), this);
    parameterGroupBox_->setTitle("Interconnect Component Parameters");

    dataModel_ = QSharedPointer<InterconnectDataModel>::create(designWidget_, library_);
    dataModel_->gatherBusAndAbstractionData(initiatorModes_, targetModes_);

    addressHelper_ = QSharedPointer<InterconnectAddressHelper>::create(
        designWidget->getEditedComponent()->getVlnv(), 
        library_, messager_);

    filteredAbsRefs_ = dataModel_->getValidAbstractionRefs();
    allAbsRefs_ = dataModel_->getAllAbstractionRefs();
    instanceBusesHash_ = dataModel_->getInstanceBusMap();
    interfaceAbsDefsHash_ = dataModel_->getInterfaceAbstractionHash();

    setUpLayout();
}

ConfigStruct* InterconnectGeneratorDialog::getConfig() {
    return config_;
}

QHash<QString, QList<QSharedPointer<BusInterface>>> InterconnectGeneratorDialog::getSelectedInitiators() {
    return selectedInitiators_;
}

QHash<QString, QList<QSharedPointer<TargetData>>> InterconnectGeneratorDialog::getSelectedTargets() {
    return selectedTargets_;
}

void InterconnectGeneratorDialog::populateParameters()
{
    QSharedPointer<ConfigurableVLNVReference> absRef;
    for (auto& ref : filteredAbsRefs_) {
        if (ref->getName() == busInterfaceCombo_->currentText()) {
            absRef = ref;
        }
    }
    if (!absRef) 
    {
        return;
    }
    QSharedPointer<Document> libComp = library_->getModel(*absRef);
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

    interconnectParams_ = QSharedPointer<QList<QSharedPointer<Parameter>>>::create();
    for (const auto& param : *absDef->getParameters()) {
        interconnectParams_->append(QSharedPointer<Parameter>::create(*param));
    }

    auto copiedChoices = QSharedPointer<QList<QSharedPointer<Choice>>>::create();
    for (const auto& choice : *absDef->getChoices()) {
        copiedChoices->append(QSharedPointer<Choice>::create(*choice));
    }

    parameterGroupBox_->setNewParameters(interconnectParams_, copiedChoices, absDef->getRevision());
}

void InterconnectGeneratorDialog::addNewInitiator() {
    addNewInstance("Initiator");
}

void InterconnectGeneratorDialog::addNewTarget() {
    addNewInstance("Target");
}

void InterconnectGeneratorDialog::addNewInstance(const QString& type) {
    QFrame* instanceFrame = createInstanceEditorFrame(type);
    if (!instanceFrame) return;
    QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
    if (!nameCombo) return;

    QString currentText = nameCombo->currentText();

    if (type == "Initiator") {
        initiatorsContainerLayout_->addWidget(instanceFrame);
        addedInitiators_.insert(currentText);
    }
    else {
        targetsContainerLayout_->addWidget(instanceFrame);
        addedTargets_.insert(currentText);
    }

    connect(nameCombo, &QComboBox::currentTextChanged, this,
        [=](const QString& newText) mutable {
            if (type == "Initiator") {
                addedInitiators_.remove(currentText);
                addedInitiators_.insert(newText);
            }
            else {
                addedTargets_.remove(currentText);
                addedTargets_.insert(newText);
            }
            currentText = newText;
        });
}

void InterconnectGeneratorDialog::updateNameCombo(QComboBox* nameCombo, const QString& instanceType, QStringList& availableInstances)
{
    availableInstances.clear();
    QString selectedAbsDef = busInterfaceCombo_->currentText();
    const QSet<General::InterfaceMode>& validModes = (instanceType == "Initiator") ? initiatorModes_ : targetModes_;
    const QSet<QString>& addedInstances = (instanceType == "Initiator") ? addedInitiators_ : addedTargets_;

    for (const QString& instanceName : instanceBusesHash_.keys()) {
        if (addedInstances.contains(instanceName)) {
            continue;
        }
        QSet<QSharedPointer<BusInterface>> busInterfaces = instanceBusesHash_.value(instanceName);
        bool hasMatchingBus = false;

        for (const QSharedPointer<BusInterface>& bus : busInterfaces) {
            if (interfaceAbsDefsHash_.contains(bus) && interfaceAbsDefsHash_.value(bus).contains(selectedAbsDef)) {
                General::InterfaceMode interfaceMode = bus->getInterfaceMode();
                if (validModes.contains(interfaceMode)) {
                    hasMatchingBus = true;
                    break;
                }
            }
        }
        if (hasMatchingBus) {
            availableInstances.append(instanceName);
        }
    }
    if (nameCombo != nullptr) {
        nameCombo->clear();
        nameCombo->addItems(availableInstances);
    }
}

void InterconnectGeneratorDialog::onInstanceSelected(const QString& instanceName, const QString& type, InterfaceEnumEditor* interfaceEditor)
{
    QStringList busInterfaceNames;
    const QSet<General::InterfaceMode>& validModes = (type == "Initiator") ? initiatorModes_ : targetModes_;

    if (instanceBusesHash_.contains(instanceName)) {
        QSet<QSharedPointer<BusInterface>> busInterfaces = instanceBusesHash_.value(instanceName);
        for (const QSharedPointer<BusInterface>& bus : busInterfaces) {
            General::InterfaceMode mode = bus->getInterfaceMode();
            if (validModes.contains(mode)) {
                busInterfaceNames.append(bus->name());
            }
        }
    }
    interfaceEditor->clearAll();
    bool isTargets = (type == "Target") ? true : false;
    interfaceEditor->addItems(busInterfaceNames, isTargets, instanceName);
}

QFrame* InterconnectGeneratorDialog::createInstanceEditorFrame(const QString& type) {
    QStringList availableInstances;
    updateNameCombo(nullptr, type, availableInstances);

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("No Instances Available"), tr("All instances have already been added."));
        return nullptr;
    }

    QString instanceName = availableInstances.first();
    QFrame* instanceFrame = new QFrame();
    instanceFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    InterfaceEnumEditor* interfaceEditor = new InterfaceEnumEditor(this);

    connect(interfaceEditor, &InterfaceEnumEditor::targetInterfaceChecked,
        this, [=](const QString& interfaceName, const QString& instanceName) {
            quint64 start = 0;
            quint64 range = 0;
            if (addressHelper_->getTargetAddressRange(instanceName, interfaceName, start, range)) {
                interfaceEditor->setTargetInterfaceValues(interfaceName, start, range);
            }
        });
    connect(interfaceEditor, &InterfaceEnumEditor::targetInterfaceUnchecked,
        this, [=](const QString& interfaceName, const QString& instanceName) {
            addressHelper_->releaseTargetAddress(instanceName);
            interfaceEditor->clearTargetInterfaceValues(interfaceName);
        });

    QVBoxLayout* nameAndEditorLayout = createNameAndEnumerationEditorLayout(type, interfaceEditor, instanceFrame);

    QFormLayout* instanceLayout = new QFormLayout(instanceFrame);
    instanceLayout->addRow("Instance Name:", nameAndEditorLayout);
    instanceLayout->addRow(interfaceEditor);

    QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
    if (nameCombo && nameCombo->count() > 0) {
        onInstanceSelected(nameCombo->currentText(), type, interfaceEditor);
    }

    connect(nameCombo, &QComboBox::currentTextChanged, this, [=]() {
        QString selectedInstance = nameCombo->currentText();
        onInstanceSelected(selectedInstance, type, interfaceEditor);
        });

    QPushButton* closeButton = instanceFrame->findChild<QPushButton*>();
    connect(closeButton, &QPushButton::clicked, this, [=]() {
        if (type == "Initiator") {
            initiatorsContainerLayout_->removeWidget(instanceFrame);
            addedInitiators_.remove(nameCombo->currentText());
        }
        else {
            targetsContainerLayout_->removeWidget(instanceFrame);
            addedTargets_.remove(nameCombo->currentText());
        }
        instanceFrame->deleteLater();
        });

    return instanceFrame;
}

QVBoxLayout* InterconnectGeneratorDialog::createNameAndEnumerationEditorLayout(
    const QString& type, InterfaceEnumEditor* interfaceEditor, QFrame* instanceFrame)
{
    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* nameLayout = new QHBoxLayout();
    InstanceComboBox* nameCombo = new InstanceComboBox();
    QStringList availableInstances;

    updateNameCombo(nameCombo, type, availableInstances);

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("No Instances Available"), 
            tr("All instances have already been added.")
);
        delete instanceFrame;
        return layout;
    }

    QPushButton* closeButton = new QPushButton(QIcon(":/icons/common/graphics/cross.png"), QString());
    closeButton->setText(type == "Initiator" ? tr("Remove Initiator") : tr("Remove Target"));

    nameLayout->addWidget(nameCombo);
    nameLayout->addWidget(closeButton);

    connect(nameCombo, &QComboBox::currentTextChanged, this, [this, nameCombo, type, interfaceEditor]() {
        QString selectedInstance = nameCombo->currentText();
        this->onInstanceSelected(selectedInstance, type, interfaceEditor);
        });

    if (nameCombo->count() > 0) {
        onInstanceSelected(nameCombo->currentText(), type, interfaceEditor);
    }

    layout->addLayout(nameLayout);

    connect(closeButton, &QPushButton::clicked, this, [this, nameCombo, instanceFrame, type]() {
        QString instanceName = nameCombo->currentText();
        if (type == "Initiator") {
            initiatorsContainerLayout_->removeWidget(instanceFrame);
            addedInitiators_.remove(instanceName);
        }
        else {
            targetsContainerLayout_->removeWidget(instanceFrame);
            addedTargets_.remove(instanceName);
        }
        instanceFrame->deleteLater();
        });

    return layout;
}

void InterconnectGeneratorDialog::clearInitiatorAndTargetLists()
{
    QLayoutItem* item;
    while ((item = initiatorsContainerLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    while ((item = targetsContainerLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    addedInitiators_.clear();
    addedTargets_.clear();
    initiatorsContainerLayout_->update();
    targetsContainerLayout_->update();
}

QWidget* InterconnectGeneratorDialog::createTopConfigSection() {
    QGroupBox* configGroup = new QGroupBox(tr("Interconnect Component Configuration"), this);
    QFormLayout* formLayout = new QFormLayout();

    busInterfaceCombo_ = new InstanceComboBox(this);
    clockCombo_ = new InstanceComboBox(this);
    resetCombo_ = new InstanceComboBox(this);
    clockCombo_->setEnabled(false);
    resetCombo_->setEnabled(false);

    clockCheckBox_ = new QCheckBox(tr("Enable Clock"), this);
    resetCheckBox_ = new QCheckBox(tr("Enable Reset"), this);
    clockCheckBox_->setChecked(false);
    resetCheckBox_->setChecked(false);

    for (QSharedPointer<ConfigurableVLNVReference> ref : filteredAbsRefs_) {
        QString name = ref->getName();
        if (busInterfaceCombo_->findText(name) != -1) continue;
        busInterfaceCombo_->addItem(name);
    }

    for (QSharedPointer<ConfigurableVLNVReference> ref : allAbsRefs_) {
        QString name = ref->getName();
        if (clockCombo_->findText(name) != -1) continue;

        clockCombo_->addItem(name);
        resetCombo_->addItem(name);
    }

    previousIndex_ = busInterfaceCombo_->currentIndex();
    populateParameters();

    connect(busInterfaceCombo_, &QComboBox::activated, this, [this](int index) {
        previousIndex_ = busInterfaceCombo_->currentIndex();
        });

    connect(busInterfaceCombo_, &QComboBox::currentIndexChanged, this, [this](int currentIndex) {
        if (ignoreChange_) {
            ignoreChange_ = false;
            return;
        }
        populateParameters();
        if (!addedInitiators_.isEmpty() || !addedTargets_.isEmpty()) {
            int response = QMessageBox::warning(this, tr("Data Loss"),
                tr("All existing initiators and targets will be removed if abstraction definition is changed. Do you want to continue?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (response == QMessageBox::No) {
                ignoreChange_ = true;
                busInterfaceCombo_->setCurrentIndex(previousIndex_);
                return;
            }
            clearInitiatorAndTargetLists();
        }
        previousIndex_ = currentIndex;
        });

    connect(clockCheckBox_, &QCheckBox::stateChanged, this, [this](int state) {
        clockCombo_->setEnabled(state == Qt::Checked);
        });
    connect(resetCheckBox_, &QCheckBox::stateChanged, this, [this](int state) {
        resetCombo_->setEnabled(state == Qt::Checked);
        });

    formLayout->addRow(tr("Abstraction Definition:"), busInterfaceCombo_);
    formLayout->addRow(clockCheckBox_, clockCombo_);
    formLayout->addRow(resetCheckBox_, resetCombo_);

    configGroup->setLayout(formLayout);

    QWidget* container = new QWidget();
    QHBoxLayout* topRowLayout = new QHBoxLayout(container);
    vlnvEditor_->setTitle("Interconnect Component VLNV");
    topRowLayout->addWidget(vlnvEditor_, 1);
    topRowLayout->addWidget(configGroup, 1);

    return container;
}

QGroupBox* InterconnectGeneratorDialog::createInitiatorsSection() {
    QPushButton* addInitiatorButton = new QPushButton(QIcon(":/icons/common/graphics/add.png"), tr("Add New Initiator"));
    connect(addInitiatorButton, &QPushButton::clicked, this, &InterconnectGeneratorDialog::addNewInitiator);

    initiatorsContainerLayout_->setAlignment(Qt::AlignTop);
    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(initiatorsContainerLayout_);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* sectionLayout = new QVBoxLayout();
    sectionLayout->addWidget(addInitiatorButton);
    sectionLayout->addWidget(scrollArea);

    QGroupBox* groupBox = new QGroupBox(tr("Initiators"), this);
    groupBox->setLayout(sectionLayout);
    return groupBox;
}

QGroupBox* InterconnectGeneratorDialog::createTargetsSection() {
    QPushButton* addTargetButton = new QPushButton(QIcon(":/icons/common/graphics/add.png"), tr("Add New Target"));
    connect(addTargetButton, &QPushButton::clicked, this, &InterconnectGeneratorDialog::addNewTarget);

    targetsContainerLayout_->setAlignment(Qt::AlignTop);
    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(targetsContainerLayout_);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* sectionLayout = new QVBoxLayout();
    sectionLayout->addWidget(addTargetButton);
    sectionLayout->addWidget(scrollArea);

    QGroupBox* groupBox = new QGroupBox(tr("Targets"), this);
    groupBox->setLayout(sectionLayout);
    return groupBox;
}

QWidget* InterconnectGeneratorDialog::createInterconnectModeSelector() {
    QGroupBox* modeGroup = new QGroupBox(tr("Interconnect Mode"), this);
    QHBoxLayout* modeLayout = new QHBoxLayout(modeGroup);

    bridgeButton_ = new QRadioButton(tr("Transparent Bridge"), this);
    channelButton_ = new QRadioButton(tr("Channel"), this);
    bridgeButton_->setChecked(true);

    modeLayout->addWidget(bridgeButton_);
    modeLayout->addWidget(channelButton_);
    modeGroup->setLayout(modeLayout);

    connect(bridgeButton_, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            isChannel_ = false;
        }
        });

    connect(channelButton_, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            isChannel_ = true;
        }
        });

    return modeGroup;
}

QDialogButtonBox* InterconnectGeneratorDialog::createButtonBox() {
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(tr("Generate"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InterconnectGeneratorDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InterconnectGeneratorDialog::reject);

    return buttonBox;
}

void InterconnectGeneratorDialog::setUpLayout() {
    setWindowTitle("Interconnect Component Configuration");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createTopConfigSection());
    mainLayout->addWidget(parameterGroupBox_);
    mainLayout->addWidget(createInterconnectModeSelector());

    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    bottomRowLayout->addWidget(createInitiatorsSection());
    bottomRowLayout->addWidget(createTargetsSection());
    mainLayout->addLayout(bottomRowLayout);

    mainLayout->addWidget(createButtonBox());

    setLayout(mainLayout);
}

void InterconnectGeneratorDialog::accept()
{
    ConfigStruct* config = new ConfigStruct;

    if (!vlnvEditor_->isValid()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Select valid VLNV for interconnect component."));
        return;
    }

    QSharedPointer<ConfigurableVLNVReference> absRef;
    QSharedPointer<ConfigurableVLNVReference> clkRef;
    QSharedPointer<ConfigurableVLNVReference> rstRef;
    for (auto& ref : allAbsRefs_) {
        if (ref->getName() == busInterfaceCombo_->currentText()) {
            absRef = ref;
        }
        if (ref->getName() == clockCombo_->currentText()) {
            clkRef = ref;
        }
        if (ref->getName() == resetCombo_->currentText()) {
            rstRef = ref;
        }
    }

    QString interconVLNV = vlnvEditor_->getVLNV().toString();
    QString busVLNV = absRef->toString();
    QString busType = absRef->getName();
    QString clkVLNV = clockCheckBox_->isChecked() ? clkRef->toString() : "";
    QString rstVLNV = resetCheckBox_->isChecked() ? rstRef->toString() : "";
    
    config->DesignVLNV = designVLNV_;
    config->InterconVLNV = interconVLNV;
    config->BusVLNV = busVLNV;
    config->ClkVLNV = clkVLNV;
    config->RstVLNV = rstVLNV;
    config->BusType = busType.split(".abs")[0];
    config->AddressWidth = 32;
    config->IDWidth = 8;
    config->UserWidth = 1;
    config->isChannel = isChannel_;
    config->interconnectParams = *interconnectParams_;

    QList<InitStruct> initiators;
    InitStruct initiator;

    initiator.Index = 0;
    initiator.Name = "i_SysCtrl_peripherals";
    initiator.DataWidth = 32;

    initiators.append(initiator);

    QList<AddressPair> addrList;
    AddressPair addr;
    addr.Start = "0x00000000";
    addr.End = "0x0000ffff";

    addrList.append(addr);
    QList<TargetStruct> targets;
    TargetStruct target;

    target.Index = 0;
    target.Name = "core_imem_bridge";
    target.DataWidth = 32;
    target.AddressRegions = addrList;
    
    targets.append(target);

    config->InitList = initiators;
    config->TargetList = targets;

    QString message = QString("Design VLNV: %1\nInterconnect VLNV: %2\nBus VLNV: %3\nClock VLNV: %4\nReset VLNV: %5\nBus Type: %6\nAddress Width: %7\nID Width: %8\n")
        .arg(config->DesignVLNV)
        .arg(config->InterconVLNV)
        .arg(config->BusVLNV)
        .arg(config->ClkVLNV)
        .arg(config->RstVLNV)
        .arg(config->BusType)
        .arg(config->AddressWidth)
        .arg(config->IDWidth);

    messager_->showMessage(message);
    collectInstances(config);
    config_ = config;
    QDialog::accept();
}

bool InterconnectGeneratorDialog::collectInstances(ConfigStruct* config)
{
    auto instanceBusesLookup = dataModel_->createInstanceBusesLookup();

    collectInitiators(instanceBusesLookup);
    collectTargets(instanceBusesLookup);

    return !config->InitList.isEmpty() || !config->TargetList.isEmpty();
}

void InterconnectGeneratorDialog::collectInitiators(
    const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup)
{
    for (int i = 0; i < initiatorsContainerLayout_->count(); ++i) {
        QLayoutItem* item = initiatorsContainerLayout_->itemAt(i);
        if (QFrame* instanceFrame = qobject_cast<QFrame*>(item->widget())) {
            QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
            InterfaceEnumEditor* interfaceEditor = instanceFrame->findChild<InterfaceEnumEditor*>();
            if (!nameCombo || !interfaceEditor) {
                continue;
            }
            QString instanceName = nameCombo->currentText();
            QStringList selectedInterfaces = interfaceEditor->getSelectedInitiatorInterfaces();

            if (!instanceBusesLookup.contains(instanceName)) {
                continue;
            }
            const QHash<QString, QSharedPointer<BusInterface>>& busLookup = instanceBusesLookup.value(instanceName);
            QList<QSharedPointer<BusInterface>> matchedInterfaces;

            for (const QString& selectedInterfaceName : selectedInterfaces) {
                if (busLookup.contains(selectedInterfaceName)) {
                    matchedInterfaces.append(busLookup.value(selectedInterfaceName));
                }
            }
            // Add to selectedInitiators_ only if there are matched interfaces
            if (!matchedInterfaces.isEmpty()) {
                selectedInitiators_.insert(instanceName, matchedInterfaces);
            }
        }
    }
}

void InterconnectGeneratorDialog::collectTargets(
    const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup)
{
    for (int i = 0; i < targetsContainerLayout_->count(); ++i) {
        QLayoutItem* item = targetsContainerLayout_->itemAt(i);
        if (QFrame* instanceFrame = qobject_cast<QFrame*>(item->widget())) {
            QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
            InterfaceEnumEditor* interfaceEditor = instanceFrame->findChild<InterfaceEnumEditor*>();
            if (!nameCombo || !interfaceEditor) {
                continue;
            }
            QString instanceName = nameCombo->currentText();
            QList<TargetInterfaceData> selectedInterfaces = interfaceEditor->getSelectedTargetInterfaces();

            if (!instanceBusesLookup.contains(instanceName)) {
                continue;
            }
            const QHash<QString, QSharedPointer<BusInterface>>& busLookup = instanceBusesLookup.value(instanceName);
            QList<QSharedPointer<TargetData > > matchedInterfaces;

            for (const TargetInterfaceData& targetInterface : selectedInterfaces) {
                if (busLookup.contains(targetInterface.name)) {
                    QSharedPointer<TargetData > targetData = QSharedPointer<TargetData >(new TargetData);
                    targetData->targetBus = busLookup.value(targetInterface.name);
                    targetData->start = targetInterface.startValue;
                    targetData->range = targetInterface.range;

                    matchedInterfaces.append(targetData);
                }
            }
            if (!matchedInterfaces.isEmpty()) {
                selectedTargets_.insert(instanceName, matchedInterfaces);
            }
        }
    }
}
