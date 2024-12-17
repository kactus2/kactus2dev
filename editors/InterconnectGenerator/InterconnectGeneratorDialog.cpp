#include "InterconnectGeneratorDialog.h"
#include <QFormLayout>
#include "KactusAPI/include/BusInterfaceInterface.h"
#include "KactusAPI/include/ListParameterFinder.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>

InterconnectGeneratorDialog::InterconnectGeneratorDialog(QSharedPointer<Component> selectedComponent,
    QSharedPointer<Design> selectedDesign, LibraryInterface* library,
    MessageMediator* messager,
    QWidget* parent) : QDialog(parent),
    library_(library),
    messager_(messager),
    instancesContainerLayout_(new QVBoxLayout()),
    initiatorsContainerLayout_(new QVBoxLayout()),
    targetsContainerLayout_(new QVBoxLayout())
{
    setMinimumWidth(900);
    setMinimumHeight(600);

    QString designVLNV = selectedComponent->getVlnv().toString(":");
    designVLNV_ = designVLNV;

    QWidget* hiddenWidget = new QWidget(this);
    hiddenWidget->hide();
    componentInstances_ = new InstanceComboBox(hiddenWidget);

    createComponentInstanceList(selectedDesign);

    QSharedPointer<ListParameterFinder> listFinder =
        QSharedPointer<ListParameterFinder>(new ListParameterFinder());
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(listFinder));

    parameterGroupBox_ = new ParameterGroupBox(parameters, choices, listFinder, expressionFormatter,
        designWidget_->getEditedComponent()->getRevision(), this);
    parameterGroupBox_->setTitle("Interconnect Component Parameters");

    getBusesFromInstances();
    getBusesFromTopComponent();
    setUpLayout();
}

ConfigStruct* InterconnectGeneratorDialog::getConfig()
{
    return config_;
}

QHash<QString, QList<QSharedPointer<BusInterface > > > InterconnectGeneratorDialog::getSelectedInitiators()
{
    return selectedInitiators_;
}

QHash<QString, QList<QSharedPointer<BusInterface > > > InterconnectGeneratorDialog::getSelectedTargets()
{
    return selectedTargets_;
}

void InterconnectGeneratorDialog::getAbstractionDefinitions(
    QSharedPointer<BusInterface> bus, bool fromInstance) {

    QSharedPointer<QList<QSharedPointer<AbstractionType>>> absTypes = bus->getAbstractionTypes();
    if (!absTypes) {
        return;
    }
    for (const QSharedPointer<AbstractionType>& type : *absTypes) {
        if (!type) {
            continue;
        }
        QSharedPointer<ConfigurableVLNVReference> absRef = type->getAbstractionRef();
        if (!absRef) {
            continue;
        }
        absRefs_.insert(absRef);
        if (!interfaceAbsDefsHash_.contains(bus)) {
            interfaceAbsDefsHash_.insert(bus, QSet<QString>());
        }
        interfaceAbsDefsHash_[bus].insert(absRef->getName());
    }
}

void InterconnectGeneratorDialog::getBusesFromInstances()
{
    QList<ComponentItem*> componnetItems = designWidget_->getInstances();
    for (ComponentItem* componentItem : componnetItems) {
        QString instanceName = componentItem->name();
        instanceBusesHash_.insert(instanceName, QSet<QSharedPointer<BusInterface>>());
        QList<ConnectionEndpoint*> endpoints = componentItem->getEndpoints();
        for (ConnectionEndpoint* endpoint : endpoints) {
            QSharedPointer<BusInterface> bus = endpoint->getBusInterface();
            if (!bus) {
                continue;
            }
            instanceBusesHash_[instanceName].insert(bus);
            getAbstractionDefinitions(bus);
        }
    }
}

void InterconnectGeneratorDialog::getBusesFromTopComponent()
{
    QSharedPointer<Component> topComponent = designWidget_->getEditedComponent();
    QString name = topComponent->getVlnv().getName();
    if (!topComponent) {
        return;
    }
    QSharedPointer<QList<QSharedPointer<BusInterface>>> buses = topComponent->getBusInterfaces();
    if (!buses) {
        return;
    }
    instanceBusesHash_.insert(name, QSet<QSharedPointer<BusInterface>>());
    for (QSharedPointer<BusInterface> bus : *buses) {
        instanceBusesHash_[name].insert(bus);
        getAbstractionDefinitions(bus, false);
    }
}

void InterconnectGeneratorDialog::populateParameters()
{
    QSharedPointer<ConfigurableVLNVReference> absRef;
    for (auto& ref : absRefs_) {
        if (ref->getName() == busInterfaceCombo_->currentText()) {
            absRef = ref;
        }
    }
    QSharedPointer<Document> libComp = library_->getModel(*absRef);
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

    parameterGroupBox_->setNewParameters(absDef->getParameters(), absDef->getChoices(),
        absDef->getRevision());
}

void InterconnectGeneratorDialog::addNewInitiator()
{
    addInstance("Initiator");
}

void InterconnectGeneratorDialog::addNewTarget()
{
    addInstance("Target");
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
    interfaceEditor->addItems(busInterfaceNames, isTargets);
}

void InterconnectGeneratorDialog::updateNameCombos()
{
    QList<QComboBox*> nameCombos = instancesContainerLayout_->findChildren<QComboBox*>();
    for (QComboBox* combo : nameCombos)
    {
        combo->clear();
        for (int i = 0; i < componentInstances_->count(); ++i)
        {
            combo->addItem(componentInstances_->itemText(i));
        }
    }
}

void InterconnectGeneratorDialog::addInstance(const QString& type)
{
    QStringList availableInstances;
    updateNameCombo(nullptr, type, availableInstances); 

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("All Instances Added"),
            tr("All available instances have already been added.")
        );
        return;
    }
    QString instanceName = availableInstances.first();
    QFrame* instanceFrame = new QFrame();
    instanceFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    InterfaceEnumEditor* interfaceEditor = new InterfaceEnumEditor(this);
    QVBoxLayout* nameAndEditorLayout = createNameAndEnumerationEditorLayout(type, interfaceEditor, instanceFrame);

    QFormLayout* instanceLayout = new QFormLayout(instanceFrame);
    instanceLayout->addRow("Instance Name:", nameAndEditorLayout);
    instanceLayout->addRow(interfaceEditor);
    
    if (type == "Initiator") {
        initiatorsContainerLayout_->addWidget(instanceFrame);
        addedInitiators_.insert(instanceName);
    }
    else {
        targetsContainerLayout_->addWidget(instanceFrame);
        addedTargets_.insert(instanceName);
    }
    updateNameCombos();
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
    instanceLayout->addRow("Name:", nameCombo);

    dataWidthEdit_ = new QLineEdit("32");

    QLabel* typeLabel = new QLabel(type);

    QVBoxLayout* addressLayout = new QVBoxLayout();
    if (hasAddressRegions)
    {
        QPushButton* addAddressButton = new QPushButton("Add Address Region");
        connect(addAddressButton, &QPushButton::clicked, this, [addressLayout, this]() {
            QHBoxLayout* regionLayout = new QHBoxLayout();

            QLineEdit* startEdit = new QLineEdit();
            QLineEdit* endEdit = new QLineEdit();
            QPushButton* removeAddressButton = new QPushButton(QIcon(":/icons/common/graphics/remove.png"), tr(""));

            connect(removeAddressButton, &QPushButton::clicked, this, [regionLayout, addressLayout]() {
                addressLayout->removeItem(regionLayout);
                QLayoutItem* item;
                while ((item = regionLayout->takeAt(0)) != nullptr) {
                    delete item->widget();
                    delete item;
                }
                delete regionLayout;
                });

            regionLayout->addWidget(new QLabel("Start:"));
            regionLayout->addWidget(startEdit);
            regionLayout->addWidget(new QLabel("End:"));
            regionLayout->addWidget(endEdit);
            regionLayout->addWidget(removeAddressButton);
            addressLayout->addLayout(regionLayout);
            });

        instanceLayout->addRow(addAddressButton);
        instanceLayout->addRow(addressLayout);
    }

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QPushButton* closeButton = new QPushButton(QIcon(":/icons/common/graphics/cross.png"), QString());
    if (type == "Initiator") {
        closeButton->setText(tr("Remove Initiator"));
    }
    else {
        closeButton->setText(tr("Remove Target"));
    }

    connect(closeButton, &QPushButton::clicked, this, [instanceFrame, line, type, this]() {
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
        updateNameCombos();
        });


    if (type == "Initiator")
    {
        initiatorsContainerLayout_->addWidget(instanceFrame);
        initiatorsContainerLayout_->addWidget(line);
    }
    else
    {
        targetsContainerLayout_->addWidget(instanceFrame);
        targetsContainerLayout_->addWidget(line);
    }

    updateNameCombos();
        QString instanceName = nameCombo->currentText();
        addedInstances_.remove(instanceName);
        updateNameCombos();
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
    updateNameCombos();
    initiatorsContainerLayout_->update();
    targetsContainerLayout_->update();
}

void InterconnectGeneratorDialog::updateNameCombos()
{
    QList<QComboBox*> nameCombos = instancesContainerLayout_->findChildren<QComboBox*>();
    for (QComboBox* combo : nameCombos)
    {
        combo->clear();
        for (int i = 0; i < componentInstances_->count(); ++i)
        {
            combo->addItem(componentInstances_->itemText(i));
        }
    }
}

void InterconnectGeneratorDialog::setUpLayout(QString designVLNV)
{
    setWindowTitle("Interconnect Configuration");

    QGroupBox* configGroup = new QGroupBox(tr("Interconnect Component Configuration"), this);
    QFormLayout* formLayout = new QFormLayout();

    QLabel* designLabel = new QLabel(designVLNV, this);
    formLayout->addRow(tr("Design VLNV:"), designLabel);

    interconnectEdit_ = new QLineEdit();
    formLayout->addRow(tr("Interconnect VLNV:"), interconnectEdit_);

    busEdit_ = new QLineEdit();
    formLayout->addRow(tr("Bus VLNV:"), busEdit_);

    clockEdit_ = new QLineEdit();
    formLayout->addRow(tr("Clock VLNV:"), clockEdit_);

    resetEdit_ = new QLineEdit();
    formLayout->addRow(tr("Reset VLNV:"), resetEdit_);

    busTypeCombo_ = new InstanceComboBox();
    busTypeCombo_->addItem("AXI4LITE");
    busTypeCombo_->addItem("AXI4");
    formLayout->addRow(tr("Bus Type:"), busTypeCombo_);

    addressWidthEdit_  = new QLineEdit("32");
    formLayout->addRow(tr("Address Width:"), addressWidthEdit_);

    idWidthEdit_ = new QLineEdit("8");
    formLayout->addRow(tr("ID Width:"), idWidthEdit_);

    configGroup->setLayout(formLayout);

    // Initiators section
    QCheckBox* clockCheckbox = new QCheckBox(tr("Enable Clock"), this);
    QCheckBox* resetCheckbox = new QCheckBox(tr("Enable Reset"), this);
    clockCheckbox->setChecked(true);
    resetCheckbox->setChecked(true);

    for (QSharedPointer<ConfigurableVLNVReference> ref : absRefs_) {
        QString name = ref->getName();
        if (busInterfaceCombo_->findText(name) != -1) {
            continue;
        }
        busInterfaceCombo_->addItem(name);
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

    connect(busInterfaceCombo_, &QComboBox::activated, this, [this](int index) {
        previousIndex_ = busInterfaceCombo_->currentIndex();
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

    QHBoxLayout* topRowLayout = new QHBoxLayout();
    vlnvEditor_->setTitle("Interconnect Component VLNV");
    topRowLayout->addWidget(vlnvEditor_, 1);
    topRowLayout->addWidget(configGroup, 1);

    QPushButton* addInitiatorButton = new QPushButton(
        QIcon(":/icons/common/graphics/add.png"), tr("Add New Initiator"));
    connect(addInitiatorButton, &QPushButton::clicked, this,
        &InterconnectGeneratorDialog::addNewInitiator);

    initiatorsContainerLayout_->setAlignment(Qt::AlignTop);
    targetsContainerLayout_->setAlignment(Qt::AlignTop);

    QWidget* initiatorsScrollWidget = new QWidget();
    initiatorsScrollWidget->setLayout(initiatorsContainerLayout_);

    QScrollArea* initiatorsScrollArea = new QScrollArea();
    initiatorsScrollArea->setWidget(initiatorsScrollWidget);
    initiatorsScrollArea->setWidgetResizable(true);

    QVBoxLayout* initiatorsSectionLayout = new QVBoxLayout();
    initiatorsSectionLayout->addWidget(addInitiatorButton);
    initiatorsSectionLayout->addWidget(initiatorsScrollArea);

    QGroupBox* initiatorsGroup = new QGroupBox(tr("Initiators"), this);
    initiatorsGroup->setLayout(initiatorsSectionLayout);

    QPushButton* addTargetButton = new QPushButton(
        QIcon(":/icons/common/graphics/add.png"), tr("Add New Target"));
    connect(addTargetButton, &QPushButton::clicked, this,
        &InterconnectGeneratorDialog::addNewTarget);

    QWidget* targetsScrollWidget = new QWidget();
    targetsScrollWidget->setLayout(targetsContainerLayout_);

    QScrollArea* targetsScrollArea = new QScrollArea();
    targetsScrollArea->setWidget(targetsScrollWidget);
    targetsScrollArea->setWidgetResizable(true);

    QVBoxLayout* targetsSectionLayout = new QVBoxLayout();
    targetsSectionLayout->addWidget(addTargetButton);
    targetsSectionLayout->addWidget(targetsScrollArea);

    QGroupBox* targetsGroup = new QGroupBox(tr("Targets"), this);
    targetsGroup->setLayout(targetsSectionLayout);

    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    bottomRowLayout->addWidget(initiatorsGroup);
    bottomRowLayout->addWidget(targetsGroup);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topRowLayout);
    mainLayout->addWidget(parameterGroupBox_);
    mainLayout->addLayout(bottomRowLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(tr("Generate"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(configGroup);

    QHBoxLayout* bottomLayout = new QHBoxLayout();  // Create a horizontal layout for Initiator and Target
    bottomLayout->addWidget(initiatorsGroup);
    bottomLayout->addWidget(targetsGroup);

    mainLayout->addLayout(bottomLayout);  // Add the horizontal layout to the main layout
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void InterconnectGeneratorDialog::accept()
{
    ConfigStruct* config = new ConfigStruct;

    // Collect VLNV values
    QString interconVLNV = interconnectEdit_->text();
    QString busVLNV = busEdit_->text();
    QString clkVLNV = clockEdit_->text();
    QString rstVLNV = resetEdit_->text();

    if (!vlnvEditor_->isValid()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Select valid VLNV for interconnect component."));
        return;
    }

    QSharedPointer<ConfigurableVLNVReference> absRef;
    QSharedPointer<ConfigurableVLNVReference> clkRef;
    QSharedPointer<ConfigurableVLNVReference> rstRef;
    for (auto& ref : absRefs_) {
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
    /*
    config->DesignVLNV = designVLNV_;
    config->InterconVLNV = interconVLNV;
    config->BusVLNV = busVLNV;
    config->ClkVLNV = clkVLNV;
    config->RstVLNV = rstVLNV;
    config->BusType = busTypeCombo_->currentText();
    config->AddressWidth = 32;
    config->IDWidth = 8;
    
    "intercon": "test:gen:intercon:1.0",
        "top" : "tuni.fi:subsystem:SysCtrl_SS:1.0",
        "bus" : "tuni.fi:interface:AXI4LITE:1.0",
        "clk" : "tuni.fi:interface:clock:1.0",
        "rst" : "tuni.fi:interface:reset:1.0",
        "Bus type" : "AXI4LITE",
    ¨*/
    config->DesignVLNV = designVLNV_;
    config->InterconVLNV = interconVLNV;
    config->BusVLNV = busVLNV;
    config->ClkVLNV = "tuni.fi:interface:clock:1.0";
    config->RstVLNV = "tuni.fi:interface:reset:1.0";
    config->BusType = busType.split(".abs")[0];
    config->AddressWidth = 32;
    config->IDWidth = 8;
    config->UserWidth = 1;

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

    // messager_->showMessage(message);
    collectInstances(config);
    config_ = config;
    QDialog::accept();
}

bool InterconnectGeneratorDialog::collectInstances(ConfigStruct* config)
{
    auto instanceBusesLookup = createInstanceBusesLookup(instanceBusesHash_);

    collectInitiators(instanceBusesLookup);
    collectTargets(instanceBusesLookup);

    return !config->InitList.isEmpty() || !config->TargetList.isEmpty();
}

QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> InterconnectGeneratorDialog::createInstanceBusesLookup(
    const QHash<QString, QSet<QSharedPointer<BusInterface>>>& instanceBusesHash)
{
    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> instanceBusesLookup;

    for (auto it = instanceBusesHash.constBegin(); it != instanceBusesHash.constEnd(); ++it) {
        QHash<QString, QSharedPointer<BusInterface>> nameToInterfaceMap;
        for (const QSharedPointer<BusInterface>& busInterface : it.value()) {
            nameToInterfaceMap.insert(busInterface->name(), busInterface);
        }
        instanceBusesLookup.insert(it.key(), nameToInterfaceMap);
    }

    return instanceBusesLookup;
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
    return true; // Success
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
            QList<QSharedPointer<BusInterface>> matchedInterfaces;

            for (const TargetInterfaceData& targetInterface : selectedInterfaces) {
                if (busLookup.contains(targetInterface.name)) {
                    matchedInterfaces.append(busLookup.value(targetInterface.name));
                }
            }
            if (!matchedInterfaces.isEmpty()) {
                selectedTargets_.insert(instanceName, matchedInterfaces);
            }
        }
    }
}
