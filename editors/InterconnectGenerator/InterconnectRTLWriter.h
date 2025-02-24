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
                          MessageMediator* messager, QString directory, ConfigStruct* config,
                          QString clk, QString rst);

    ~InterconnectRTLWriter() = default;

    void generateRTL();

    void writeBus(QTextStream& stream, QString type);

    void writeAddrMap(QTextStream& stream);

    void writeXbarCfg(QTextStream& stream);

    void writeXbar(QTextStream& stream);

    void writeAssign(QTextStream& stream, QString busName, int index);

    void writeTargetAssign(QTextStream& stream);

    void writeInitAssign(QTextStream& stream);

    //! No copying. No assignment.
    InterconnectRTLWriter(const InterconnectRTLWriter& other) = delete;
    InterconnectRTLWriter& operator=(const InterconnectRTLWriter& other) = delete;

private:

    QSharedPointer<Component> component_{nullptr};

    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    ConfigStruct* config_;

    QString directory_;
    QString clkPort_;
    QString rstPort_;

    QStringList axiLitePorts_ = {"aw_addr", "aw_prot", "aw_valid", "aw_ready",
                                "w_data", "w_strb", "w_valid", "w_ready",
                                "b_reasp", "b_valid", "b_ready",
                                "ar_addr", "ar_prot", "ar_valid", "ar_ready",
                                "r_data", "r_resp", "r_valid", "r_ready"};

    QStringList axiPorts_ = {"aw_id", "aw_addr", "aw_len", "aw_burst", "aw_lock",
                            "aw_cache", "aw_prot", "aw_qos", "aw_region",
                            "aw_atop", "aw_user", "aw_valid", "aw_ready",
                            "w_data", "w_strb", "w_last", "w_user", "w_valid", "w_ready",
                            "b_id", "b_resp", "b_user", "b_valid", "b_ready",
                            "ar_id", "ar_addr", "ar_len", "ar_burst", "ar_lock",
                            "ar_cache", "ar_prot", "ar_qos", "ar_region",
                            "ar_atop", "ar_user", "ar_valid", "ar_ready",
                            "r_id", "r_data", "r_resp", "r_last", "r_user", "r_valid", "r_ready"};

    QString axiTargetBus_ = "_target";
    QString axiInitBus_ = "_init";
    QString axiXbar_;

    QString axiTargetParam_ = "AXI_TARGETS";
    QString axiInitParam_ = "AXI_INITIATORS";
    QString axiIdWidthInits_ = "IdWidthInits";
    QString addrRulesParam_ = "NoAddrRules";
    QString ruleType_ = "rule_t";
    QString addrMapXBAR_ = "AddrMapXBAR";
    QString axiCfg_ = "xbar_cfg";
};

#endif // INTERCONNECTRTLWRITER_H
