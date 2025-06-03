#include "InterconnectGeneratorDialog.h"
#include "InterconnectDataModel.h"
#include <KactusAPI/KactusAPI.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <QMessageBox>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::InterconnectGeneratorDialog()
//-----------------------------------------------------------------------------
InterconnectGeneratorDialog::InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
    MessageMediator* messager, QWidget* parent)
    : QDialog(parent),
    library_(library),
    messager_(messager),
    designWidget_(designWidget),
    startingPointsLayout_(new QVBoxLayout()),
    endpointsLayout_(new QVBoxLayout()),
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

    dataModel_ = QSharedPointer<InterconnectDataModel>::create(designWidget_, library_, messager_);
    dataModel_->gatherBusAndAbstractionData();

    addressHelper_ = QSharedPointer<InterconnectAddressHelper>::create(
        designWidget->getEditedComponent()->getVlnv(), 
        library_, messager_);

    filteredAbsRefs_ = dataModel_->getValidAbstractionRefs();
    if (filteredAbsRefs_.isEmpty()) {
        messager_->showMessage("empty");
    }
    allAbsRefs_ = dataModel_->getAllAbstractionRefs();
    instanceBusesHash_ = dataModel_->getInstanceBusMap();
    interfaceAbsDefsHash_ = dataModel_->getInterfaceAbstractionHash();

    setUpLayout();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::getConfig()
//-----------------------------------------------------------------------------
ConfigStruct* InterconnectGeneratorDialog::getConfig()  const
{
    return config_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::getSelectedInitiators()
//-----------------------------------------------------------------------------
QHash<QString, QList<QSharedPointer<BusInterface>>> InterconnectGeneratorDialog::getSelectedStartingPoints() const
{
    return selectedStartingPoints_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::getSelectedTargets()
//-----------------------------------------------------------------------------
QHash<QString, QList<QSharedPointer<EndpointData>>> InterconnectGeneratorDialog::getSelectedEndpoints() const
{
    return selectedEndpoints_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::populateParameters()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::addNewStartingPoint()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::addNewStartingPoint()
{
    addNewInstance("StartingPoint");
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::addNewEndpoint()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::addNewEndpoint()
{
    addNewInstance("Endpoint");
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::addNewInstance()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::addNewInstance(const QString& type)
{
    QFrame* instanceFrame = createInstanceEditorFrame(type);
    if (!instanceFrame) return;

    QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
    if (!nameCombo) return;

    QString currentText = nameCombo->currentText();

    if (type == "StartingPoint") {
        startingPointsLayout_->addWidget(instanceFrame);
        addedStartingPoints_.insert(currentText);
    }
    else if (type == "Endpoint") {
        endpointsLayout_->addWidget(instanceFrame);
        addedEndpoints_.insert(currentText);
    }

    connect(nameCombo, &QComboBox::currentTextChanged, this,
        [=](const QString& newText) mutable {
            if (type == "StartingPoint") {
                addedStartingPoints_.remove(currentText);
                addedStartingPoints_.insert(newText);
            }
            else if (type == "Endpoint") {
                addedEndpoints_.remove(currentText);
                addedEndpoints_.insert(newText);
            }
            currentText = newText;
        });
}

InterconnectDataModel::InterconnectType InterconnectGeneratorDialog::getSelectedInterconnectType() const
{
    return isChannel_ ? InterconnectDataModel::InterconnectType::Channel
        : InterconnectDataModel::InterconnectType::Bridge;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::updateNameCombo()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::updateNameCombo(QComboBox* nameCombo, const QString& roleLabel, QStringList& availableInstances)
{
    availableInstances.clear();

    QString selectedAbsDef = busInterfaceCombo_->currentText();
    InterconnectDataModel::InterconnectType icType = getSelectedInterconnectType();

    for (const QString& instanceName : instanceBusesHash_.keys())
    {
        if (addedStartingPoints_.contains(instanceName) || addedEndpoints_.contains(instanceName)) {
            continue;
        }

        InterconnectDataModel::EntityType entityType = (instanceName == designWidget_->getEditedComponent()->getVlnv().getName())
            ? InterconnectDataModel::EntityType::TopComponent
            : InterconnectDataModel::EntityType::Instance;

        const QSet<QSharedPointer<BusInterface>>& busInterfaces = instanceBusesHash_.value(instanceName);

        for (const QSharedPointer<BusInterface>& bus : busInterfaces)
        {
            if (!interfaceAbsDefsHash_.contains(bus) ||
                !interfaceAbsDefsHash_.value(bus).contains(selectedAbsDef))
            {
                continue;
            }

            General::InterfaceMode mode = InterconnectDataModel::normalizeTo2014(bus->getInterfaceMode());

            const QList<InterconnectDataModel::ConnectionRule> validTargets =
                dataModel_->getValidConnectionTargets(entityType, mode, icType);

            if (!validTargets.isEmpty()) {
                availableInstances.append(instanceName);
                break;
            }
        }
    }

    if (nameCombo) {
        nameCombo->clear();
        nameCombo->addItems(availableInstances);
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::onInstanceSelected()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::onInstanceSelected(const QString& instanceName, const QString& roleLabel, InterfaceEnumEditor* interfaceEditor)
{
    QString selectedAbsDef = busInterfaceCombo_->currentText();
    QStringList validInterfaceNames;

    if (!instanceBusesHash_.contains(instanceName)) {
        interfaceEditor->clearAll();
        return;
    }

    InterconnectDataModel::EntityType entityType = (instanceName == designWidget_->getEditedComponent()->getVlnv().getName())
        ? InterconnectDataModel::EntityType::TopComponent
        : InterconnectDataModel::EntityType::Instance;

    InterconnectDataModel::InterconnectType icType = getSelectedInterconnectType();

    const QSet<QSharedPointer<BusInterface>>& busSet = instanceBusesHash_.value(instanceName);
    for (const QSharedPointer<BusInterface>& bus : busSet)
    {
        // Only consider interfaces that use the currently selected abstraction definition
        if (!interfaceAbsDefsHash_.contains(bus) ||
            !interfaceAbsDefsHash_.value(bus).contains(selectedAbsDef))
        {
            continue;
        }

        General::InterfaceMode mode = InterconnectDataModel::normalizeTo2014(bus->getInterfaceMode());
        QList<InterconnectDataModel::ConnectionRule> rules =
            dataModel_->getValidConnectionTargets(entityType, mode, icType);

        if (!rules.isEmpty()) {
            validInterfaceNames.append(bus->name());
        }
    }

    interfaceEditor->clearAll();
    bool isTarget = (roleLabel == "Endpoint");
    interfaceEditor->addItems(validInterfaceNames, isTarget, instanceName);
}

void InterconnectGeneratorDialog::updateInterconnectModeOptions()
{
    // Ensure latest selection state
    auto lookUp = dataModel_->createInstanceBusesLookup();
    collectStartingPoints(lookUp);
    collectEndpoints(lookUp);

    bool bridgeValid = dataModel_->isModeValidForAllConnections(
        selectedStartingPoints_, selectedEndpoints_, InterconnectDataModel::InterconnectType::Bridge);

    bool channelValid = dataModel_->isModeValidForAllConnections(
        selectedStartingPoints_, selectedEndpoints_, InterconnectDataModel::InterconnectType::Channel);

    bridgeButton_->setEnabled(bridgeValid);
    channelButton_->setEnabled(channelValid);

    // Adjust which button is checked if current one became invalid
    if (!bridgeValid && !channelValid) {
        bridgeButton_->setChecked(false);
        channelButton_->setChecked(false);
    }
    else if (!channelValid && channelButton_->isChecked()) {
        bridgeButton_->setChecked(true);
    }
    else if (!bridgeValid && bridgeButton_->isChecked()) {
        channelButton_->setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createInstanceEditorFrame()
//-----------------------------------------------------------------------------
QFrame* InterconnectGeneratorDialog::createInstanceEditorFrame(const QString& roleLabel)
{
    QStringList availableInstances;
    updateNameCombo(nullptr, roleLabel, availableInstances);

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("No Instances Available"), tr("All instances have already been added."));
        return nullptr;
    }

    QString instanceName = availableInstances.first();
    QFrame* instanceFrame = new QFrame();
    instanceFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    InterfaceEnumEditor* interfaceEditor = new InterfaceEnumEditor(this);

    connect(interfaceEditor, &InterfaceEnumEditor::endpointInterfaceChecked, this,
        [=](const QString& interfaceName, const QString& instanceName) {
            quint64 start = 0;
            quint64 range = 0;
            if (addressHelper_->getEndpointAddressRange(instanceName, interfaceName, start, range)) {
                interfaceEditor->setEndpointInterfaceValues(interfaceName, start, range);
            }
            updateInterconnectModeOptions();
        });

    connect(interfaceEditor, &InterfaceEnumEditor::endpointInterfaceUnchecked, this,
        [=](const QString& interfaceName, const QString& instanceName) {
            addressHelper_->releaseEndpointAddress(instanceName);
            interfaceEditor->clearEndpointInterfaceValues(interfaceName);

            updateInterconnectModeOptions();
        });

    connect(interfaceEditor, &InterfaceEnumEditor::startingPointInterfaceChecked, this,
        [=](const QString&, const QString&) {
            updateInterconnectModeOptions();
        });

    connect(interfaceEditor, &InterfaceEnumEditor::startingPointInterfaceUnchecked, this,
        [=](const QString&, const QString&) {
            updateInterconnectModeOptions();
        });

    QVBoxLayout* nameAndEditorLayout = createNameAndEnumerationEditorLayout(roleLabel, interfaceEditor, instanceFrame);

    QFormLayout* instanceLayout = new QFormLayout(instanceFrame);
    instanceLayout->addRow("Instance Name:", nameAndEditorLayout);
    instanceLayout->addRow(interfaceEditor);

    QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
    if (nameCombo && nameCombo->count() > 0) {
        onInstanceSelected(nameCombo->currentText(), roleLabel, interfaceEditor);
    }

    connect(nameCombo, &QComboBox::currentTextChanged, this,
        [=]() {
            QString selectedInstance = nameCombo->currentText();
            onInstanceSelected(selectedInstance, roleLabel, interfaceEditor);
        });

    QPushButton* closeButton = instanceFrame->findChild<QPushButton*>();
    connect(closeButton, &QPushButton::clicked, this,
        [=]() {
            if (roleLabel == "StartingPoint") {
                startingPointsLayout_->removeWidget(instanceFrame);
                addedStartingPoints_.remove(nameCombo->currentText());
            }
            else {
                endpointsLayout_->removeWidget(instanceFrame);
                addedEndpoints_.remove(nameCombo->currentText());
            }
            instanceFrame->deleteLater();
            updateInterconnectModeOptions();
        });

    return instanceFrame;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createNameAndEnumerationEditorLayout()
//-----------------------------------------------------------------------------
QVBoxLayout* InterconnectGeneratorDialog::createNameAndEnumerationEditorLayout(
    const QString& roleLabel, InterfaceEnumEditor* interfaceEditor, QFrame* instanceFrame)
{
    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* nameLayout = new QHBoxLayout();
    InstanceComboBox* nameCombo = new InstanceComboBox();
    QStringList availableInstances;

    updateNameCombo(nameCombo, roleLabel, availableInstances);

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("No Instances Available"),
            tr("All instances have already been added."));
        delete instanceFrame;
        return layout;
    }

    QPushButton* closeButton = new QPushButton(QIcon(":/icons/common/graphics/cross.png"), QString());
    closeButton->setText(roleLabel == "StartingPoint" ? tr("Remove Starting Point") : tr("Remove Endpoint"));

    nameLayout->addWidget(nameCombo);
    nameLayout->addWidget(closeButton);

    connect(nameCombo, &QComboBox::currentTextChanged, this, [=]() {
        QString selectedInstance = nameCombo->currentText();
        onInstanceSelected(selectedInstance, roleLabel, interfaceEditor);
        });

    if (nameCombo->count() > 0) {
        onInstanceSelected(nameCombo->currentText(), roleLabel, interfaceEditor);
    }

    layout->addLayout(nameLayout);

    connect(closeButton, &QPushButton::clicked, this, [=]() {
        QString instanceName = nameCombo->currentText();

        if (roleLabel == "StartingPoint") {
            startingPointsLayout_->removeWidget(instanceFrame);
            addedStartingPoints_.remove(instanceName);
        }
        else {
            endpointsLayout_->removeWidget(instanceFrame);
            addedEndpoints_.remove(instanceName);
        }

        instanceFrame->deleteLater();
        });

    return layout;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::clearInitiatorAndTargetLists()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::clearConnectionsLists()
{
    QLayoutItem* item;

    while ((item = startingPointsLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    while ((item = endpointsLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    addedStartingPoints_.clear();
    addedEndpoints_.clear();

    startingPointsLayout_->update();
    endpointsLayout_->update();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createTopConfigSection()
//-----------------------------------------------------------------------------
QWidget* InterconnectGeneratorDialog::createTopConfigSection() 
{
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
        if (!addedStartingPoints_.isEmpty() || !addedEndpoints_.isEmpty()) {
            int response = QMessageBox::warning(this, tr("Data Loss"),
                tr("All existing initiators and targets will be removed if abstraction definition is changed. Do you want to continue?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (response == QMessageBox::No) {
                ignoreChange_ = true;
                busInterfaceCombo_->setCurrentIndex(previousIndex_);
                return;
            }
            clearConnectionsLists();
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

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createInitiatorsSection()
//-----------------------------------------------------------------------------
QGroupBox* InterconnectGeneratorDialog::createStartingPointsSection()
{
    QPushButton* addInitiatorButton = new QPushButton(QIcon(":/icons/common/graphics/add.png"), tr("Add New Starting Point"));
    connect(addInitiatorButton, &QPushButton::clicked, this, &InterconnectGeneratorDialog::addNewStartingPoint);

    startingPointsLayout_->setAlignment(Qt::AlignTop);
    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(startingPointsLayout_);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* sectionLayout = new QVBoxLayout();
    sectionLayout->addWidget(addInitiatorButton);
    sectionLayout->addWidget(scrollArea);

    QGroupBox* groupBox = new QGroupBox(tr("Connection Starting Points"), this);
    groupBox->setLayout(sectionLayout);
    return groupBox;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createEndpointsSection()
//-----------------------------------------------------------------------------
QGroupBox* InterconnectGeneratorDialog::createEndpointsSection()
{
    QPushButton* addTargetButton = new QPushButton(QIcon(":/icons/common/graphics/add.png"), tr("Add New Endpoint"));
    connect(addTargetButton, &QPushButton::clicked, this, &InterconnectGeneratorDialog::addNewEndpoint);

    endpointsLayout_->setAlignment(Qt::AlignTop);
    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(endpointsLayout_);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* sectionLayout = new QVBoxLayout();
    sectionLayout->addWidget(addTargetButton);
    sectionLayout->addWidget(scrollArea);

    QGroupBox* groupBox = new QGroupBox(tr("Connection Endpoints"), this);
    groupBox->setLayout(sectionLayout);
    return groupBox;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createInterconnectModeSelector()
//-----------------------------------------------------------------------------
QWidget* InterconnectGeneratorDialog::createInterconnectModeSelector() 
{
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

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createButtonBox()
//-----------------------------------------------------------------------------
QDialogButtonBox* InterconnectGeneratorDialog::createButtonBox() 
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(tr("Generate"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InterconnectGeneratorDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InterconnectGeneratorDialog::reject);

    return buttonBox;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::setUpLayout()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::setUpLayout() 
{
    setWindowTitle("Interconnect Component Configuration");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createTopConfigSection());
    mainLayout->addWidget(parameterGroupBox_);

    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    bottomRowLayout->addWidget(createStartingPointsSection());
    bottomRowLayout->addWidget(createEndpointsSection());
    mainLayout->addLayout(bottomRowLayout);

    mainLayout->addWidget(createInterconnectModeSelector());

    mainLayout->addWidget(createButtonBox());

    setLayout(mainLayout);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::accept()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::accept()
{
    ConfigStruct* config = new ConfigStruct;

    if (!vlnvEditor_->isValid()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Select valid VLNV for interconnect component."));
        return;
    }
    if (KactusAPI::getLibrary()->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox::warning(this, tr("Input Error"), tr("Selected VLNV for interconnect component already exists."));
        return;
    }
    if (!bridgeButton_->isEnabled() && !channelButton_->isEnabled()) 
    {
        QMessageBox::warning(this, tr("Input Error"), tr("Can't connect selected interfaces."));
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

    // Try to find possible address width value from UI parameters
    for (const auto& param : *interconnectParams_) {

        if (!param) continue;
        QString name = param->name().toLower();
        if ((name.contains("addr") && name.contains("width")) ||
            name.contains("addrwidth") ||
            name == "aw" || name == "addr_w") {

            config->AddressWidth = param->getValue().toInt();
            break;
        }
    }

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
    config_ = config; //new
    collectInstances();
    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectInstances()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectInstances()
{
    auto instanceBusesLookup = dataModel_->createInstanceBusesLookup();

    collectStartingPoints(instanceBusesLookup);
    collectEndpoints(instanceBusesLookup);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectStartingPoints()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectStartingPoints(
    const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup)
{
    selectedStartingPoints_.clear();
    QList<InitStruct> initiators;

    for (int i = 0; i < startingPointsLayout_->count(); ++i) {
        QLayoutItem* item = startingPointsLayout_->itemAt(i);
        if (QFrame* instanceFrame = qobject_cast<QFrame*>(item->widget())) {
            QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
            InterfaceEnumEditor* interfaceEditor = instanceFrame->findChild<InterfaceEnumEditor*>();
            if (!nameCombo || !interfaceEditor) {
                continue;
            }
            QString instanceName = nameCombo->currentText();
            QStringList selectedInterfaces = interfaceEditor->getSelectedStartingPointInterfaces();

            if (!instanceBusesLookup.contains(instanceName)) {
                continue;
            }
            const auto& busLookup = instanceBusesLookup.value(instanceName);
            QList<QSharedPointer<BusInterface>> matchedInterfaces;

            for (const QString& selectedInterfaceName : selectedInterfaces) {
                if (busLookup.contains(selectedInterfaceName)) {
                    matchedInterfaces.append(busLookup.value(selectedInterfaceName));

                    InitStruct init;

                    init.Index = i;
                    init.Name = instanceName + "_" + selectedInterfaceName;
                    init.DataWidth = 32;

                    // Get possible data width value from UI parameters
                    for (const auto& param : *interconnectParams_) {

                        if (param == nullptr) {
                            continue;
                        }

                        QString name = param->name().toLower();
                        if ((name.contains("data") && name.contains("width")) ||
                            name.contains("datawidth") ||
                            name == "dw" || name == "data_w") {

                            init.DataWidth = param->getValue().toInt();
                            break;
                        }
                    }

                    initiators.append(init);
                }
            }
            // Add to selectedInitiators_ only if there are matched interfaces
            if (!matchedInterfaces.isEmpty()) {
                selectedStartingPoints_.insert(instanceName, matchedInterfaces);
            }
        }
    }
    if (config_) 
    {
        config_->InitList.clear();
        config_->InitList = initiators;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectEndpoints()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectEndpoints(
    const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup)
{
    selectedEndpoints_.clear();
    QList<AddressPair> addrList;
    QList<TargetStruct> targets;

    for (int i = 0; i < endpointsLayout_->count(); ++i) {
        QLayoutItem* item = endpointsLayout_->itemAt(i);
        if (QFrame* instanceFrame = qobject_cast<QFrame*>(item->widget())) {
            QComboBox* nameCombo = instanceFrame->findChild<QComboBox*>();
            InterfaceEnumEditor* interfaceEditor = instanceFrame->findChild<InterfaceEnumEditor*>();
            if (!nameCombo || !interfaceEditor) {
                continue;
            }
            QString instanceName = nameCombo->currentText();
            QList<EndpointInterfaceData> selectedInterfaces = interfaceEditor->getSelectedEndpointInterfaces();

            if (!instanceBusesLookup.contains(instanceName)) {
                continue;
            }
            const auto& busLookup = instanceBusesLookup.value(instanceName);
            QList<QSharedPointer<EndpointData > > matchedInterfaces;

            for (const EndpointInterfaceData& targetInterface : selectedInterfaces) {
                if (busLookup.contains(targetInterface.name)) {
                    auto targetData = QSharedPointer<EndpointData>::create();
                    targetData->endpointBus = busLookup.value(targetInterface.name);
                    targetData->start = targetInterface.startValue;
                    targetData->range = targetInterface.range;

                    matchedInterfaces.append(targetData);

                    addrList.clear();

                    AddressPair addr;

                    addr.Start = targetInterface.startValue.isEmpty() ? "<address>" : targetInterface.startValue;
                    addr.End = targetInterface.range.isEmpty() ? "<address>" : targetInterface.range;

                    addrList.append(addr);
                    TargetStruct target;

                    target.DataWidth = 32;

                    // Get possible data width value from UI parameters
                    for (const auto& param : *interconnectParams_) {

                        if (param == nullptr) {
                            continue;
                        }

                        QString name = param->name().toLower();
                        if ((name.contains("data") && name.contains("width")) ||
                            name.contains("datawidth") ||
                            name == "dw" || name == "data_w") {

                            target.DataWidth = param->getValue().toInt();
                            break;
                        }
                    }

                    target.Index = i;
                    target.Name = instanceName + "_" + targetInterface.name;
                    target.AddressRegions = addrList;

                    targets.append(target);
                }
            }
            if (!matchedInterfaces.isEmpty()) {
                selectedEndpoints_.insert(instanceName, matchedInterfaces);
            }
        }
    }
    if (config_) 
    {
        config_->TargetList.clear();
        config_->TargetList = targets;
    }
}
