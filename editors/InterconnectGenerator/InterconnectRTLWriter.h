#ifndef INTERCONNECTRTLWRITER_H
#define INTERCONNECTRTLWRITER_H

#include "ConfigJsonParser.h"

#include <QtCore>

#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/include/PluginManager.h>
#include <KactusAPI/include/CLIGenerator.h>
#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/PluginUtilityAdapter.h>
#include <KactusAPI/include/VersionHelper.h>

class InterconnectRTLWriter
{
public:
    InterconnectRTLWriter(QSharedPointer<Component> component, LibraryInterface* library,
                          MessageMediator* messager, QString directory, ConfigJsonParser::ConfigStruct* config);

    ~InterconnectRTLWriter() = default;

    void generateRTL();

    void writeBus(QTextStream& stream, QString type);

    void writeAddrMap(QTextStream& stream);

    void writeXbar(QTextStream& stream);

    //! No copying. No assignment.
    InterconnectRTLWriter(const InterconnectRTLWriter& other) = delete;
    InterconnectRTLWriter& operator=(const InterconnectRTLWriter& other) = delete;

private:

    QSharedPointer<Component> component_{nullptr};

    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    ConfigJsonParser::ConfigStruct* config_;

    QString directory_;

    QString axiTargetBus_;
    QString axiInitBus_;

    QString axiTargetParam_ = "AXI_TARGETS";
    QString axiInitParam_ = "AXI_INITIATORS";
    QString addrRulesParam_ = "NoAddrRules";
    QString ruleType_ = "rule_t";
    QString addrMapXBAR_ = "AddrMapXBAR";
    QString axiCfg_ = "xbar_cfg";
};

#endif // INTERCONNECTRTLWRITER_H
