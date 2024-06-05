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
        verilogRTL << "  localparam " << axiInitParam_ << " = " << config_->InitList.size() << ";\n" << Qt::endl;
        writeBus(verilogRTL, "target");
        writeBus(verilogRTL, "init");
        writeXbar(verilogRTL);
        verilogRTL << "endmodule" << Qt::endl;
    }
    verilogFile.close();
    messager_->showMessage(QString("Closed file %1").arg(verilogDirectory));
}

void InterconnectRTLWriter::writeBus(QTextStream &stream, QString type)
{
    QString param = axiTargetParam_;
    if(type=="init"){
        param = axiInitParam_;
    }

    if(config_->BusType == "AXI4"){
        stream << "  AXI_BUS #(" << Qt::endl;
    }else if(config_->BusType == "AXI4LITE"){
        stream << "  AXI_LITE #(" << Qt::endl;
    }

    if(config_->BusType == "AXI4") {
        stream << "    .AXI_ID_WIDTH(" << config_->IDWidth<< ")," << Qt::endl;
        stream << "    .AXI_USER_WIDTH(" << config_->UserWidth << ")," << Qt::endl;

    }
    stream << "    .AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << "    .AXI_DATA_WIDTH(" << config_->TargetList[0].DataWidth << ")" << Qt::endl;
    stream << "  ) " << config_->BusType.toLower() << "_" << type.toLower();
    stream << " [" << param << "-1:0]();\n" << Qt::endl;
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
                stream << "}" << Qt::endl;
            } else {
                stream << "}," << Qt::endl;
            }
        }
    }
    stream << "    };\n" << Qt::endl;
}

void InterconnectRTLWriter::writeXbar(QTextStream &stream)
{
    stream << "  localparam axi_pkg::xbar_cfg_t " << axiCfg_ << " = '{" << Qt::endl;
    stream << "    NoSlvPorts:         " << axiTargetParam_ << "," << Qt::endl;
    stream << "    NoMstPorts:         " << axiInitParam_ << "," << Qt::endl;
    stream << "    MaxMstTrans:        1," << Qt::endl;
    stream << "    MaxSlvTrans:        1," << Qt::endl;
    stream << "    FallThrough:        " << Qt::endl;
    stream << "    LatencyMode:        " << Qt::endl;
    stream << "    PipelineStages:     " << Qt::endl;
    stream << "    AxiIdWidthSlvPorts: " << Qt::endl;
    stream << "    AxiIdUsedSlvPorts:  " << Qt::endl;
    stream << "    UniqueIds:          " << Qt::endl;
    stream << "    AxiAddrWidth:       " << Qt::endl;
    stream << "    AxiDataWidth:       " << Qt::endl;
    stream << "    NoAddrRules:        " << addrRulesParam_ << "," << Qt::endl;
    stream << "  };" << Qt::endl;

    //MaxMstTrans:        1,
    //MaxSlvTrans:        1,
    //FallThrough:        1'b0,
    //LatencyMode:        axi_pkg::CUT_ALL_AX,
    //PipelineStages:     0,
    //AxiIdWidthSlvPorts: 11,
    //AxiIdUsedSlvPorts:  0,
    //UniqueIds:          1,
    //AxiAddrWidth:       AXI_AW,
    //AxiDataWidth:       AXI_DW,
    //NoAddrRules:        AXI4LITE_TARGETS
}
