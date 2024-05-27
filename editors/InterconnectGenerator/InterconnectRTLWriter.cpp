#include "InterconnectRTLWriter.h"

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/PluginManager.h>
#include <KactusAPI/include/CLIGenerator.h>
#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/PluginUtilityAdapter.h>
#include <KactusAPI/include/VersionHelper.h>

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
    messager_->showMessage(QString("Opening component %1").arg(verilogDirectory));
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
        verilogRTL << "  localparam AXI_TARGETS = " << config_->TargetList.size() << ";" << Qt::endl;
        verilogRTL << "  localparam AXI_INITIATORS = " << config_->InitList.size() << ";" << Qt::endl;
        verilogRTL << "endmodule" << Qt::endl;
    }
    verilogFile.close();
}

void InterconnectRTLWriter::writeBus(QTextStream &stream, QString type){
    if(config_->BusType == "AXI4"){
        stream << "AXI_BUS #(" << Qt::endl;
    }else if(config_->BusType == "AXI4LITE"){
        stream << "AXI_LITE #(" << Qt::endl;
    }
    stream << ".AXI_ADDR_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << ".AXI_DATA_WIDTH(" << config_->AddressWidth << ")," << Qt::endl;
    stream << Qt::endl;
}
