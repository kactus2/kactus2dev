#include "InterconnectRTLWriter.h"

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>



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

    QString usedInterfaceStr = config_->BusType.toLower();
    // usedInterfaceStr = "axi4"; //debug

    if (supportedInterfaces.contains(usedInterfaceStr)) {
        removeEndmodule(verilogFile);
    }

    if (verilogFile.open(QIODevice::WriteOnly | QIODevice::Append)) {

        if (usedInterfaceStr == "axi4") {

            writeAXI4(verilogRTL);
            writeAxiAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL);

        } else if (usedInterfaceStr == "axi4lite") {

            writeAXI4LITE(verilogRTL);
            writeAxiAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL);

        } else if (usedInterfaceStr == "obi") {

            writeObiParams(verilogRTL);
            writeObiInterfaces(verilogRTL);
            writeObiAddrMap(verilogRTL);
            writeObiXbar(verilogRTL);

        } else {

            messager_->showMessage(QString("*** No RTL generated: Interface type '%1' is not supported.").arg(config_->BusType));

            verilogRTL << Qt::endl;
            verilogRTL << "// Could not implement the interconnect: unknown interface type" << Qt::endl;

            return;
        }

        verilogRTL << commentWriter("Signal assignments") << "\n";
        writeTargetAssign(verilogRTL);
        writeInitAssign(verilogRTL);
        verilogRTL << "endmodule" << Qt::endl;
    }
    
    verilogFile.close();
    messager_->showMessage(QString("Closed file %1").arg(verilogDirectory));
    return;
}



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


void InterconnectRTLWriter::writeAXI4(QTextStream& stream) {

    writeAxiParams(stream);

    if (!config_->TargetList.isEmpty()) {
        stream << indent()  << "AXI_BUS #(" << Qt::endl;
        stream << indent(2) << ".AXI_ID_WIDTH(" << IdWidthInits_ << ")," << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

        axiTargetBus_ = config_->BusType.toLower() + axiTargetBus_;
        stream << indent() << ") " << axiTargetBus_;
        stream << "[" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }

    if (!config_->InitList.isEmpty()) {
        stream << indent()  << "AXI_BUS #(" << Qt::endl;
        stream << indent(2) << ".AXI_ID_WIDTH(" << IdWidthInits_ << ")," << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

        axiInitBus_ = config_->BusType.toLower() + axiInitBus_;
        stream << indent() << ") " << axiInitBus_;
        stream << "[" << axiInitParam_ << "-1:0]();\n" << Qt::endl;
    }
}


void InterconnectRTLWriter::writeAXI4LITE(QTextStream& stream) {

    writeAxiParams(stream);

    if (!config_->TargetList.isEmpty()) {
        stream << indent()  << "AXI_LITE #("      << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

        axiTargetBus_ = config_->BusType.toLower() + axiTargetBus_;
        stream << "    ) " << axiTargetBus_;
        stream << " [" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }

    if (!config_->InitList.isEmpty()) {
        stream << indent()  << "AXI_LITE #("      << Qt::endl;
        stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
        stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->InitList[0].DataWidth << ")" << Qt::endl;

        axiInitBus_ = config_->BusType.toLower() + axiInitBus_;
        stream << indent() << ") " << axiInitBus_;
        stream << " [" << axiInitParam_ << "-1:0]();\n" << Qt::endl;
    }
}


void InterconnectRTLWriter::writeAxiParams(QTextStream& stream) {

    stream << indent() << "localparam " << axiTargetParam_
        << " = " << config_->TargetList.size() << ";" << Qt::endl;

    stream << indent() << "localparam " << axiInitParam_
        << " = " << config_->InitList.size() << ";" << Qt::endl;
    
    stream << indent() << "localparam " << IdWidthInits_ << " = ";
        stream << config_->IDWidth << " + " << "$clog2(" << axiTargetParam_ << ");" << Qt::endl;
        stream << Qt::endl;
}


void InterconnectRTLWriter::writeAxiAddrMap(QTextStream& stream)
{
    int targetRegions = 0;
    for (TargetStruct target : config_->TargetList) {
        if (target.AddressRegions.length() > 1) {
            targetRegions += target.AddressRegions.length() - 1;
        }
    }

    stream << indent() << "localparam " << addrRulesParam_ << " = " << axiTargetParam_ << " + " << targetRegions << ";\n" << Qt::endl;

    stream << indent() << "typedef axi_pkg::xbar_rule_" << config_->AddressWidth << "_t " << ruleType_ << ";\n" << Qt::endl;
    stream << indent() << ruleType_ << " [" << addrRulesParam_ << "-1:0] " << addrMapXBAR_ << ";\n" << Qt::endl;
    stream << indent() << "assign " << addrMapXBAR_ << " = " << Qt::endl;
    stream << indent(2) << "'{" << Qt::endl;

    int regionCounter = 0;

    for (TargetStruct target : config_->TargetList) {
        for (AddressPair addrPair : target.AddressRegions) {
            regionCounter += 1;

            QString startStr = parseAddress(addrPair.Start);
            QString endStr = parseAddress(addrPair.End);
            
            stream << indent(3) << "'{idx: " << config_->IDWidth << "'d" << target.Index; // hard coded
                stream << ", start_addr: " << startStr;
                stream << ", end_addr: " << endStr;
            

            if (regionCounter == targetRegions + config_->TargetList.size()) {
                stream << "} ";
            }
            else {
                stream << "},";
            }
            stream << " // Target: " << target.Name << Qt::endl;
        }
    }
    stream << indent(2) << "};\n" << Qt::endl;
}


void InterconnectRTLWriter::writeAxiXbarCfg(QTextStream& stream)
{
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
    stream << indent(2) << "AxiIdWidthSlvPorts: " << config_->IDWidth << "," << Qt::endl;
    stream << indent(2) << "AxiIdUsedSlvPorts:  0,    // Adjustable" << Qt::endl;
    stream << indent(2) << "UniqueIds:          1'b1, // Adjustable" << Qt::endl;
    stream << indent(2) << "AxiAddrWidth:       " << config_->AddressWidth << "," << Qt::endl;
    stream << indent(2) << "AxiDataWidth:       " << config_->TargetList[0].DataWidth << "," << Qt::endl;
    stream << indent(2) << "NoAddrRules:        " << addrRulesParam_ << Qt::endl;
    stream << indent() << "};\n" << Qt::endl;
}


void InterconnectRTLWriter::writeAxiXbar(QTextStream& stream) {

    if (config_->InitList.isEmpty() || config_->TargetList.isEmpty()) {
        return;
    }

    if (config_->BusType.toLower() == "axi4") {

        axiXbar_ = "i_axi_xbar";
        stream << indent() << "axi_xbar_intf #(" << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;

    }
    else if (config_->BusType.toLower() == "axi4lite") {

        axiXbar_ = "i_axi_lite_xbar";
        stream << indent() << "axi_lite_xbar_intf #(" << Qt::endl;

    } else {
        return;
    }
    
    stream << indent(2) << ".Cfg(" << axiCfg_ << ")," << Qt::endl;
    stream << indent(2) << ".rule_t(" << ruleType_ << ")" << Qt::endl;
    
    stream << indent() << ") " << axiXbar_ << "(" << Qt::endl;

    stream << indent(2) << ".clk_i(" << clkPort_ << ")," << Qt::endl;
    stream << indent(2) << ".rst_ni(" << rstPort_ << ")," << Qt::endl;
    stream << indent(2) << ".test_i(1'b0)," << Qt::endl;
    stream << indent(2) << ".slv_ports(" << axiInitBus_ << ")," << Qt::endl;
    stream << indent(2) << ".mst_ports(" << axiTargetBus_ << ")," << Qt::endl;
    stream << indent(2) << ".addr_map_i(" << addrMapXBAR_ << ")," << Qt::endl;
    stream << indent(2) << ".en_default_mst_port_i('0)," << Qt::endl;
    stream << indent(2) << ".default_mst_port_i('0)" << Qt::endl;
    stream << indent() << ");\n" << Qt::endl;
}


void InterconnectRTLWriter::writeAxiAssign(QTextStream& stream, QString busName, int index) {

    /*QStringList debug = component_->getBusInterfaceNames();
    for (QString compPort : debug) {
        stream << " // interfaceName: " << compPort << Qt::endl;
    }*/

    if (!component_) {
        return;
    }

    stream << indent() << "// Interface: " << busName << '\n' << Qt::endl;

    QStringList ports = (config_->BusType.toLower() == "axi4") ? axiPorts_ : axiLitePorts_;

    // Assign input ports first
    for (QSharedPointer<Port> compPort : component_->getPortsMappedInInterface(busName)) {

        if (!compPort) {
            continue;
        }

        for (QString port : ports) {
            if (compPort->name().contains("_" + port) && compPort->getDirection() == DirectionTypes::IN) {
                stream << indent() << "assign " << axiTargetBus_ << "[" << index << "]." << port;
                stream << " = " << compPort->name() << ";" << Qt::endl;
            }
        }
    }

    // Then assign other ports
    for (QSharedPointer<Port> compPort : component_->getPortsMappedInInterface(busName)) {

        if (!compPort) {
            return;
        }

        for (QString port : ports) {
            if (compPort->name().contains("_" + port) && compPort->getDirection() != DirectionTypes::IN) {
                stream << indent() << "assign " << compPort->name();
                stream << " = " << config_->BusType << axiTargetBus_ << "[" << index << "]." << port << ";" << Qt::endl;
            }
        }
    }
}


void InterconnectRTLWriter::writeTargetAssign(QTextStream& stream) {

    if (config_->TargetList.isEmpty()) return;

    for (const TargetStruct& target : config_->TargetList) {
        writeAxiAssign(stream, target.Name, target.Index);
        stream << Qt::endl;
    }
}


void InterconnectRTLWriter::writeInitAssign(QTextStream& stream) {

    if (config_->InitList.isEmpty()) return;

    for (const InitStruct& init : config_->InitList) {
        writeAxiAssign(stream, init.Name, init.Index);
        stream << Qt::endl;
    }
}


void InterconnectRTLWriter::writeObiParams(QTextStream& stream) {
    
    stream << commentWriter("Local parameters") << "\n";

    stream << indent() << "localparam " << obiInitParam_
        << " = " << config_->InitList.size() << ";\n";

    stream << indent() << "localparam " << obiTargetParam_
        << " = " << config_->TargetList.size() << ";\n";

    stream << "\n";
    
    stream << indent() << "localparam " << IdWidthInits_ << " = ";
        stream << config_->IDWidth << " + " << "$clog2(" << obiTargetParam_ << ");\n";

    stream << indent() << "localparam " << addrRulesParam_
        << " = " << obiTargetParam_ << ";\n";

    stream << "\n";

    stream << indent()  << "localparam obi_pkg::obi_optional_cfg_t obi_cfg = obi_pkg::obi_all_optional_config(\n";
    
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


void InterconnectRTLWriter::writeObiInterfaces(QTextStream& stream) {

    stream << commentWriter("Initiator and target interface(s)") << "\n";

    if (!config_->InitList.isEmpty()) {
        stream << indent()  << "OBI_BUS #(\n";
        stream << indent(2) << ".OBI_CFG(obi_cfg)\n";

        if (config_->InitList.size() == 1) {
            stream << indent()  << ") " << obiInitInterface_ << " [(" << obiInitParam_
                << "+1)-1:0](); // NOTE: Extra interface due to single initiator issue\n" << Qt::endl;
        } else {
            stream << indent()  << ") " << obiInitInterface_ << " [" << obiInitParam_ << "-1:0]();\n" << Qt::endl;
        }
    }

    if (!config_->TargetList.isEmpty()) {
        stream << indent()  << "OBI_BUS #(\n";
        stream << indent(2) << ".OBI_CFG(obi_cfg)\n";
        stream << indent()  << ") " << obiTargetInterface_ << " [" << obiTargetParam_ << "-1:0]();\n" << Qt::endl;
    }
}


void InterconnectRTLWriter::writeObiAddrMap(QTextStream& stream) {

    stream << commentWriter("Address mapping") << "\n";

    int targetRegions = 0;
    for (TargetStruct target : config_->TargetList) {
        if (target.AddressRegions.length() > 1) {
            targetRegions += target.AddressRegions.length() - 1;
        }
    }

    stream << indent() << "typedef struct packed {\n";
    stream << indent(2) << "int unsigned idx;\n";
    stream << indent(2) << "logic [ADDR_WIDTH-1:0] start_addr;\n";
    stream << indent(2) << "logic [ADDR_WIDTH-1:0] end_addr;\n";
    stream << indent() << "} " << obiAddrRule_ << " ;\n" << Qt::endl;

    stream << indent() << obiAddrRule_ << " " << addrMapXBAR_ << " [" << addrRulesParam_ << "] = '{\n";

    int regionCounter = 0;

    for (TargetStruct target : config_->TargetList) {
        for (AddressPair addrPair : target.AddressRegions) {
            regionCounter += 1;

            QString startStr = parseAddress(addrPair.Start);
            QString endStr = parseAddress(addrPair.End);

            stream << indent(2)
                << "'{idx: " << IdWidthInits_ << "'('d" << target.Index
                << "), start_addr: ADDR_WIDTH'(" << startStr
                << "), end_addr: ADDR_WIDTH'(" << endStr << ")"
                << ((regionCounter == targetRegions + config_->TargetList.size()) ? "} " : "},")
                << " // Target: " << target.Name << "\n";
        }
    }
    stream << indent() << "};\n" << Qt::endl;
}


void InterconnectRTLWriter::writeObiXbar(QTextStream& stream) {

    stream << commentWriter("Crossbar initialization", "See parameter definitions in 'obi_xbar.sv'") << "\n";

    stream << indent() << "obi_xbar_intf #(\n";
    stream << indent(2) << ".NumSbrPorts       (" << obiInitParam_ << "),\n";
    stream << indent(2) << ".NumMgrPorts       (" << obiTargetParam_ << "),\n";
    stream << indent(2) << ".NumMaxTrans       (1),             // Adjustable\n";
    stream << indent(2) << ".NumAddrRules      (" << addrRulesParam_ << "),\n";
    stream << indent(2) << ".addr_map_rule_t   (" << obiAddrRule_ << "),\n";
    stream << indent(2) << ".UseIdForRouting   (0)              // Adjustable\n";
    stream << indent() << ") i_obi_xbar (\n";
    stream << indent(2) << ".clk_i             (clk_i),\n";
    stream << indent(2) << ".rst_ni            (rst_ni),\n";
    stream << indent(2) << ".testmode_i        (1'b0),          // Adjustable\n";
    stream << indent(2) << ".sbr_ports         (" << obiInitInterface_ << "),\n";
    stream << indent(2) << ".mgr_ports         (" << obiTargetInterface_ << "),\n";
    stream << indent(2) << ".addr_map_i        (" << addrMapXBAR_ << "),\n";
    stream << indent(2) << ".en_default_idx_i  ('0),            // Adjustable\n";
    stream << indent(2) << ".default_idx_i     ('0)             // Adjustable\n";
    stream << indent() << ");\n" << Qt::endl;
}


QString InterconnectRTLWriter::indent(int n) {

    return QString(n * 4, ' ');
}


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
        return QString::number(config_->AddressWidth) + "'h" + parsed.toUpper();
    }

    else if (parsed.startsWith("0x", Qt::CaseInsensitive)) {

        parsed = parsed.mid(2);
        return QString::number(config_->AddressWidth) + "'h" + parsed.toUpper();
    }
    
    else if (parsed.contains("'b", Qt::CaseInsensitive)) {

        int index = parsed.indexOf("'b", Qt::CaseInsensitive);
        parsed = parsed.mid(index + 2).trimmed();

        bool ok;
        uint64_t temp = parsed.toULongLong(&ok, 2);
        
        if (ok) {
            parsed = QString::number(config_->AddressWidth) + "'h" + QString::number(temp, 16).toUpper();
            return parsed;
        } else {
            return original;
        }
    }

    else if (parsed.startsWith("0b", Qt::CaseInsensitive)) {

        parsed = parsed.mid(2);

        bool ok;
        uint64_t temp = parsed.toULongLong(&ok, 2);
        
        if (ok) {
            parsed = QString::number(config_->AddressWidth) + "'h" + QString::number(temp, 16).toUpper();
            return parsed;
        } else {
            return original;
        }
    }

    bool isDecimal;
    uint64_t dec = parsed.toULongLong(&isDecimal, 10);
    if (isDecimal) {
        parsed = QString::number(config_->AddressWidth) + "'h" + QString::number(dec, 16).toUpper();
        return parsed;
    }
    
    return original;
}


QString InterconnectRTLWriter::commentWriter(QString title, QString subtitle) {

    QString comment = "\n//---------------------------------------------------\n"
                    "// " + title + "\n";

    if (!subtitle.isEmpty()) {
        comment += "// \n// " + indent() + "- " + subtitle + "\n";
    }

    comment += "//---------------------------------------------------\n";
    return comment;
}