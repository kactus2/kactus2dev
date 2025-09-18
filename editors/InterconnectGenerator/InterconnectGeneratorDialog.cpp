#include "InterconnectGeneratorDialog.h"
#include "InterconnectDataModel.h"
#include <KactusAPI/KactusAPI.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include "InstanceInterfacesEditor.h"

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
    instancesLayout_(new QVBoxLayout()),
    endpointsLayout_(new QVBoxLayout()),
    vlnvEditor_(new VLNVEditor(VLNV::BUSDEFINITION, library, this, this))
{
    setMinimumWidth(900);
    setMinimumHeight(900);

    designVLNV_ = designWidget->getEditedComponent()->getVlnv().toString(":");

    //QWidget* hiddenWidget = new QWidget(this);
    //hiddenWidget->hide();
    //componentInstances_ = new InstanceComboBox(hiddenWidget);

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
    busLookUp_ = dataModel_->createInstanceBusesLookup();

    setUpLayout();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::~InterconnectGeneratorDialog()
//-----------------------------------------------------------------------------
InterconnectGeneratorDialog::~InterconnectGeneratorDialog()
{
    if (config_)
        delete config_;
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
        if (ref->getName() == absDefCombo_->currentText()) {
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
// Function: InterconnectGeneratorDialog::addNewInstance()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::addNewInstance()
{
    // create instance editor
    auto newInterfacesEditor = new InstanceInterfacesEditor(designWidget_->getEditedComponent()->getRevision());
    connect(newInterfacesEditor, SIGNAL(instanceRemoved()), this, SLOT(onInstanceRemoved()), Qt::UniqueConnection);

    instancesLayout_->addWidget(newInterfacesEditor);
    instanceEditors_.insert(newInterfacesEditor);
    
    // Update available instances of new editor
    newInterfacesEditor->updateAvailableInstances(availableInstances_);
    
    // Remove item currently selected in new editor from available instances, then update the rest of the editors,
    // ignoring the new editor
    availableInstances_.remove(newInterfacesEditor->getSelectedInstance());
    updateAvailableInstances();

    connect(newInterfacesEditor, SIGNAL(instanceSelected(QString const&, QString const&)), this, SLOT(onInstanceSelected(QString const&, QString const&)), Qt::UniqueConnection);
    updateAvailableInstanceInterfaces(newInterfacesEditor, newInterfacesEditor->getSelectedInstance());

    updateAddInstanceButtonStatus();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::onInterconnectModeChanged()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::onInterconnectSelectorToggled(bool checked)
{
    if (!checked)
    {
        return;
    }

    // Clear already selected instances and interfaces, if permission is given. Otherwise set selection back to previous interface mode.
    if (hasSelectedInstances())
    {
        auto response = QMessageBox::warning(this, tr("Change interconnect mode"), 
            tr("All existing interface selections will be removed if interface mode is changed. Continue?"), 
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (response == QMessageBox::No)
        {
            if (static_cast<QRadioButton*>(sender()) == bridgeButton_)
            {
                disconnect(channelButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)));
                channelButton_->setChecked(true);
                connect(channelButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)), Qt::UniqueConnection);
            }
            else
            {
                disconnect(bridgeButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)));
                bridgeButton_->setChecked(true);
                connect(bridgeButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)), Qt::UniqueConnection);
            }

            return;
        }

        clearInstanceSelections();
    }

    if (sender() == bridgeButton_)
    {
        isChannel_ = false;
    }
    else if (sender() == channelButton_)
    {
        isChannel_ = true;
    }

    filterAvailableInstancesByAbsDef();
    updateAddInstanceButtonStatus();
}

InterconnectDataModel::InterconnectType InterconnectGeneratorDialog::getSelectedInterconnectType() const
{
    return isChannel_ ? InterconnectDataModel::InterconnectType::Channel
        : InterconnectDataModel::InterconnectType::Bridge;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::updateNameCombo()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::updateNameCombo(QComboBox* nameCombo, QStringList& availableInstances)
{
    availableInstances.clear();

    QString selectedAbsDef = absDefCombo_->currentText();

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

            General::InterfaceMode mode = InterconnectDataModel::normalizeTo2022(bus->getInterfaceMode());

            if (dataModel_->hasAnyValidConnection(entityType, mode)) {
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
    QString selectedAbsDef = absDefCombo_->currentText();
    QStringList validInterfaceNames;

    if (!instanceBusesHash_.contains(instanceName)) {
        interfaceEditor->clearAll();
        return;
    }

    InterconnectDataModel::EntityType entityType = (instanceName == designWidget_->getEditedComponent()->getVlnv().getName())
        ? InterconnectDataModel::EntityType::TopComponent
        : InterconnectDataModel::EntityType::Instance;

    //InterconnectDataModel::InterconnectType icType = getSelectedInterconnectType();

    const QSet<QSharedPointer<BusInterface>>& busSet = instanceBusesHash_.value(instanceName);
    for (const QSharedPointer<BusInterface>& bus : busSet)
    {
        // Only consider interfaces that use the currently selected abstraction definition
        if (!interfaceAbsDefsHash_.contains(bus) ||
            !interfaceAbsDefsHash_.value(bus).contains(selectedAbsDef))
        {
            continue;
        }

        General::InterfaceMode mode = InterconnectDataModel::normalizeTo2022(bus->getInterfaceMode());

        if (dataModel_->hasAnyValidConnection(entityType, mode)) {
            validInterfaceNames.append(bus->name());
        }
    }

    interfaceEditor->clearAll();
    bool isTarget = (roleLabel == "Endpoint");
    interfaceEditor->addItems(validInterfaceNames, isTarget, instanceName);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::onInstanceSelected()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::onInstanceSelected(QString const& newInstanceName, QString const& oldInstanceName)
{
    if (auto interfaceEditor = dynamic_cast<InstanceInterfacesEditor*>(sender()))
    {
        // Clear old interface selections and add new interface options

        interfaceEditor->clearInterfaceItems();

        // Figure out which interfaces are valid options to choose from the selected instance based on selected 
        // interconnect type (bridge or channel) and type of instance (design instance or top component)
        updateAvailableInstanceInterfaces(interfaceEditor, newInstanceName);

        if (newInstanceName.compare(oldInstanceName) != 0)
        {
            // update available instances if new selection is different from old
            availableInstances_.remove(newInstanceName);

            if (!oldInstanceName.isEmpty())
            {
                availableInstances_.insert(oldInstanceName);
            }
        }
    }

    updateAvailableInstances();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::onInstanceRemoved()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::onInstanceRemoved()
{
    auto interfacesEditor = dynamic_cast<InstanceInterfacesEditor*>(sender());
    Q_ASSERT_X(interfacesEditor, "InterconnectGeneratorDialog::onInstanceRemoved", "Invalid sender connected to slot");

    if (!interfacesEditor)
    {
        return;
    }
    
    availableInstances_.insert(interfacesEditor->getSelectedInstance());

    instancesLayout_->removeWidget(interfacesEditor);
    instanceEditors_.remove(interfacesEditor);
    interfacesEditor->deleteLater();

    // Need to update available instances of added combo boxes
    updateAvailableInstances();
    updateAddInstanceButtonStatus();
}

void InterconnectGeneratorDialog::updateInterconnectModeOptions()
{
    collectStartingPoints(busLookUp_);
    collectEndpoints(busLookUp_);

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
// Function: InterconnectGeneratorDialog::filterAvailableInstancesByAbsDef()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::filterAvailableInstancesByAbsDef()
{
    auto currentAbsDef = absDefCombo_->currentText();
    QSet<QString> newAvailableInstances;

    for (const QString& instanceName : instanceBusesHash_.keys())
    {
        // Take into account that interconnect type limits possible instances
        // AKA for bridge interconnect only instances with interfaces that can be connected to bridge should be listed
        auto allowedInterfaceTypes = getAllowedInterfaceTypes(instanceName);

        for (auto const& bus : instanceBusesHash_.value(instanceName))
        {
            if (interfaceAbsDefsHash_.contains(bus) && interfaceAbsDefsHash_.value(bus).contains(currentAbsDef)
                && allowedInterfaceTypes.contains(bus->getInterfaceMode()))
            {
                newAvailableInstances.insert(instanceName);
                break;
            }
        }
    }

    availableInstances_ = newAvailableInstances;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::updateAvailableInstances()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::updateAvailableInstances()
{
    // Update available instances of instance interface editor combo boxes
    for (auto const& singleEditor : instanceEditors_)
    {
        disconnect(singleEditor, SIGNAL(instanceSelected(QString const&, QString const&)), this, SLOT(onInstanceSelected(QString const&, QString const&)));
        singleEditor->updateAvailableInstances(availableInstances_);
        connect(singleEditor, SIGNAL(instanceSelected(QString const&, QString const&)), this, SLOT(onInstanceSelected(QString const&, QString const&)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::updateAllowedInstanceInterfaces()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::updateAvailableInstanceInterfaces(InstanceInterfacesEditor* interfacesEditor, QString const& selectedInstance)
{
    // Get component interfaces
    auto const& instanceInterfaces = instanceBusesHash_.value(selectedInstance);

    if (instanceInterfaces.isEmpty())
    {
        return;
    }

    QList<QPair<QString, bool> > interfacesToAdd;

    auto allowedInterfaceTypes = getAllowedInterfaceTypes(selectedInstance);
    auto currentAbsDef = absDefCombo_->currentText();

    bool isTopComponent = designWidget_->getEditedComponent()->getVlnv().getName() == selectedInstance;

    QList<InterfaceEnumEditor::InterfaceInput> targetsToAdd;
    QList<InterfaceEnumEditor::InterfaceInput> initiatorsToAdd;
    QList<InterfaceEnumEditor::InterfaceInput> mirroredTargetsToAdd;
    QList<InterfaceEnumEditor::InterfaceInput> mirroredInitiatorsToAdd;

    for (auto const& busInterface : instanceInterfaces)
    {
        auto busAbsTypes = busInterface->getAbstractionTypes();
        auto interfaceMode = busInterface->getInterfaceMode();
        bool interfaceModeAllowed = allowedInterfaceTypes.contains(interfaceMode);
        bool busHasCompatibleAbsType = interfaceAbsDefsHash_.value(busInterface).contains(currentAbsDef);

        if (interfaceModeAllowed && busHasCompatibleAbsType)
        {
            // Divide interfaces in groups based on 1) type and 2) if they are target- or initiator adjacent
            // (target adjacent = start and range can be set)
            if (isChannel_)
            {
                // E.g. Target is always target-adjacent when used with channel
                // (because function getAllowedInterfaceTypes() filters top component targets)
                if (interfaceMode == General::InterfaceMode::TARGET || interfaceMode == General::InterfaceMode::SLAVE)
                {
                    targetsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), true });
                }
                else if (interfaceMode == General::InterfaceMode::INITIATOR || interfaceMode == General::InterfaceMode::MASTER)
                {
                    initiatorsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), false });
                }
                else if (interfaceMode == General::InterfaceMode::MIRRORED_TARGET || interfaceMode == General::InterfaceMode::MIRRORED_SLAVE)
                {
                    mirroredTargetsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), true });
                }
                else if (interfaceMode == General::InterfaceMode::MIRRORED_INITIATOR || interfaceMode == General::InterfaceMode::MIRRORED_MASTER)
                {
                    mirroredInitiatorsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), false });
                }
            }
            else
            {
                if (interfaceMode == General::InterfaceMode::TARGET || interfaceMode == General::InterfaceMode::SLAVE)
                {
                    // Both top component and instance targets can be connected when using bridge, target is target-adjacent
                    // if it is an instance interface.
                    targetsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), isTopComponent == false });
                }
                else if (interfaceMode == General::InterfaceMode::INITIATOR || interfaceMode == General::InterfaceMode::MASTER)
                {
                    // Both top component and instance initiators can be connected when using bridge. Initiator is target-adjacent,
                    // if it is a top component interface.
                    initiatorsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), isTopComponent == true });
                }
                else if (interfaceMode == General::InterfaceMode::MIRRORED_TARGET || interfaceMode == General::InterfaceMode::MIRRORED_SLAVE)
                {
                    mirroredTargetsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), false });
                }
                else if (interfaceMode == General::InterfaceMode::MIRRORED_INITIATOR || interfaceMode == General::InterfaceMode::MIRRORED_MASTER)
                {
                    mirroredInitiatorsToAdd.append(InterfaceEnumEditor::InterfaceInput{ busInterface->name(), true });
                }
            }
        }
    }

    interfacesEditor->addInterfaceItems(initiatorsToAdd, General::InterfaceMode::INITIATOR);
    interfacesEditor->addInterfaceItems(mirroredInitiatorsToAdd, General::InterfaceMode::MIRRORED_INITIATOR);
    interfacesEditor->addInterfaceItems(targetsToAdd, General::InterfaceMode::TARGET);
    interfacesEditor->addInterfaceItems(mirroredTargetsToAdd, General::InterfaceMode::MIRRORED_TARGET);

    interfacesEditor->createInterfaceItems(isChannel_);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::updateAddInstanceButtonStatus()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::updateAddInstanceButtonStatus()
{
    addInstanceButton_->setEnabled(availableInstances_.isEmpty() == false);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::hasSelectedInstances()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::hasSelectedInstances()
{
    return instancesLayout_->isEmpty() == false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::getAllowedInterfaceTypes()
//-----------------------------------------------------------------------------
QSet<General::InterfaceMode> InterconnectGeneratorDialog::getAllowedInterfaceTypes(QString const& selectedInstance)
{
    // Need to figure out if selected instance is top component or regular instance
    auto isTopComponent = selectedInstance == designWidget_->getEditedComponent()->getVlnv().getName();

    InterconnectDataModel::EntityType instanceType = isTopComponent
        ? InterconnectDataModel::EntityType::TopComponent
        : InterconnectDataModel::EntityType::Instance;

    InterconnectDataModel::ConnectionKeyNew allowedInterfacesKey;

    // Allowed interfaces when channel is selected:
    // Top component: mirrored target/slave & mirrored initiator/master
    // Instance: initiator/master, target/slave, mirrored initiator/master, mirrored target/slave
    if (isChannel_)
    {
        allowedInterfacesKey = {
            InterconnectDataModel::InterconnectType::Channel,
            instanceType
        };
    }
    else
    {
        allowedInterfacesKey = {
            InterconnectDataModel::InterconnectType::Bridge,
            instanceType
        };
    }

    return dataModel_->getConnectableInterfaceTypes(allowedInterfacesKey);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::interfaceSelectionsAreLegal()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::interfaceSelectionsAreLegal(QString& errorMsg)
{
    auto isStd22 = designWidget_->getEditedComponent()->getRevision() == Document::Revision::Std22;

    QString initStr = isStd22 ? QStringLiteral("initiator") : QStringLiteral("master");
    QString targetStr = isStd22 ? QStringLiteral("target") : QStringLiteral("slave");

    if (selectedEndpoints_.isEmpty() || selectedStartingPoints_.isEmpty())
    {
        if (isChannel_)
        {
            errorMsg = tr("Selections must contain the following:\n"
                          "1. At least one instance %1 or design component mirrored %1\n"
                          "2. At least one instance %2 or design component mirrored %2")
                .arg(initStr).arg(targetStr);
        }
        else
        {
            errorMsg = tr("Selections must contain the following:\n"
                          "1. At least one instance %1 or design component %2\n"
                          "2. At least one instance %2 or mirrored %1 or design component %1")
                .arg(initStr).arg(targetStr);
        }

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createInstanceEditorFrame()
//-----------------------------------------------------------------------------
QFrame* InterconnectGeneratorDialog::createInstanceEditorFrame(const QString& roleLabel)
{
    // TODO remove unused

    QStringList availableInstances;
    updateNameCombo(nullptr, availableInstances);

    if (availableInstances.isEmpty()) {
        QMessageBox::information(this, tr("No Instances Available"), tr("All instances have already been added."));
        return nullptr;
    }

    QString instanceName = availableInstances.first();
    QFrame* instanceFrame = new QFrame();
    instanceFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    InterfaceEnumEditor* interfaceEditor = new InterfaceEnumEditor(designWidget_->getEditedComponent()->getRevision(), this);

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
                instancesLayout_->removeWidget(instanceFrame);
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

    updateNameCombo(nameCombo, availableInstances);

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
            instancesLayout_->removeWidget(instanceFrame);
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
void InterconnectGeneratorDialog::clearInstanceSelections()
{
    QLayoutItem* item;

    while ((item = instancesLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    instanceEditors_.clear();
    instancesLayout_->update();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::createTopConfigSection()
//-----------------------------------------------------------------------------
QWidget* InterconnectGeneratorDialog::createTopConfigSection() 
{
    QGroupBox* configGroup = new QGroupBox(tr("Interconnect Component Configuration"), this);
    QFormLayout* formLayout = new QFormLayout();

    absDefCombo_ = new InstanceComboBox(this);
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
        if (absDefCombo_->findText(name) != -1) continue;
        absDefCombo_->addItem(name);
    }

    for (QSharedPointer<ConfigurableVLNVReference> ref : allAbsRefs_) {
        QString name = ref->getName();
        if (clockCombo_->findText(name) != -1) continue;

        clockCombo_->addItem(name);
        resetCombo_->addItem(name);
    }

    previousAbsDefIndex_ = absDefCombo_->currentIndex();
    populateParameters();
    filterAvailableInstancesByAbsDef();

    connect(absDefCombo_, SIGNAL(currentIndexChanged(int)), SLOT(onAbsDefChanged(int)), Qt::UniqueConnection);

    connect(absDefCombo_, &QComboBox::activated, this, [this]() {
        previousAbsDefIndex_ = absDefCombo_->currentIndex();
        });

    connect(clockCheckBox_, &QCheckBox::stateChanged, this, [this](int state) {
        clockCombo_->setEnabled(state == Qt::Checked);
        });
    connect(resetCheckBox_, &QCheckBox::stateChanged, this, [this](int state) {
        resetCombo_->setEnabled(state == Qt::Checked);
        });

    formLayout->addRow(tr("Abstraction Definition:"), absDefCombo_);
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
// Function: InterconnectGeneratorDialog::createInstancesSection()
//-----------------------------------------------------------------------------
QGroupBox* InterconnectGeneratorDialog::createInstancesSection()
{
    connect(addInstanceButton_, SIGNAL(clicked()), this, SLOT(addNewInstance()));

    instancesLayout_->setAlignment(Qt::AlignTop);
    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(instancesLayout_);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* sectionLayout = new QVBoxLayout();
    sectionLayout->addWidget(addInstanceButton_);
    sectionLayout->addWidget(scrollArea);

    QGroupBox* groupBox = new QGroupBox(tr("Instances to connect"), this);
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

    connect(bridgeButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)), Qt::UniqueConnection);
    connect(channelButton_, SIGNAL(toggled(bool)), this, SLOT(onInterconnectSelectorToggled(bool)), Qt::UniqueConnection);

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
    bottomRowLayout->addWidget(createInstancesSection());
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
        QString message = dataModel_->getLastInvalidConnectionMessage();
        QMessageBox::warning(this, tr("Input Error"), message);
        return;
    }

    // TODO Collect selected interfaces into selectedStartingPoints_ and selectedEndpoints_
    // Interfaces shall be mapped to either bucket based on which interconnect mode is selected.


    QString selectionErrorMsg;
    if (interfaceSelectionsAreLegal(selectionErrorMsg) == false)
    {
        QMessageBox::warning(this, tr("Input Error"), selectionErrorMsg);
        return;
    }

    QSharedPointer<ConfigurableVLNVReference> absRef;
    QSharedPointer<ConfigurableVLNVReference> clkRef;
    QSharedPointer<ConfigurableVLNVReference> rstRef;
    for (auto& ref : allAbsRefs_) {
        if (ref->getName() == absDefCombo_->currentText()) {
            absRef = ref;
        }
        if (ref->getName() == clockCombo_->currentText()) {
            clkRef = ref;
        }
        if (ref->getName() == resetCombo_->currentText()) {
            rstRef = ref;
        }
    }

    auto interconVLNV = vlnvEditor_->getVLNV().toString();
    auto busVLNV = absRef->toString();
    auto busType = absRef->getName();
    auto clkVLNV = clockCheckBox_->isChecked() ? clkRef->toString() : QString();
    auto rstVLNV = resetCheckBox_->isChecked() ? rstRef->toString() : QString();
    
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

    config_ = config; //new
    collectInstances();
    if (selectedStartingPoints_.isEmpty() || selectedEndpoints_.isEmpty()) 
    {
        QMessageBox::warning(this, tr("Input Error"), tr("Can't connect selected interfaces."));
        return;
    }
    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::onAbsDefChanged()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::onAbsDefChanged(int currentIndex)
{
    if (ignoreChange_)
    {
        ignoreChange_ = false;
        return;
    }

    populateParameters();
    filterAvailableInstancesByAbsDef();

    if (hasSelectedInstances())
    {
        int response = QMessageBox::warning(this, tr("Data Loss"),
            tr("All existing interface selections will be removed if abstraction definition is changed. Do you want to continue?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (response == QMessageBox::No)
        {
            ignoreChange_ = true;
            absDefCombo_->setCurrentIndex(previousAbsDefIndex_);
            return;
        }

        clearInstanceSelections();
        filterAvailableInstancesByAbsDef();
        updateAddInstanceButtonStatus();
    }
    previousAbsDefIndex_ = currentIndex;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectInstances()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectInstances()
{

    collectStartingPoints(busLookUp_);
    collectEndpoints(busLookUp_);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectStartingPoints()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectStartingPoints(
    const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup)
{
    selectedStartingPoints_.clear();
    QList<InitStruct> initiators;
    int intfIndex = 0;

    for (int i = 0; i < instancesLayout_->count(); ++i)
    {
        auto item = instancesLayout_->itemAt(i);

        if (auto instanceFrame = qobject_cast<QFrame*>(item->widget()))
        {
            auto nameCombo = instanceFrame->findChild<QComboBox*>();
            auto interfaceEditor = instanceFrame->findChild<InterfaceEnumEditor*>();

            if (!nameCombo || !interfaceEditor)
            {
                continue;
            }

            QString instanceName = nameCombo->currentText();
            QStringList selectedInterfaces = interfaceEditor->getSelectedStartingPointInterfaces();

            if (!instanceBusesLookup.contains(instanceName))
            {
                continue;
            }

            const auto& busLookup = instanceBusesLookup.value(instanceName);
            QList<QSharedPointer<BusInterface>> matchedInterfaces;

            for (const QString& selectedInterfaceName : selectedInterfaces)
            {
                if (busLookup.contains(selectedInterfaceName))
                {
                    matchedInterfaces.append(busLookup.value(selectedInterfaceName));

                    InitStruct init;

                    init.Index = intfIndex++;
                    init.Name = instanceName + "_" + selectedInterfaceName;
                    init.DataWidth = 32;

                    // Get possible data width value from UI parameters
                    for (const auto& param : *interconnectParams_)
                    {
                        if (param == nullptr)
                        {
                            continue;
                        }

                        QString name = param->name().toLower();

                        if ((name.contains("data") && name.contains("width")) ||
                            name.contains("datawidth") ||
                            name == "dw" || name == "data_w")
                        {
                            init.DataWidth = param->getValue().toInt();
                            break;
                        }
                    }

                    initiators.append(init);
                }
            }
            // Add to selectedInitiators_ only if there are matched interfaces
            if (!matchedInterfaces.isEmpty())
            {
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
    int intfIndex = 0;

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

                    target.Index = intfIndex++;
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
