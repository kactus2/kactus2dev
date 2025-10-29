#include "InterconnectGeneratorDialog.h"
#include "InterconnectDataModel.h"
#include "InstanceInterfacesEditor.h"

#include <KactusAPI/KactusAPI.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <editors/common/ExpressionSet.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::InterconnectGeneratorDialog()
//-----------------------------------------------------------------------------
InterconnectGeneratorDialog::InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
    MessageMediator* messager, QWidget* parent)
    : QDialog(parent),
    designWidget_(designWidget),
    library_(library),
    messager_(messager),
    vlnvEditor_(new VLNVEditor(VLNV::BUSDEFINITION, library, this, this)),
    instancesLayout_(new QVBoxLayout())
{
    setMinimumWidth(1300);
    setMinimumHeight(900);

    designVLNV_ = designWidget->getEditedComponent()->getVlnv().toString(":");

    auto parameters = QSharedPointer<QList<QSharedPointer<Parameter>>>::create();
    auto choices = QSharedPointer<QList<QSharedPointer<Choice>>>::create();

    QSharedPointer<ListParameterFinder> listFinder = QSharedPointer<ListParameterFinder>::create();
    parameterFinder_ = listFinder;
    expressionFormatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(listFinder));

    parameterCompleterModel_ = new ComponentParameterModel(parameterFinder_, this);
    parameterCompleterModel_->setExpressionParser(expressionParser_);

    parameterGroupBox_ = new ParameterGroupBox(parameters, choices, listFinder, expressionFormatter_,
        designWidget_->getEditedComponent()->getRevision(), this);
    parameterGroupBox_->setTitle("Interconnect Component Parameters");

    dataModel_ = QSharedPointer<InterconnectDataModel>::create(designWidget_, library_, messager_);
    dataModel_->gatherBusAndAbstractionData();

    //addressHelper_ = QSharedPointer<InterconnectAddressHelper>::create(
    //    designWidget->getEditedComponent()->getVlnv(), 
    //    library_, messager_);

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
// Function: InterconnectGeneratorDialog::rtlGenerationSelected()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::rtlGenerationSelected()
{
    return generateRTLCheckBox_->isChecked();
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
    auto newInterfacesEditor = new InstanceInterfacesEditor(ExpressionSet{parameterFinder_, expressionParser_, expressionFormatter_}, 
        parameterCompleterModel_, designWidget_->getEditedComponent()->getRevision());

    connect(newInterfacesEditor, SIGNAL(instanceRemoved()), this, SLOT(onInstanceRemoved()), Qt::UniqueConnection);

    // Add new instance to the front
    instancesLayout_->insertWidget(0, newInterfacesEditor);
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
        auto allowedInterfaceTypes = getAllowedInterfaceModes(instanceName);

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

    auto allowedInterfaceTypes = getAllowedInterfaceModes(selectedInstance);
    auto currentAbsDef = absDefCombo_->currentText();

    bool isTopComponent = instanceIsTopComponent(selectedInstance);

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
    return instanceEditors_.size() > 0;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::getAllowedInterfaceModes()
//-----------------------------------------------------------------------------
QSet<General::InterfaceMode> InterconnectGeneratorDialog::getAllowedInterfaceModes(QString const& selectedInstance)
{
    // Need to figure out if selected instance is top component or regular instance
    auto isTopComponent = instanceIsTopComponent(selectedInstance);

    InterconnectDataModel::EntityType instanceType = isTopComponent
        ? InterconnectDataModel::EntityType::TopComponent
        : InterconnectDataModel::EntityType::Instance;

    InterconnectDataModel::AllowedInterfaceModesKey allowedInterfacesKey;

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
// Function: InterconnectGeneratorDialog::interfaceSelectionsAreValid()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::interfaceSelectionsAreValid(QString& errorMsg)
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
// Function: InterconnectGeneratorDialog::instanceIsTopComponent()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::instanceIsTopComponent(QString const& instanceName)
{
    return designWidget_->getEditedComponent()->getVlnv().getName().compare(instanceName) == 0;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::clearInitiatorAndTargetLists()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::clearInstanceSelections()
{
    QLayoutItem* item;

    // Temporarily remove condenser item
    auto condenserItem = instancesLayout_->takeAt(instancesLayout_->count() - 1);

    while ((item = instancesLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    instanceEditors_.clear();

    // Add condenser item and stretch back
    instancesLayout_->addItem(condenserItem);
    instancesLayout_->addStretch(1);

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

    // Condenser item and stretch to fill space at bottom of instances list
    QWidget* layoutCondenser = new QWidget();
    layoutCondenser->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    instancesLayout_->addWidget(layoutCondenser);
    instancesLayout_->addStretch(1);

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
// Function: InterconnectGeneratorDialog::createGenerateRtlCheckbox()
//-----------------------------------------------------------------------------
QWidget* InterconnectGeneratorDialog::createGenerateRtlCheckbox()
{
    QGroupBox* rtlGenerationGroup = new QGroupBox(tr("Generate RTL"), this);
    auto layout = new QVBoxLayout();
    
    generateRTLCheckBox_ = new QCheckBox("Generate RTL code (for compatible abstraction definitions)");
    generateRTLCheckBox_->setToolTip("RTL generation support is currently only provided for TUNI-authored abstractions for OBI and AXI4/AXI4LITE.");
    layout->addWidget(generateRTLCheckBox_);
    rtlGenerationGroup->setLayout(layout);

    generateRTLCheckBox_->setEnabled(rtlCanBeGenerated());

    return rtlGenerationGroup;
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::rtlCanBeGenerated()
//-----------------------------------------------------------------------------
bool InterconnectGeneratorDialog::rtlCanBeGenerated()
{
    // RTL can only be generated for compatible abs defs (TUNI versions of AXI4, AXI4LITE, OBI for now)
    // Check if selected abs def is compatible.

    auto selectedAbsDef = absDefCombo_->currentText();

    // Find selected abs def vlnv
    auto foundIt = std::find_if(allAbsRefs_.cbegin(), allAbsRefs_.cend(),
        [&selectedAbsDef](QSharedPointer<ConfigurableVLNVReference> vlnv)
        {
            return vlnv->getName().compare(selectedAbsDef) == 0;
        });

    if (foundIt == allAbsRefs_.cend())
    {
        return false;
    }

    if (auto foundVlnv = foundIt->get())
    {
        for (auto const& validVlnvStr : rtlCompatibleAbsDefs_)
        {
            auto validAbsDefVlnv = VLNV(VLNV::ABSTRACTIONDEFINITION, validVlnvStr);

            if (validAbsDefVlnv.getVendor().compare(foundVlnv->getVendor()) == 0
                && validAbsDefVlnv.getLibrary().compare(foundVlnv->getLibrary()) == 0
                && validAbsDefVlnv.getName().compare(foundVlnv->getName()) == 0)
            {
                return true;
            }
        }
    }
    return false;
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

    auto mainLayout = new QVBoxLayout(this);
    auto leftRightLayout = new QHBoxLayout();

    auto leftSideLayout = new QVBoxLayout();
    leftSideLayout->addWidget(createTopConfigSection());
    leftSideLayout->addWidget(parameterGroupBox_);

    auto rightSideLayout = new QVBoxLayout();
    rightSideLayout->addWidget(createInterconnectModeSelector());
    rightSideLayout->addWidget(createGenerateRtlCheckbox());
    rightSideLayout->addWidget(createInstancesSection());

    leftRightLayout->addLayout(leftSideLayout);
    leftRightLayout->addLayout(rightSideLayout);

    mainLayout->addLayout(leftRightLayout);
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

    config_ = config;
    
    // Collect selected interfaces into selectedStartingPoints_ and selectedEndpoints_
    // Interfaces shall be mapped to either bucket based on which interconnect mode is selected.

    // 1. get all selected interfaces
    // 2. divide into startpoints and endpoints

    collectSelectedInterfaces();

    QString selectionErrorMsg;
    if (interfaceSelectionsAreValid(selectionErrorMsg) == false)
    {
        QMessageBox::warning(this, tr("Input Error"), selectionErrorMsg);
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

    // Check if new abs def selection allows for RTL generation
    generateRTLCheckBox_->setEnabled(rtlCanBeGenerated());
    generateRTLCheckBox_->setChecked(false);
}

//-----------------------------------------------------------------------------
// Function: InterconnectGeneratorDialog::collectSelectedInterfaces()
//-----------------------------------------------------------------------------
void InterconnectGeneratorDialog::collectSelectedInterfaces()
{
    // Get all selected interfaces, divide them into starting points and end points (= initiator adjacent 
    // and target adjacent interfaces)
    // Starting points (bridge): instance initiator or mirrored target, top component target
    // Starting points (channel): Instance initiator or top component mirrored initiator
    // Endpoints (bridge): Instance target or mirrored initiator, top component initiator
    // Endpoints (channel): Instance target or top component mirrored target

    selectedStartingPoints_.clear();
    selectedEndpoints_.clear();

    InterconnectDataModel::InterconnectType interconnectType = isChannel_
        ? InterconnectDataModel::InterconnectType::Channel
        : InterconnectDataModel::InterconnectType::Bridge;

    // Get data width to use
    int dataWidth = 32;
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
            dataWidth = param->getValue().toInt();
            break;
        }
    }

    int targetIndex = 0;
    int initIndex = 0;

    for (auto const& editor : instanceEditors_)
    {
        auto interfaceData = editor->getSelectedInterfaces();
        auto currentInstance = editor->getSelectedInstance();
        auto instanceBuses = busLookUp_.value(currentInstance);
        bool isTopComponent = instanceIsTopComponent(currentInstance);

        InterconnectDataModel::EntityType currentInstanceType = isTopComponent
            ? InterconnectDataModel::EntityType::TopComponent
            : InterconnectDataModel::EntityType::Instance;

        for (auto const& singleInterface : interfaceData)
        {
            // Get interface mode
            auto interfaceModel = instanceBuses.value(singleInterface.name);

            auto interfaceMode = interfaceModel->getInterfaceMode();

            if (dataModel_->interfaceIsTargetAdjacent(interfaceMode, interconnectType, currentInstanceType))
            {
                if (selectedEndpoints_.contains(currentInstance) == false)
                {
                    selectedEndpoints_.insert(currentInstance, QList<QSharedPointer<EndpointData> >());
                }
                
                // Data for IP-XACT model of interconnect
                QSharedPointer<EndpointData> newEndpoint(new EndpointData);
                newEndpoint->endpointBus = interfaceModel;
                newEndpoint->start = singleInterface.startValue;
                newEndpoint->range = singleInterface.range;

                selectedEndpoints_[currentInstance].append(newEndpoint);

                // Below is data for RTL writer
                AddressPair addr;

                // Format start and range values to contain parameter names instead of UUIDs
                auto formattedStartValue = expressionFormatter_->formatReferringExpression(singleInterface.startValue);
                auto formattedRangeValue = expressionFormatter_->formatReferringExpression(singleInterface.range);

                addr.Start = singleInterface.startValue.isEmpty() ? "<address>" : formattedStartValue;
                addr.End = singleInterface.range.isEmpty() ? "<address>" : formattedRangeValue;

                TargetStruct target;
                target.Name = currentInstance + "_" + singleInterface.name;
                target.DataWidth = dataWidth;
                target.Index = targetIndex++;
                target.AddressRegions.append(addr);
                config_->TargetList.append(target);
            }
            else if (dataModel_->interfaceIsInitiatorAdjacent(interfaceMode, interconnectType, currentInstanceType))
            {
                if (selectedStartingPoints_.contains(currentInstance) == false)
                {
                    selectedStartingPoints_.insert(currentInstance, QList<QSharedPointer<BusInterface> >());
                }
                
                selectedStartingPoints_[currentInstance].append(interfaceModel);

                // Data for RTL writer
                InitStruct init;
                init.Index = initIndex++;
                init.Name = currentInstance + "_" + singleInterface.name;
                init.DataWidth = dataWidth;

                config_->InitList.append(init);
            }
        }
    }
}
