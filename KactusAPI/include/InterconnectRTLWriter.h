//-----------------------------------------------------------------------------
// File: InterconnectRTLWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Multiple
// Date: 07.2025
//
// Description:
// Generates top-level Verilog interconnects for supported bus types utilizing
// Pulp Platform libraries (https://github.com/pulp-platform).
//-----------------------------------------------------------------------------

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
    //! The constructor
    InterconnectRTLWriter(QSharedPointer<Component> component, LibraryInterface* library,
                          MessageMediator* messager, QString directory, ConfigStruct* config,
                          QString clk, QString rst);

    //! The destructor
    ~InterconnectRTLWriter() = default;

    //! Called externally to generate the interconnect code
    void generateRTL();

    //! No copying. No assignment.
    InterconnectRTLWriter(const InterconnectRTLWriter& other) = delete;
    InterconnectRTLWriter& operator=(const InterconnectRTLWriter& other) = delete;

private:

    /*!
     *  Calls method 'writeAxiParams' and outputs 'AXI_BUS' bus instances to the
     *  given stream.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAXI4(QTextStream& stream);


    /*!
     *  Calls method 'writeAxiParams' and outputs 'AXI_LITE' bus instances to
     *  the given stream.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAXI4LITE(QTextStream& stream);


    /*!
     *  Outputs local parameters for numbers of initiators, targets, and ID width.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAxiParams(QTextStream& stream);


    /*!
     *  Outputs AddrMapXbar-specific content, including typedef for
     *  'axi_pkg::xbar_rule_', initialization of AddrMapXbar, and calls to
     *  'parseAddress' to get correct memory addresses in hexadecimal form.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAxiAddrMap(QTextStream& stream);


    /*!
     *  Outputs a localparam for configuring the submodule of an AXI crossbar.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAxiXbarCfg(QTextStream& stream);


    /*!
     *  Checks that 'InitList' and 'TargetList' contain at least one object.
     *  Then outputs initialization of an AXI or AXI Lite crossbar submodule.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeAxiXbar(QTextStream& stream);


    /*!
     *  Outputs signal assignments between initiator/target and the 
     *  corresponding interface module. Performs null pointer checks for
     *  'component_' and each of its ports
     *
     *    @param [in] stream    Verilog file output stream
     *    @param [in] busName   Name of the current interface
     *                          (e.g. 'jtag_dbg_wrapper_OBI_T')
     *    @param [in] index     Index of the current interface
     *    @param [in] isInit    Boolean indicating whether the interface
     *                          represents an initiator or target
     */
    void writeAxiAssign(QTextStream& stream, QString busName, int index, bool isInit);


    /*!
     *  Returns immediately if 'TargetList' is empty. Iterates through each
     *  target and calls the 'writeAxiAssign' function.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeTargetAssign(QTextStream& stream);


    /*!
     *  Returns immediately if 'InitList' is empty. Iterates through each
     *  initiator and calls the 'writeAxiAssign' function.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeInitAssign(QTextStream& stream);


    /*!
     *  Outputs OBI-specific local parameters for various integers and the
     *  'obi_cfg' parameter used in initialization of submodules.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeObiParams(QTextStream& stream);


    /*!
     *  Outputs instantiations of interface modules for initiators and targets.
     *
     *    @note Due to a known issue in the used library when only one initiator
     *          is present, an extra initiator interface is added in that case.
     *
     *    @param [in] stream  Verilog file output stream
     */
    void writeObiInterfaces(QTextStream& stream);


    /*!
     *  Outputs a packed struct and address map array for OBI target address
     *  regions. Calls 'parseAddress' and modifies address ranges for ObiAddrXbar.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeObiAddrMap(QTextStream& stream);


    /*!
     *  Outputs initialization of an OBI crossbar interface.
     *
     *    @param [in] stream    Verilog file output stream
     */
    void writeObiXbar(QTextStream& stream);


    /*!
     *  Iterates through a Verilog file and removes the line containing the old
     *  'endmodule' in order to place a new one at the end.
     *
     *    @param [in] file    Verilog file that will be modified
     */
    void removeEndmodule(QFile& file);


    /*!
     *  Returns an n*4-width QString of spaces to create indentation.
     *
     *    @param [in] n   Depth of the indentation
     *    @return         A QString containing n*4 spaces
     */
    QString indent(int n = 1);


    /*!
     *  Tries to parse an address from hexadecimal, binary, or decimal format into
     *  hexadecimal format with an "<AddressWidth>'h" prefix. If parsing fails,
     *  returns the trimmed original input value.
     *
     *    @param [in] original   String that is tried to be parsed
     *    @return   A QString containing either the parsed address or trimmed
     *              original input
     */
    QString parseAddress(QString original);


    /*!
     *  Returns a section comment with given title and possible subtitle.
     *
     *    @param [in] title     Title, text that is written first
     *    @param [in] subtitle  Subtitle, text that is written second
     *    @return               A QString containing the section comment
     */
    QString commentWriter(QString title, QString subtitle = "");

    QSharedPointer<Component> component_{nullptr};

    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    //! Configure struct defined in 'ConfigJsonParser.h'
    ConfigStruct* config_;

    QString directory_;
    QString clkPort_;
    QString rstPort_;

    //! Based on ports defined in 'axi/src/axi_intf.sv'
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


    //! Based on ports defined in 'axi/src/axi_intf.sv'
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


    //! Based on ports defined in 'obi/src/obi_intf.sv'
    QStringList obiPorts_ = {
        // Output
        "req", "reqpar", "gnt", "gntpar", "addr", "we", "be", "wdata", "aid", "a_optional",

        // Input
        "rvalid", "rvalidpar", "rready", "rreadypar", "rdata", "rid", "err", "r_optional"
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
    QString obiAddrRule_ = "addr_rule_t";
    QString obiInitInterface_ = "obi_init_bus";
    QString obiTargetInterface_ = "obi_target_bus";

    //! Defined in 'obi/src/obi_pkg.sv'
    QStringList obiXbarCfgParams_ = {
        "ADDR_WIDTH",
        "DATA_WIDTH",
        IdWidthInits_,
        "obi_pkg::ObiMinimalOptionalConfig"
    };

    /*QStringList obiXbarCfgParams_ = {
        /*"AUSER_WIDTH",
        "WUSER_WIDTH",
        "RUSER_WIDTH",
        "MID_WIDTH",
        "ACHK_WIDTH",
        "RCHK_WIDTH"
    };*/
};

#endif // INTERCONNECTRTLWRITER_H
