#ifndef INTERCONNECTGENERATORDIALOG_H
#define INTERCONNECTGENERATORDIALOG_H


#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <editors/common/DesignWidget.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/EnumCollectionEditor/InterfaceEnumEditor.h>
#include <common/widgets/instanceWidgets/InstanceWidgets.h>

#include <KactusAPI/include/ConfigJsonParser.h>
#include <KactusAPI/include/InterconnectGenerator.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/ComponentAndInstantiationsParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>
#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>

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

class InterconnectGeneratorDialog : public QDialog
{
    Q_OBJECT
public:
    InterconnectGeneratorDialog(DesignWidget* designWidget, LibraryHandler* library,
        MessageMediator* messager, QWidget* parent = nullptr);
    ~InterconnectGeneratorDialog() = default;

    ConfigStruct* getConfig();

    QHash<QString, QList<QSharedPointer<BusInterface > > > getSelectedInitiators();
    QHash<QString, QList<QSharedPointer<TargetData > > > getSelectedTargets();

protected:
    void accept() override;

private:
    void getBusesFromInstances();
    void getBusesFromTopComponent();
    void filterValidAbstractionReferences();
    void getAbstractionDefinitions(QSharedPointer<BusInterface> bus, bool fromInstance = true);
    void addNewInitiator();
    void addNewTarget();
    void addInstance(const QString& type);
    QVBoxLayout* createNameAndEnumerationEditorLayout(const QString& type, InterfaceEnumEditor* interfaceEditor, QFrame* instanceFrame);
    void clearInitiatorAndTargetLists();
    void populateParameters();
    void onInstanceSelected(const QString& instanceName, const QString& type, InterfaceEnumEditor* interfaceEditor);
    void updateNameCombos();
    void updateNameCombo(QComboBox* nameCombo, const QString& instanceType, QStringList& availableInstances);
    void setUpLayout();
    bool collectInstances(ConfigStruct* config);
    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> createInstanceBusesLookup(
        const QHash<QString, QSet<QSharedPointer<BusInterface>>>& instanceBusesHash);
    void collectInitiators(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);
    void collectTargets(const QHash<QString, QHash<QString, QSharedPointer<BusInterface>>>& instanceBusesLookup);

    LibraryHandler* library_;
    ConfigStruct* config_;
    VLNVEditor* vlnvEditor_;
    ParameterGroupBox* parameterGroupBox_;
    MessageMediator* messager_;
    DesignWidget* designWidget_;
    QSharedPointer<Component> selectedComponent_;
    QString designVLNV_;
    InstanceComboBox* componentInstances_;
    QVBoxLayout* instancesContainerLayout_;
    QVBoxLayout* initiatorsContainerLayout_;
    QVBoxLayout* targetsContainerLayout_;

    QSet<QSharedPointer<ConfigurableVLNVReference>> absRefs_;
    QSet<QString> addedInitiators_;
    QSet<QString> addedTargets_;
    QHash<QString, QSet<QSharedPointer<BusInterface > > > instanceBusesHash_;
    QHash<QString, QList<QSharedPointer<BusInterface > > > selectedInitiators_;
    QHash<QString, QList<QSharedPointer<TargetData > > > selectedTargets_;
    QHash<QSharedPointer<BusInterface >, QSet<QString > > interfaceAbsDefsHash_;
    const QSet<General::InterfaceMode > initiatorModes_ = { General::MASTER, General::INITIATOR };
    const QSet<General::InterfaceMode > targetModes_ = { General::SLAVE, General::TARGET };
    QLineEdit* dataWidthEdit_;
    InstanceComboBox* clockCombo_;
    InstanceComboBox* resetCombo_;
    InstanceComboBox* busInterfaceCombo_;
    int previousIndex_ = -1;
    bool ignoreChange_ = false;

    QCheckBox* clockCheckBox_;
    QCheckBox* resetCheckBox_;
};

#endif // INTERCONNECTGENERATORDIALOG_H
