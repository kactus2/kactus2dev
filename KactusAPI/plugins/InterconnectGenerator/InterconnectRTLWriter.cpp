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
            writeAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL, usedInterfaceStr);

        } else if (usedInterfaceStr == "axi4lite") {

            writeAXI4(verilogRTL);
            writeAddrMap(verilogRTL);
            writeAxiXbarCfg(verilogRTL);
            writeAxiXbar(verilogRTL, usedInterfaceStr);

        } else {

            messager_->showMessage(QString("*** No RTL generated: Interface type '%1' is not supported.").arg(config_->BusType));

            verilogRTL << Qt::endl;
            verilogRTL << "// Could not implement the interconnect: unknown interface type" << Qt::endl;

            return;
        }

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

    stream << indent()  << "AXI_BUS #(" << Qt::endl;
    stream << indent(2) << ".AXI_ID_WIDTH(" << axiIdWidthInits_ << ")," << Qt::endl;
    stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;    

    axiTargetBus_ = config_->BusType.toLower() + axiTargetBus_;
    stream << indent() << ") " << axiTargetBus_;
    stream << "[" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;


    stream << indent()  << "AXI_BUS #(" << Qt::endl;
    stream << indent(2) << ".AXI_ID_WIDTH(" << axiIdWidthInits_ << ")," << Qt::endl;
    stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

    axiInitBus_ = config_->BusType.toLower() + axiInitBus_;
    stream << indent() << ") " << axiInitBus_;
    stream << "[" << axiInitParam_ << "-1:0]();\n" << Qt::endl;

}


void InterconnectRTLWriter::writeAXI4LITE(QTextStream& stream) {

    writeAxiParams(stream);

    stream << indent()  << "AXI_LITE #("      << Qt::endl;
    stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

    axiTargetBus_ = config_->BusType.toLower() + axiTargetBus_;
    stream << "    ) " << axiTargetBus_;
    stream << " [" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;


    stream << indent()  << "AXI_LITE #("      << Qt::endl;
    stream << indent(2) << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << indent(2) << ".AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;

    axiInitBus_ = config_->BusType.toLower() + axiInitBus_;
    stream << indent() << ") " << axiInitBus_;
    stream << " [" << axiInitParam_ << "-1:0]();\n" << Qt::endl;

}


void InterconnectRTLWriter::writeAxiParams(QTextStream& stream) {
    
    stream << indent() << "localparam " << axiTargetParam_
    << " = " << config_->TargetList.size() << ";" << Qt::endl;

    stream << indent() << "localparam " << axiInitParam_ << " = "
    << config_->InitList.size() << ";" << Qt::endl;

    stream << indent() << "localparam " << axiIdWidthInits_ << " = ";
    stream << config_->IDWidth << " + " << "$clog2(" << axiTargetParam_ << ");" << Qt::endl;
    stream << Qt::endl;
}


void InterconnectRTLWriter::writeAddrMap(QTextStream& stream)
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
            int start = std::stoul(addrPair.Start.toStdString(), nullptr, 16);
            int end = std::stoul(addrPair.End.toStdString(), nullptr, 16);
            stream << indent(3) << "'{idx: 32'd" << target.Index;
            stream << ", start_addr: " << config_->AddressWidth << "'h" << QString::number(start, 16);
            stream << ", end_addr: " << config_->AddressWidth << "'h" << QString::number(end, 16);
            if (regionCounter == targetRegions + config_->TargetList.size()) {
                stream << "}";
            }
            else {
                stream << "},";
            }
            stream << " //" << target.Name << Qt::endl;
        }
    }
    stream << indent(2) << "};\n" << Qt::endl;
}


void InterconnectRTLWriter::writeAxiXbarCfg(QTextStream& stream)
{
    stream << indent()  << "localparam axi_pkg::xbar_cfg_t " << axiCfg_ << " = '{" << Qt::endl;
    stream << indent(2) << "NoSlvPorts:         " << axiTargetParam_ << "," << Qt::endl;
    stream << indent(2) << "NoMstPorts:         " << axiInitParam_ << "," << Qt::endl;
    stream << indent(2) << "MaxMstTrans:        1," << Qt::endl;
    stream << indent(2) << "MaxSlvTrans:        1," << Qt::endl;
    stream << indent(2) << "FallThrough:        1'b0," << Qt::endl;
    stream << indent(2) << "PipelineStages:     1'b0," << Qt::endl; // check
    stream << indent(2) << "LatencyMode:        axi_pkg::CUT_ALL_AX," << Qt::endl;
    stream << indent(2) << "AxiIdWidthSlvPorts: " << config_->IDWidth << "," << Qt::endl;
    stream << indent(2) << "AxiIdUsedSlvPorts:  0," << Qt::endl;
    stream << indent(2) << "UniqueIds:          1'b1," << Qt::endl;
    stream << indent(2) << "AxiAddrWidth:       " << config_->AddressWidth << "," << Qt::endl;
    stream << indent(2) << "AxiDataWidth:       " << config_->TargetList[0].DataWidth << "," << Qt::endl;
    stream << indent(2) << "NoAddrRules:        " << addrRulesParam_ << Qt::endl;
    stream << indent() << "};" << Qt::endl;
    stream << '\n';
}


void InterconnectRTLWriter::writeAxiXbar(QTextStream& stream, QString interface) {

    if (interface == "axi4") {

        axiXbar_ = "i_axi_xbar";
        stream << indent() << "axi_xbar_intf #(" << Qt::endl;
        stream << indent(2) << ".AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;

    }
    else if (interface == "axi4lite") {

        axiXbar_ = "i_axi_lite_xbar";
        stream << indent() << "axi_lite_xbar_intf #(" << Qt::endl;

    }
    
    stream << indent(2) << ".Cfg(" << axiCfg_ << ")," << Qt::endl;
    stream << indent(2) << ".rule_t(" << ruleType_ << ")" << Qt::endl;
    
    stream << indent() << ") " << axiXbar_ << "(" << Qt::endl;

    stream << indent(2) << ".clk_i(" << clkPort_ << ")," << Qt::endl;       // fix
    stream << indent(2) << ".rst_ni(" << rstPort_ << ")," << Qt::endl;     // fix
    stream << indent(2) << ".test_i(1'b0)," << Qt::endl;
    stream << indent(2) << ".slv_ports(" << axiInitBus_ << ")," << Qt::endl;
    stream << indent(2) << ".mst_ports(" << axiTargetBus_ << ")," << Qt::endl;
    stream << indent(2) << ".addr_map_i(" << addrMapXBAR_ << ")," << Qt::endl;
    stream << indent(2) << ".en_default_mst_port_i('0)," << Qt::endl;
    stream << indent(2) << ".default_mst_port_i('0)" << Qt::endl;
    stream << indent() << ");\n" << Qt::endl;
}


void InterconnectRTLWriter::writeAxiAssign(QTextStream& stream, QString busName, int index) {
    QStringList ports;
    for (QSharedPointer<Port> compPort : component_->getPortsMappedInInterface(busName)) {
        if (config_->BusType.toLower() == "axi4") {
            ports = axiPorts_;
        }
        else {
            ports = axiLitePorts_;
        }
        for (QString port : ports) {
            if (compPort->name().contains("_" + port)) {
                if (compPort->getDirection() == DirectionTypes::IN) {
                    stream << indent() << "assign " << axiTargetBus_ << "[" << index << "]." << port;
                    stream << " = " << compPort->name() << Qt::endl;
                }
                else {
                    stream << indent() << "assign " << compPort->name();
                    stream << " = " << axiTargetBus_ << "[" << index << "]." << port << Qt::endl;
                }
            }
        }
    }
}


void InterconnectRTLWriter::writeTargetAssign(QTextStream& stream) {
    for (TargetStruct target : config_->TargetList) {
        QString busName = target.Name + "_" + config_->BusType;
        writeAxiAssign(stream, busName, target.Index);
        stream << Qt::endl;
    }
}


void InterconnectRTLWriter::writeInitAssign(QTextStream& stream) {
    for (InitStruct init : config_->InitList) {
        QString busName = init.Name + "_" + config_->BusType;
        writeAxiAssign(stream, busName, init.Index);
        stream << Qt::endl;
    }
}


QString InterconnectRTLWriter::indent(int n) {

    return QString(n * 4, ' ');
}