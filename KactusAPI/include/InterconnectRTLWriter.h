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

    //! No copying. No assignment.
    InterconnectRTLWriter(const InterconnectRTLWriter& other) = delete;
    InterconnectRTLWriter& operator=(const InterconnectRTLWriter& other) = delete;

private:
    void writeBus(QTextStream& stream, QString type);

    void writeAxiAddrMap(QTextStream& stream);

    void writeAxiXbarCfg(QTextStream& stream);

    void writeAxiXbar(QTextStream& stream);

    void writeAxiAssign(QTextStream& stream, QString busName, int index);

    void writeTargetAssign(QTextStream& stream);

    void writeInitAssign(QTextStream& stream);

    void removeEndmodule(QFile& file);

    void writeAXI4(QTextStream& stream);

    void writeAXI4LITE(QTextStream& stream);

    void writeAxiParams(QTextStream& stream);

    void writeObiParams(QTextStream& stream);

    void writeObiInterfaces(QTextStream& stream);

    void writeObiAddrMap(QTextStream& stream);

    QString indent(int n = 1);

    QString parseAddress(QString original);

    QSharedPointer<Component> component_{nullptr};

    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    ConfigStruct* config_;

    QString directory_;
    QString clkPort_;
    QString rstPort_;

    /*QStringList axiLitePorts_ = {"aw_addr", "aw_port", "aw_valid", "aw_ready",
                                "w_data", "w_strb", "w_valid", "w_ready",
                                "b_reasp", "b_valid", "b_ready",
                                "ar_addr", "ar_prot", "ar_valid", "ar_ready",
                                "r_data", "r_resp", "r_valid", "r_ready"};*/


    // Based on ports defined in axi/src/axi_intf.sv
    QStringList axiPorts_ = {
        // Write address channel
        "aw_id", "aw_addr", "aw_len", "aw_size", "aw_burst", "aw_lock", "aw_cache",
        "aw_prot", "aw_qos", "aw_region", "aw_atop", "aw_user", "aw_writetoken", "aw_readpointer",

        // Write data channel
        "w_data", "w_strb", "w_last", "w_user", "w_writetoken", "w_readpointer",

        // Write response channel
        "b_id", "b_resp", "b_user", "b_writetoken", "b_readpointer",

        // Read address channel
        "ar_id", "ar_addr", "ar_len", "ar_size", "ar_burst", "ar_lock", "ar_cache",
        "ar_prot", "ar_qos", "ar_region", "ar_user", "ar_writetoken", "ar_readpointer",

        // Read data channel
        "r_id", "r_data", "r_resp", "r_last", "r_user", "r_writetoken", "r_readpointer"
    };


    /*QStringList axiPorts_ = {
        "aw_id", "aw_addr", "aw_len", "aw_size", "aw_burst", "aw_lock",
        "aw_cache", "aw_prot", "aw_qos", "aw_region", "aw_atop", "aw_user",
        "aw_valid", "aw_ready", "w_data", "w_strb", "w_last", "w_user",
        "w_valid", "w_ready", "b_id", "b_resp", "b_user", "b_valid",
        "b_ready", "ar_id", "ar_addr", "ar_len", "ar_size", "ar_burst", "ar_lock",
        "ar_cache", "ar_prot", "ar_qos", "ar_region", "ar_atop", "ar_user",
        "ar_valid", "ar_ready", "r_id", "r_data", "r_resp", "r_last",
        "r_user", "r_valid", "r_ready"
};*/


    // Based on ports defined in axi/src/axi_intf.sv
    QStringList axiLitePorts_ = {
        // Write address channel
        "aw_addr", "aw_prot", "aw_valid", "aw_ready",

        // Write data channel
        "w_data", "w_strb", "w_valid", "w_ready",

        // Write response channel
        "b_resp", "b_valid", "b_ready",

        // Read address channel
        "ar_addr", "ar_prot", "ar_valid", "ar_ready",

        // Read data channel
        "r_data", "r_resp", "r_valid", "r_ready"
    };

    QStringList obiXbarCfgParams_ = {
        "AUSER_WIDTH",
        "WUSER_WIDTH",
        "RUSER_WIDTH",
        "MID_WIDTH",
        "ACHK_WIDTH",
        "RCHK_WIDTH"
    };

    QStringList supportedInterfaces = {"axi4", "axi4lite", "obi"};
                                

    QString axiTargetBus_ = "_target";
    QString axiInitBus_ = "_init";
    QString axiXbar_;

    QString axiTargetParam_ = "AXI_TARGETS";
    QString axiInitParam_ = "AXI_INITIATORS";
    QString IdWidthInits_ = "IdWidthInits";
    QString addrRulesParam_ = "NoAddrRules";
    QString ruleType_ = "rule_t";
    QString addrMapXBAR_ = "AddrMapXBAR";
    QString axiCfg_ = "xbar_cfg";

    QString obiTargetParam_ = "OBI_TARGETS";
    QString obiInitParam_ = "OBI_INITIATORS";
};

#endif // INTERCONNECTRTLWRITER_H
