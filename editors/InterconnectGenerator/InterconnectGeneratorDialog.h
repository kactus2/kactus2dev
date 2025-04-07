#ifndef INTERCONNECTGENERATORDIALOG_H
#define INTERCONNECTGENERATORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QString>
#include <QScrollArea>
#include <QFrame>
#include <QSet>
#include <QHash>
#include <QList>

#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/EnumCollectionEditor/InterfaceEnumEditor.h>
#include <common/widgets/instanceWidgets/InstanceWidgets.h>
#include <editors/common/DesignWidget.h>
#include <editors/InterconnectGenerator/InterconnectAddressHelper.h>
#include <common/graphicsItems/ComponentItem.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <KactusAPI/include/ConfigJsonParser.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/InterconnectGenerator.h>
#include <KactusAPI/include/MessageMediator.h>

class InterconnectDataModel;

class InterconnectGeneratorDialog : public QDialog
{
    Q_OBJECT
public:
    InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
        MessageMediator* messager, QWidget* parent = nullptr);
    ~InterconnectGeneratorDialog() = default;

    ConfigStruct* getConfig();
    QHash<QString, QList<QSharedPointer<BusInterface>>> getSelectedInitiators();
    QHash<QString, QList<QSharedPointer<TargetData>>> getSelectedTargets();

protected:
    void accept() override;

private:
    // UI construction helpers
    QWidget* createTopConfigSection();
    QGroupBox* createInitiatorsSection();
    QGroupBox* createTargetsSection();
    QDialogButtonBox* createButtonBox();
    QVBoxLayout* createNameAndEnumerationEditorLayout(const QString& type, InterfaceEnumEditor* interfaceEditor, QFrame* instanceFrame);
    QFrame* createInstanceEditorFrame(const QString& type);
    void setUpLayout();

    // Parameter and selection logic
    void populateParameters();
    void updateNameCombos();
    void updateNameCombo(QComboBox* nameCombo, const QString& instanceType, QStringList& availableInstances);
    void onInstanceSelected(const QString& instanceName, const QString& type, InterfaceEnumEditor* interfaceEditor);
    void clearInitiatorAndTargetLists();

    // Instance collection for config output
    bool collectInstances(ConfigStruct* config);
    void collectInitiators(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);
    void collectTargets(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);

    void addNewInstance(const QString& type);
    void addNewInitiator();
    void addNewTarget();

    // Dependencies
    DesignWidget* designWidget_;
    LibraryHandler* library_;
    MessageMediator* messager_;

    // Shared logic class (NEW)
    QSharedPointer<InterconnectDataModel> dataModel_;
    QSharedPointer<InterconnectAddressHelper> addressHelper_;

    // UI and state
    ConfigStruct* config_ = nullptr;
    QString designVLNV_;
    VLNVEditor* vlnvEditor_;
    ParameterGroupBox* parameterGroupBox_;
    InstanceComboBox* componentInstances_;
    InstanceComboBox* busInterfaceCombo_;
    InstanceComboBox* clockCombo_;
    InstanceComboBox* resetCombo_;
    QLineEdit* dataWidthEdit_;
    QCheckBox* clockCheckBox_;
    QCheckBox* resetCheckBox_;

    QVBoxLayout* instancesContainerLayout_;
    QVBoxLayout* initiatorsContainerLayout_;
    QVBoxLayout* targetsContainerLayout_;

    int previousIndex_ = -1;
    bool ignoreChange_ = false;

    QSet<QSharedPointer<ConfigurableVLNVReference>> absRefs_;
    QHash<QString, QSet<QSharedPointer<BusInterface > > > instanceBusesHash_;
    QHash<QSharedPointer<BusInterface >, QSet<QString > > interfaceAbsDefsHash_;
    QSet<QString> addedInitiators_;
    QSet<QString> addedTargets_;
    QHash<QString, QList<QSharedPointer<BusInterface>>> selectedInitiators_;
    QHash<QString, QList<QSharedPointer<TargetData>>> selectedTargets_;

    const QSet<General::InterfaceMode> initiatorModes_ = { General::MASTER, General::INITIATOR };
    const QSet<General::InterfaceMode> targetModes_ = { General::SLAVE, General::TARGET };
};

#endif // INTERCONNECTGENERATORDIALOG_H
