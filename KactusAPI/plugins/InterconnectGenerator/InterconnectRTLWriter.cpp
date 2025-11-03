//-----------------------------------------------------------------------------
// File: InterconnectRTLWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Teemu Hanhisuanto, Linda Virtanen
// Date: 07.2025
//
// Description:
// Generates top-level Verilog interconnects for supported bus types utilizing
// Pulp Platform libraries (https://github.com/pulp-platform).
//-----------------------------------------------------------------------------

#include "InterconnectRTLWriter.h"

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>

using namespace InterconnectGeneration;

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::InterconnectRTLWriter
//-----------------------------------------------------------------------------
InterconnectRTLWriter::InterconnectRTLWriter(QSharedPointer<Component> component,
    LibraryInterface* library, MessageMediator* messager, QString directory,
    ConfigStruct* config, QString clk, QString rst)
{
    component_ = component;
    library_ = library;
    messager_ = messager;
    directory_ = directory;
    config_ = config;
    clkPort_ = clk;
    rstPort_ = rst;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::generateRTL
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::generateRTL()
{

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConfig;
    for (IPlugin* plugin : PluginManager::getInstance().getActivePlugins()) {
        CLIGenerator* runnable = dynamic_cast<CLIGenerator*>(plugin);
        if (runnable != 0)
        {
            if (runnable->getOutputFormat().toLower() == "verilog") {
                PluginUtilityAdapter adapter(library_, messager_, VersionHelper::createVersionString(), nullptr);
                runnable->runGenerator(&adapter, component_, design, designConfig, "", directory_);
            }
        }
    }

    QString verilogDirectory = directory_ + "/" + component_->getVlnv().getName() + ".v";
    messager_->showMessage(QString("Opening file %1").arg(verilogDirectory));
    QFile verilogFile(verilogDirectory);

    QString verilogTxt;
    QTextStream verilogRTL(&verilogFile);
    
    if (verilogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!verilogRTL.atEnd()) {
            QString line = verilogRTL.readLine();
            verilogTxt.append(line + "\n");
            if (line.startsWith(");")) {
                break;
            }
        }
    }
    verilogFile.close();

    if (verilogFile.open(QIODevice::WriteOnly)) {
        verilogRTL << verilogTxt << Qt::endl;
    }
    verilogFile.close();

    auto usedInterface = config_->busType;

    if (verilogFile.open(QIODevice::WriteOnly | QIODevice::Append)) {

        switch (usedInterface)
        {
        case BusType::AXI4:
            writeAXI4(verilogRTL);
            writeAxiAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL);
            break;
        case BusType::AXI4LITE:
            writeAXI4LITE(verilogRTL);
            writeAxiAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL);
            break;
        case BusType::OBI:
            writeObiParams(verilogRTL);
            writeObiInterfaces(verilogRTL);
            writeObiAddrMap(verilogRTL);
            writeObiXbar(verilogRTL);
            break;
        default:
            messager_->showMessage(QString(
                "*** No RTL generated: Interface type is not supported."));

            verilogRTL << Qt::endl;
            verilogRTL << "// Could not implement the interconnect: unknown interface type" << Qt::endl;

            return;
        }

        verilogRTL << commentWriter("Signal assignments") << "\n";
        writeTargetAssign(verilogRTL);
        writeInitAssign(verilogRTL);
        removeEndmodule(verilogFile);
        verilogRTL << "endmodule" << Qt::endl;
    }
    
    verilogFile.close();
    messager_->showMessage(QString("Closed file %1").arg(verilogDirectory));
    return;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeAXI4
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAXI4(QTextStream& stream) {

    writeAxiParams(stream);

    stream << commentWriter("Initiator and target interface(s)") << "\n";

    auto busTypeStr = ConfigJsonParser::busTypeToStr(config_->busType).toLower();

    if (!config_->targetList.isEmpty()) {
        stream << indent()  << "AXI_BUS #(" << Qt::endl;
        stream << indent(2) << ".AXI_ID_WIDTH(" << IdWidthInits_ << ")," << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->userWidth << "),      // Adjustable" << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(ADDR_WIDTH)," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(DATA_WIDTH)" << Qt::endl;

        axiTargetBus_ = busTypeStr + axiTargetBus_;
        stream << indent() << ") " << axiTargetBus_;
        stream << "[" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }

    if (!config_->initList.isEmpty()) {
        stream << indent()  << "AXI_BUS #(" << Qt::endl;
        stream << indent(2) << ".AXI_ID_WIDTH(" << IdWidthInits_ << ")," << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->userWidth << "),      // Adjustable" << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(ADDR_WIDTH)," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(DATA_WIDTH)" << Qt::endl;

        axiInitBus_ = busTypeStr + axiInitBus_;
        stream << indent() << ") " << axiInitBus_;
        stream << "[" << axiInitParam_ << "-1:0]();\n" << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeAXI4LITE
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAXI4LITE(QTextStream& stream) {

    writeAxiParams(stream);

    stream << commentWriter("Initiator and target interface(s)") << "\n";
    auto busTypeStr = ConfigJsonParser::busTypeToStr(config_->busType).toLower();

    if (!config_->targetList.isEmpty()) {
        stream << indent()  << "AXI_LITE #("      << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(ADDR_WIDTH)," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(DATA_WIDTH)" << Qt::endl;

        axiTargetBus_ = busTypeStr + axiTargetBus_;
        stream << "    ) " << axiTargetBus_;
        stream << " [" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }

    if (!config_->initList.isEmpty()) {
        stream << indent()  << "AXI_LITE #("      << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(ADDR_WIDTH)," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(DATA_WIDTH)" << Qt::endl;

        axiInitBus_ = busTypeStr + axiInitBus_;
        stream << indent() << ") " << axiInitBus_;
        stream << " [" << axiInitParam_ << "-1:0]();\n" << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeAxiParams
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAxiParams(QTextStream& stream) {

    stream << commentWriter("Local parameters") << "\n";

    stream << indent() << "localparam " << axiTargetParam_
        << " = " << config_->targetList.size() << ";\n";

    stream << indent() << "localparam " << axiInitParam_
        << " = " << config_->initList.size() << ";\n";
    
    stream << indent() << "localparam " << IdWidthInits_ << " = ";
        stream << config_->idWidth << " + " << "$clog2(" << axiTargetParam_ << ");\n";

    stream << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeAxiAddrMap
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAxiAddrMap(QTextStream& stream)
{
    int targetRegions = 0;
    for (TargetStruct const& target : config_->targetList) {
        if (target.addressRegions.length() > 1) {
            targetRegions += target.addressRegions.length() - 1;
        }
    }
    stream << commentWriter("Address mapping") << "\n";

    stream << indent() << "localparam " << addrRulesParam_ << " = "
        << axiTargetParam_ << " + " << targetRegions << ";\n";

    stream << indent() << "typedef axi_pkg::xbar_rule_" << config_->addressWidth
        << "_t " << ruleType_ << ";\n" << Qt::endl;
    stream << indent() << ruleType_ << " [" << addrRulesParam_ << "-1:0] "
        << addrMapXBAR_ << ";\n" << Qt::endl;
    stream << indent() << "assign " << addrMapXBAR_ << " = '{" << Qt::endl;

    int regionCounter = 0;

    for (TargetStruct const& target : config_->targetList) {
        for (AddressPair const& addrPair : target.addressRegions) {
            regionCounter += 1;

            QString startStr = parseAddress(addrPair.start);
            QString rangeStr = parseAddress(addrPair.end);

            // Add range to target address to get the end address
            bool startOk, rangeOk;

            uint64_t startValue = startStr.mid(startStr.indexOf('h') + 1).toULongLong(&startOk, 16);
            uint64_t endValue = rangeStr.mid(startStr.indexOf('h') + 1).toULongLong(&rangeOk, 16);

            if (startOk && rangeOk) {
                uint64_t sum = startValue + endValue + 1; // start >=, end <
                rangeStr = /*QString::number(config_->AddressWidth)*/ + "'h" + QString::number(sum, 16).toUpper();
            }
            else
            {
                // if parameters are used, create new expression for end andress using them
                rangeStr = startStr + "+" + rangeStr;
            }
            
            stream << indent(2)
                << "'{idx: " << IdWidthInits_ << "'('d" << target.index
                << "), start_addr: ADDR_WIDTH'(" << startStr
                << "), end_addr: ADDR_WIDTH'(" << rangeStr << ")"
                << ((regionCounter == targetRegions + config_->targetList.size()) ? "} " : "},")
                << " // Target: " << target.name << "\n";
        }
    }
    stream << indent() << "};\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeAxiXbarCfg
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAxiXbarCfg(QTextStream& stream)
{
    stream << commentWriter("Crossbar configuration", "See definitions in 'axi_pkg.sv'") << "\n";

    stream << indent()  << "localparam axi_pkg::xbar_cfg_t " << axiCfg_ << " = '{" << Qt::endl;
    stream << indent(2) << "NoSlvPorts:         " << axiTargetParam_ << "," << Qt::endl;
    stream << indent(2) << "NoMstPorts:         " << axiInitParam_ << "," << Qt::endl;
    stream << indent(2) << "MaxMstTrans:        1,    // Adjustable" << Qt::endl;
    stream << indent(2) << "MaxSlvTrans:        1,    // Adjustable" << Qt::endl;
    stream << indent(2) << "FallThrough:        1'b0, // Adjustable" << Qt::endl;
    stream << indent(2) << "PipelineStages:     0,    // Adjustable" << Qt::endl;
    stream << indent(2) << "LatencyMode:        axi_pkg::CUT_ALL_AX, // Options:"
        << " NO_LATENCY, CUT_SLV_AX, CUT_MST_AX, CUT_ALL_AX, CUT_SLV_PORTS,"
        << " CUT_MST_PORTS, CUT_ALL_PORTS" << Qt::endl;
    stream << indent(2) << "AxiIdWidthSlvPorts: " << IdWidthInits_ << "," << Qt::endl;
    stream << indent(2) << "AxiIdUsedSlvPorts:  0,    // Adjustable" << Qt::endl;
    stream << indent(2) << "UniqueIds:          1'b1, // Adjustable" << Qt::endl;
    stream << indent(2) << "AxiAddrWidth:       ADDR_WIDTH," << Qt::endl;
    stream << indent(2) << "AxiDataWidth:       DATA_WIDTH," << Qt::endl;
    stream << indent(2) << "NoAddrRules:        " << addrRulesParam_ << Qt::endl;
    stream << indent() << "};\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::witeAxiXbar
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeAxiXbar(QTextStream& stream) {

    if (config_->initList.isEmpty() || config_->targetList.isEmpty()) {
        return;
    }

    if (config_->busType == BusType::AXI4) {

        stream << commentWriter("Crossbar initialization", "See parameter definitions in 'axi_xbar.sv'") << "\n";
        axiXbar_ = "i_axi_xbar";
        stream << indent() << "axi_xbar_intf #(\n";
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->userWidth << "),\n";

    }
    else if (config_->busType == BusType::AXI4LITE) {

        stream << commentWriter("Crossbar initialization", "See parameter definitions in 'axi_lite_xbar.sv'") << "\n";
        axiXbar_ = "i_axi_lite_xbar";
        stream << indent() << "axi_lite_xbar_intf #(\n";

    } else {
        return;
    }
    
    stream << indent(2) << ".Cfg(" << axiCfg_ << "),\n";
    stream << indent(2) << ".rule_t(" << ruleType_ << ")\n";
    
    stream << indent() << ") " << axiXbar_ << "(\n";

    stream << indent(2) << ".clk_i(" << clkPort_ << "),\n";
    stream << indent(2) << ".rst_ni(" << rstPort_ << "),\n";
    stream << indent(2) << ".test_i(1'b0),               // Adjustable\n";
    stream << indent(2) << ".slv_ports(" << axiInitBus_ << "),\n";
    stream << indent(2) << ".mst_ports(" << axiTargetBus_ << "),\n";
    stream << indent(2) << ".addr_map_i(" << addrMapXBAR_ << "),\n";
    stream << indent(2) << ".en_default_mst_port_i('0),  // Adjustable\n";
    stream << indent(2) << ".default_mst_port_i('0)      // Adjustable\n";
    stream << indent() << ");\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeSignalAssignments
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeSignalAssignments(QTextStream& stream, QString busName, int index, bool isInit) {

    if (!component_) {
        return;
    }

    stream << indent() << "// Interface: " << busName << '\n' << Qt::endl;

    QString targetBus;
    QString initBus;

    auto busType = config_->busType;

    // Check which bus type is used and set the correct port list and bus names
    QStringList ports = (busType == BusType::AXI4) ? axiPorts_ : axiLitePorts_;
    if (busType == BusType::OBI) {
        ports = obiPorts_;
        targetBus = obiTargetInterface_;
        initBus = obiInitInterface_;
    } else {
        targetBus = axiTargetBus_;
        initBus = axiInitBus_;
    }

    // Assign input ports first
    for (QSharedPointer<Port> compPort : component_->getPortsMappedInInterface(busName)) {

        if (!compPort) {
            continue;
        }

        for (QString const& port : ports) {
            if (compPort->getDirection() != DirectionTypes::IN &&
                (compPort->name().endsWith("_" + port) ||
                 compPort->name().endsWith("_" + port + "_in")  ||
                 compPort->name().endsWith("_" + port + "_out") ||
                 compPort->name().endsWith("_" + port + "_i")   ||
                 compPort->name().endsWith("_" + port + "_o"))) {

                if (isInit) {
                    stream << indent() << "assign " << initBus << "[" << index << "]." << port;
                    stream << " = " << compPort->name() << ";" << Qt::endl;
                } else {
                    stream << indent() << "assign " << compPort->name();
                    stream << " = " << targetBus << "[" << index << "]." << port << ";" << Qt::endl;
                }
            }
        }
    }

    // Then assign other ports
    for (QSharedPointer<Port> compPort : component_->getPortsMappedInInterface(busName)) {

        if (!compPort) {
            return;
        }

        for (QString port : ports) {
            if (compPort->getDirection() == DirectionTypes::IN &&
                (compPort->name().endsWith("_" + port) ||
                 compPort->name().endsWith("_" + port + "_in")  ||
                 compPort->name().endsWith("_" + port + "_out") ||
                 compPort->name().endsWith("_" + port + "_i")   ||
                 compPort->name().endsWith("_" + port + "_o"))) {

                if (isInit) {
                    stream << indent() << "assign " << compPort->name();
                    stream << " = " << initBus << "[" << index << "]." << port << ";\n";
                } else {
                    stream << indent() << "assign " << targetBus << "[" << index << "]." << port;
                    stream << " = " << compPort->name() << ";\n";
                }
            }
        }
    }
    stream << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeTargetAssign
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeTargetAssign(QTextStream& stream) {

    if (config_->targetList.isEmpty()) return;

    for (const TargetStruct& target : config_->targetList) {
        writeSignalAssignments(stream, target.name, target.index, false);
        //stream << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeInitAssign
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeInitAssign(QTextStream& stream) {

    if (config_->initList.isEmpty()) return;

    for (const InitStruct& init : config_->initList) {
        writeSignalAssignments(stream, init.name, init.index, true);
        //stream << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeObiParams
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeObiParams(QTextStream& stream) {
    
    stream << commentWriter("Local parameters") << "\n";

    stream << indent() << "localparam " << obiInitParam_
        << " = " << config_->initList.size() << ";\n";

    stream << indent() << "localparam " << obiTargetParam_
        << " = " << config_->targetList.size() << ";\n";

    stream << "\n";
    
    stream << indent() << "localparam " << IdWidthInits_ << " = ";
        stream << config_->idWidth << " + " << "$clog2(" << obiTargetParam_ << ");\n";

    stream << indent() << "localparam " << addrRulesParam_
        << " = " << obiTargetParam_ << ";\n";

    stream << "\n";

    //stream << indent()  << "localparam obi_pkg::obi_optional_cfg_t obi_cfg = obi_pkg::obi_all_optional_config(\n";
    stream << indent()  << "localparam obi_pkg::obi_cfg_t obi_cfg = obi_pkg::obi_default_cfg(\n";
    
    // Print parameters of the configure struct
    for (int i = 0; i < obiXbarCfgParams_.size(); ++i) {
        const QString& param = obiXbarCfgParams_[i];
        bool isLast = (i == obiXbarCfgParams_.size() - 1);
        stream << indent(2) << param;
        if (!isLast)
            stream << ",";
        stream << "\n";
    }
    stream << indent() << ");\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeObiInterfaces
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeObiInterfaces(QTextStream& stream) {

    stream << commentWriter("Initiator and target interface(s)") << "\n";

    if (!config_->initList.isEmpty()) {
        stream << indent()  << "OBI_BUS #(\n";
        stream << indent(2) << ".OBI_CFG(obi_cfg)\n";

        if (config_->initList.size() == 1) {
            stream << indent()  << ") " << obiInitInterface_ << " [(" << obiInitParam_
                << "+1)-1:0](); // NOTE: Extra interface due to known single initiator issue\n" << Qt::endl;
        } else {
            stream << indent()  << ") " << obiInitInterface_ << " [" << obiInitParam_ << "-1:0]();\n" << Qt::endl;
        }
    }

    if (!config_->targetList.isEmpty()) {
        stream << indent()  << "OBI_BUS #(\n";
        stream << indent(2) << ".OBI_CFG(obi_cfg)\n";
        stream << indent()  << ") " << obiTargetInterface_ << " [" << obiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeObiAddrMap
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeObiAddrMap(QTextStream& stream) {

    stream << commentWriter("Address mapping") << "\n";

    int targetRegions = 0;
    for (TargetStruct const& target : config_->targetList) {
        if (target.addressRegions.length() > 1) {
            targetRegions += target.addressRegions.length() - 1;
        }
    }

    stream << indent() << "typedef struct packed {\n";
    stream << indent(2) << "int unsigned idx;\n";
    stream << indent(2) << "logic [ADDR_WIDTH-1:0] start_addr;\n";
    stream << indent(2) << "logic [ADDR_WIDTH-1:0] end_addr;\n";
    stream << indent() << "} " << obiAddrRule_ << " ;\n" << Qt::endl;

    stream << indent() << obiAddrRule_ << " " << addrMapXBAR_ << " [" << addrRulesParam_ << "] = '{\n";

    int regionCounter = 0;

    for (TargetStruct const& target : config_->targetList) {
        for (AddressPair const& addrPair : target.addressRegions) {
            regionCounter += 1;

            QString startStr = parseAddress(addrPair.start);
            QString rangeStr = parseAddress(addrPair.end);

            // Add range to target address to get the end address
            bool startOk, rangeOk;

            uint64_t startValue = startStr.mid(startStr.indexOf('h') + 1).toULongLong(&startOk, 16);
            uint64_t endValue = rangeStr.mid(startStr.indexOf('h') + 1).toULongLong(&rangeOk, 16);

            if (startOk && rangeOk) {
                uint64_t sum = startValue + endValue + 1; // start >=, end <
                rangeStr = QString::number(config_->addressWidth) + "'h" + QString::number(sum, 16).toUpper();
            }

            stream << indent(2)
                << "'{idx: " << IdWidthInits_ << "'('d" << target.index
                << "), start_addr: ADDR_WIDTH'(" << startStr
                << "), end_addr: ADDR_WIDTH'(" << rangeStr << ")"
                << ((regionCounter == targetRegions + config_->targetList.size()) ? "} " : "},")
                << " // Target: " << target.name << "\n";
        }
    }
    stream << indent() << "};\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeObiXbar
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::writeObiXbar(QTextStream& stream) {

    stream << commentWriter("Crossbar initialization", "See parameter definitions in 'obi_xbar.sv'") << "\n";

    stream << indent() << "obi_xbar_intf #(\n";
    stream << indent(2) << ".NumSbrPorts       (" << obiInitParam_ << "),\n";
    stream << indent(2) << ".NumMgrPorts       (" << obiTargetParam_ << "),\n";
    stream << indent(2) << ".NumMaxTrans       (1),          // Adjustable\n";
    stream << indent(2) << ".NumAddrRules      (" << addrRulesParam_ << "),\n";
    stream << indent(2) << ".addr_map_rule_t   (" << obiAddrRule_ << "),\n";
    stream << indent(2) << ".UseIdForRouting   (0)           // Adjustable\n";
    stream << indent() << ") i_obi_xbar (\n";
    stream << indent(2) << ".clk_i             (" << clkPort_ << "),\n";
    stream << indent(2) << ".rst_ni            (" << rstPort_ << "),\n";
    stream << indent(2) << ".testmode_i        (1'b0),       // Adjustable\n";
    stream << indent(2) << ".sbr_ports         (" << obiInitInterface_ << "),\n";
    stream << indent(2) << ".mgr_ports         (" << obiTargetInterface_ << "),\n";
    stream << indent(2) << ".addr_map_i        (" << addrMapXBAR_ << "),\n";
    stream << indent(2) << ".en_default_idx_i  ('0),         // Adjustable\n";
    stream << indent(2) << ".default_idx_i     ('0)          // Adjustable\n";
    stream << indent() << ");\n" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::writeEndmodule
//-----------------------------------------------------------------------------
void InterconnectRTLWriter::removeEndmodule(QFile& file) {

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString newContent;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed() != "endmodule") {
            newContent.append(line + '\n');
        }
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << newContent;

    file.close();
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::indent
//-----------------------------------------------------------------------------
QString InterconnectRTLWriter::indent(int n) {

    return QString(n * 4, ' ');
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::parseAddress
//-----------------------------------------------------------------------------
QString InterconnectRTLWriter::parseAddress(QString original) {

    QString parsed = original.trimmed();

    if (parsed.isEmpty()) {
        return parsed;
    }

    // Checks if the given address is in hexadecimal, binary, or decimal format.
    // Attempts to convert it to hexadecimal before returning.
    // Otherwise, returns the trimmed original QString
    if (parsed.contains("'h", Qt::CaseInsensitive)) {

        int index = parsed.indexOf("'h", Qt::CaseInsensitive);
        parsed = parsed.mid(index + 2).trimmed();
        return /*QString::number(config_->AddressWidth)*/ + "'h" + parsed.toUpper();
    }

    else if (parsed.startsWith("0x", Qt::CaseInsensitive)) {

        parsed = parsed.mid(2);
        return /*QString::number(config_->AddressWidth)*/ + "'h" + parsed.toUpper();
    }
    
    else if (parsed.contains("'b", Qt::CaseInsensitive)) {

        int index = parsed.indexOf("'b", Qt::CaseInsensitive);
        parsed = parsed.mid(index + 2).trimmed();

        bool ok;
        uint64_t temp = parsed.toULongLong(&ok, 2);
        
        if (ok) {
            parsed = /*QString::number(config_->AddressWidth) +*/ "'h" + QString::number(temp, 16).toUpper();
            return parsed;
        } else {
            return parsed;
        }
    }

    else if (parsed.startsWith("0b", Qt::CaseInsensitive)) {

        parsed = parsed.mid(2);

        bool ok;
        uint64_t temp = parsed.toULongLong(&ok, 2);
        
        if (ok) {
            parsed = /*QString::number(config_->AddressWidth) +*/ "'h" + QString::number(temp, 16).toUpper();
            return parsed;
        } else {
            return parsed;
        }
    }

    bool isDecimal;
    uint64_t dec = parsed.toULongLong(&isDecimal, 10);
    if (isDecimal) {
        parsed = /*QString::number(config_->AddressWidth) +*/ "'h" + QString::number(dec, 16).toUpper();
        return parsed;
    }
    
    return parsed;
}

//-----------------------------------------------------------------------------
// Function: InterconnectRTLWriter::commentWriter
//-----------------------------------------------------------------------------
QString InterconnectRTLWriter::commentWriter(QString title, QString subtitle) {

    QString comment = "\n//---------------------------------------------------\n"
                        "// " + title + "\n";

    if (!subtitle.isEmpty()) {
        comment +=      "// \n// " + indent() + "- " + subtitle + "\n";
    }

    comment +=          "//---------------------------------------------------\n";
    return comment;
}