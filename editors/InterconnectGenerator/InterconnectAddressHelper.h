#ifndef INTERCONNECTADDRESSHELPER_H
#define INTERCONNECTADDRESSHELPER_H

#include <QHash>
#include <QString>
#include <QSharedPointer>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/ResetInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/AccessPolicyInterface.h>
#include <KactusAPI/include/ModeReferenceInterface.h>
#include <KactusAPI/include/ParametersInterface.h>
#include <KactusAPI/include/MessageMediator.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/PortmapValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>


class InterconnectAddressHelper {
public:
    InterconnectAddressHelper(VLNV designVLNV,
        LibraryHandler* library, MessageMediator* messager);

    virtual ~InterconnectAddressHelper() = default;

    bool getTargetAddressRange(const QString& instanceName,
        const QString& interfaceName,
        quint64& outStart,
        quint64& outRange);

    void releaseTargetAddress(const QString& instanceName);

    void reset();

private:
    void initialize(QSharedPointer<Component> component);
    void createValidators(QSharedPointer<Component> component);
    void createInterfaces(QSharedPointer<Component> component);

    QSharedPointer<Component> loadComponentForInstance(const QString& instanceName);
    QString findMatchingMemoryMap(QSharedPointer<Component> component, const QString& interfaceName);
    quint64 calculateTotalRange(QSharedPointer<Component> component, const QString& mapName);
    bool assignAddressRange(const QString& instanceName, quint64 totalRange,
        quint64& outStart, quint64& outRange);
    void mergeFreeSpaces();

    MessageMediator* messager_;

    QSharedPointer<ExpressionParser> expressionParser_;
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
    QSharedPointer<ListParameterFinder> listParameterFinder_;
    LibraryHandler* library_;

    QSharedPointer<ParameterValidator> parameterValidator_;
    QSharedPointer<EnumeratedValueValidator> enumValidator_;
    QSharedPointer<FieldValidator> fieldValidator_;
    QSharedPointer<RegisterValidator> registerValidator_;
    QSharedPointer<RegisterFileValidator> registerFileValidator_;
    QSharedPointer<AddressBlockValidator> addressBlockValidator_;
    QSharedPointer<SubspaceMapValidator> subspaceValidator_;
    QSharedPointer<MemoryMapValidator> memoryMapValidator_;
    QSharedPointer<PortMapValidator> portMapValidator_;
    QSharedPointer<BusInterfaceValidator> busValidator_;

    QSharedPointer<MemoryMapInterface> memoryMapInterface_;
    QSharedPointer<AddressBlockInterface> addressBlockInterface_;
    ResetInterface* resetInterface_;
    FieldInterface* fieldInterface_;
    ModeReferenceInterface* modeRefInterface_;
    AccessPolicyInterface* accessPolicyInterface_;
    BusInterfaceInterface* busInfInterface_;
    QSharedPointer<ParametersInterface> parameterInterface_;
    RegisterInterface* registerInterface_;

    //! Interface for accessing interconnections.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for accessing ad hoc connections.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for accessing component instances.
    ComponentInstanceInterface* instanceInterface_{ new ComponentInstanceInterface(connectionInterface_,
        adhocConnectionInterface_) };

    QSharedPointer<Component> currentComponent_;
    QHash<QString, QPair<quint64, quint64>> usedAddressRanges_;
    quint64 nextAvailableAddress_ = 0;
    QList<QPair<quint64, quint64>> freeAddressSpaces_;
};

#endif // INTERCONNECTADDRESSHELPER_H