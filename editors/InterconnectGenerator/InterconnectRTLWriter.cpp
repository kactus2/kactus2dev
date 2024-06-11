#include "InterconnectRTLWriter.h"

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>

InterconnectRTLWriter::InterconnectRTLWriter(QSharedPointer<Component> component, LibraryInterface* library,
                                             MessageMediator* messager, QString directory, ConfigJsonParser::ConfigStruct* config)
{
    component_ = component;
    library_ = library;
    messager_ = messager;
    directory_ = directory;
    config_ = config;
}

void InterconnectRTLWriter::generateRTL()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> designConfig;
    for (IPlugin* plugin : PluginManager::getInstance().getActivePlugins()){
        CLIGenerator* runnable = dynamic_cast<CLIGenerator*>(plugin);
        if (runnable != 0)
        {
            if (runnable->getOutputFormat().toLower() == "verilog"){
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

    if(verilogFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        while(!verilogRTL.atEnd()){
            QString line = verilogRTL.readLine();
            verilogTxt.append(line+ "\n");
            if(line.startsWith(");")){
                break;
            }
        }
    }
    verilogFile.close();

    if(verilogFile.open(QIODevice::WriteOnly)){
        verilogRTL << verilogTxt << Qt::endl;
    }
    verilogFile.close();

    if(verilogFile.open(QIODevice::WriteOnly | QIODevice::Append)){

        verilogRTL << "  localparam " << axiTargetParam_ << " = " << config_->TargetList.size() << ";" << Qt::endl;
        verilogRTL << "  localparam " << axiInitParam_ << " = " << config_->InitList.size() << ";" << Qt::endl;
        verilogRTL << "  localparam " << axiIdWidthInits_ << " = ";
        verilogRTL << config_->IDWidth << " + " << "$clog2(" << axiTargetParam_ << ");\n" << Qt::endl;

        writeBus(verilogRTL, "target");
        writeBus(verilogRTL, "init");
        writeAddrMap(verilogRTL);
        writeXbarCfg(verilogRTL);
        writeXbar(verilogRTL);
        verilogRTL << "endmodule" << Qt::endl;
    }
    verilogFile.close();
    messager_->showMessage(QString("Closed file %1").arg(verilogDirectory));
}

void InterconnectRTLWriter::writeBus(QTextStream &stream, QString type)
{
    if(config_->BusType == "AXI4"){
        stream << "  AXI_BUS #(" << Qt::endl;
    }else if(config_->BusType == "AXI4LITE"){
        stream << "  AXI_LITE #(" << Qt::endl;
    }

    if(config_->BusType == "AXI4") {
        stream << "    .AXI_ID_WIDTH(" << axiIdWidthInits_ << ")," << Qt::endl;
        stream << "    .AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;

    }
    stream << "    .AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << "    .AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;
    stream << "  ) " << config_->BusType.toLower() << "_" << type.toLower();
    if(type=="target"){
        stream << " [" << axiTargetParam_ << "-1:0]();\n" << Qt::endl;
    } else {
        stream << " [" << axiInitParam_ << "-1:0]();\n" << Qt::endl;
    }
}

void InterconnectRTLWriter::writeAddrMap(QTextStream &stream)
{
    int targetRegions = 0;
    for(ConfigJsonParser::TargetStruct target : config_->TargetList){
        if(target.AddressRegions.length() > 1){
            targetRegions += target.AddressRegions.length() - 1;
        }
    }

    stream << "  localparam " << addrRulesParam_ << " = " << axiTargetParam_ << " + "<< targetRegions << ";\n" << Qt::endl;
    stream << "  typedef axi_pkg::xbar_rule_" << config_->AddressWidth << "_t " << ruleType_ << ";\n"<< Qt::endl;
    stream << "  " << ruleType_ << " [" << addrRulesParam_ << "-1:0] " << addrMapXBAR_ << ";\n" << Qt::endl;
    stream << "  assign " << addrMapXBAR_ << " = " << Qt::endl;

    stream << "    '{" << Qt::endl;

    int regionCounter = 0;

    for(ConfigJsonParser::TargetStruct target : config_->TargetList){
        for(ConfigJsonParser::AddressPair addrPair : target.AddressRegions){
            regionCounter += 1;
            int start = std::stoul(addrPair.Start.toStdString(), nullptr, 16);
            int end = std::stoul(addrPair.End.toStdString(), nullptr, 16);
            stream << "      '{idx: 32'd" << target.Index;
            stream << ", start_addr: " << config_->AddressWidth << "'h" << QString::number( start, 16 );
            stream << ", end_addr: " << config_->AddressWidth << "'h" << QString::number( end, 16 );
            if(regionCounter == targetRegions + config_->TargetList.size()){
                stream << "}";
            } else {
                stream << "},";
            }
            stream << " //" << target.Name << Qt::endl;
        }
    }
    stream << "    };\n" << Qt::endl;
}

void InterconnectRTLWriter::writeXbarCfg(QTextStream &stream)
{
    stream << "  localparam axi_pkg::xbar_cfg_t " << axiCfg_ << " = '{" << Qt::endl;
    stream << "    NoSlvPorts:         " << axiTargetParam_ << "," << Qt::endl;
    stream << "    NoMstPorts:         " << axiInitParam_ << "," << Qt::endl;
    stream << "    MaxMstTrans:        1," << Qt::endl;
    stream << "    MaxSlvTrans:        1," << Qt::endl;
    stream << "    FallThrough:        1'b0," << Qt::endl;
    stream << "    LatencyMode:        axi_pkg::CUT_ALL_AX," << Qt::endl;
    stream << "    AxiIdWidthSlvPorts: " << config_->IDWidth << "," << Qt::endl;
    stream << "    AxiIdUsedSlvPorts:  0," << Qt::endl;
    stream << "    UniqueIds:          1'b1," << Qt::endl;
    stream << "    AxiAddrWidth:       " << config_->AddressWidth << "," << Qt::endl;
    stream << "    AxiDataWidth:       " << config_->TargetList[0].DataWidth << "," << Qt::endl;
    stream << "    NoAddrRules:        " << addrRulesParam_ << Qt::endl;
    stream << "  };\n" << Qt::endl;
}

void InterconnectRTLWriter::writeXbar(QTextStream &stream) {
    if(config_->BusType == "AXI4") {
        axiXbar_ = "i_axi_xbar";
        stream << "  axi_xbar_intf #(" << Qt::endl;
        stream << "    .AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;
    } else if(config_->BusType == "AXI4LITE"){
        axiXbar_ = "i_axi_lite_xbar";
        stream << "  axi_lite_xbar_intf #(" << Qt::endl;
    }
    stream << "    .Cfg(" << axiCfg_ << ")," << Qt::endl;
    stream << "    .rule_t(" << ruleType_ << ")" << Qt::endl;

    stream << "  ) " << axiXbar_ << "(" << Qt::endl;
    stream << "    .clk_i(" << Qt::endl;
    stream << "    .rst_ni(" << Qt::endl;
    stream << "    .test_i(1'b0)" << Qt::endl;
    stream << "    .slv_ports(" << Qt::endl;
    stream << "    .mst_ports(" << Qt::endl;
    stream << "    .addr_map_i(" << Qt::endl;
    stream << "    .en_default_mst_port_i('0)" << Qt::endl;
    stream << "    .default_mst_port_i('0)" << Qt::endl;
}
