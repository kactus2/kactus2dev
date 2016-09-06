//-----------------------------------------------------------------------------
// File: ComponentVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.08.2014
//
// Description:
// Class for writing a component as a Verilog module.
//-----------------------------------------------------------------------------

#include "ComponentVerilogWriter.h"

#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QSharedPointer>
#include "IPXACTmodels/Component/View.h"
#include "IPXACTmodels/Component/ComponentInstantiation.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<GenerationComponent> component,
	QSharedPointer<ExpressionFormatter> expressionFormatter) :
component_(component),
formatter_(expressionFormatter),
useInterfaces_(true),
childWriters_()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::~ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::~ComponentVerilogWriter()
{
    childWriters_.clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::write
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::write(QTextStream& outputStream) const
{
    if (nothingToWrite())
    {
        return;
    }

    QStringList fileNames;

    foreach(QSharedPointer<GenerationInterface> gif, component_->interfaces)
    {
        fileNames.append(gif->fileName);
    }

    fileNames.removeDuplicates();

    foreach(QString name, fileNames)
    {
        outputStream << "`include \"" << name << "\"" <<  endl;
    }

    writeModuleDeclaration(outputStream);

    writeInternalWiresAndComponentInstances(outputStream);

	if ( implementation_ )
    {
        // Implementing -> may need remap states.
        foreach (QSharedPointer<GenerationRemapState> grms, component_->remapStates)
        {
            QString condition;
            
            QSharedPointer<QPair<QSharedPointer<Port>, QString> > parsedPort;
            foreach (parsedPort, grms->ports)
            {
                condition += "(" + parsedPort->first->name() + " == " + parsedPort->second + ")";

                if (parsedPort != grms->ports.last())
                {
                     condition += " && ";
                }
            }

            outputStream << indentation() << "`define " << grms->stateName << " " << condition << endl;
        }

        if (component_->remapStates.count() > 0)
        {
            outputStream << endl;
        }

        // Implementing -> may need registers.
        writeRegisters(outputStream);

		// If an implementation exists, there must be a warning about overwriting as well.
		outputStream << "// " << VerilogSyntax::TAG_OVERRIDE << endl;

		implementation_->write(outputStream);
	}
		
	writeModuleEnd(outputStream);

	if ( postModule_ )
	{
		postModule_->write(outputStream);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setImplementation
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setImplementation( QSharedPointer<TextBodyWriter> implementation )
{
	implementation_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setPostModule
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setPostModule( QSharedPointer<TextBodyWriter> postModule )
{
	postModule_ = postModule;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::nothingToWrite
//-----------------------------------------------------------------------------
bool ComponentVerilogWriter::nothingToWrite() const
{
    return component_.isNull();
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleDeclaration()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleDeclaration(QTextStream& outputStream) const
{
    outputStream << "module " << component_->component->getVlnv().getName();
    
    writeParameterDeclarations(outputStream);

    writePortDeclarations(outputStream);     
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
{
	if (component_->parameters.isEmpty())
    {
        return;
    }

    outputStream << " #(" << endl;

    foreach(QSharedPointer<Parameter> parameter, component_->parameters)
    {
        bool isLastParameter = parameter == component_->parameters.last();
        writeParameter(outputStream, parameter, isLastParameter);
    }

    outputStream << ") ";
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameter()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameter(QTextStream& outputStream, QSharedPointer<Parameter> parameter,
    bool isLast) const
{
    outputStream << indentation();
    ModuleParameterVerilogWriter parameterWriter(parameter, formatter_);
    parameterWriter.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(parameter->description());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (parameter->description().isEmpty())
    {
        outputStream << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::indentation()
//-----------------------------------------------------------------------------
QString ComponentVerilogWriter::indentation(int depth /*= 1*/) const
{
    QString indent = "    ";

    return indent.repeated(depth);
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    QString previousInterfaceName = "";

    outputStream << "(";

    foreach(QSharedPointer<GenerationInterface> gif, component_->interfaces)
    {
        outputStream  <<  endl << gif->typeName.replace('.',"_") << "." << gif->mode << " " << gif->name << ",";
    }

    foreach(QSharedPointer<GenerationPort> port, component_->ports)
    {   
        QString interfaceName;

        if (port->interfaces.count() < 1 )
        {
            interfaceName = "none";
        }
        else if (useInterfaces_)
        {
            continue;
        }
        else
        {
            if (port->interfaces.count() == 1)
            {
                interfaceName  = port->interfaces.first()->name;
            }
            else
            {
                interfaceName = "several";
            }
        }

        writeInterfaceIntroduction(interfaceName, port->description, previousInterfaceName, outputStream);

        bool lastPortToWrite = port == component_->ports.last();
        writePort(outputStream, port, lastPortToWrite);
    }
    
    outputStream << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInterfaceIntroduction()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInterfaceIntroduction(QString const& interfaceName, QString const& interfaceDescription,
    QString& previousInterfaceName, QTextStream& outputStream) const
{
    if (previousInterfaceName.compare(interfaceName) != 0)
    {
        outputStream << endl;

        if (interfaceName == "none")
        {
            outputStream << indentation() << "// These ports are not in any interface" << endl;                
        }
        else if (interfaceName == "several")
        {
            outputStream << indentation() << "// There ports are contained in many interfaces" << endl;       
        }
        else
        {
            outputStream << indentation() << "// Interface: " << interfaceName << endl;

            CommentWriter descriptionWriter(interfaceDescription);
            descriptionWriter.setIndent(4);
            descriptionWriter.write(outputStream);
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePort()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePort(QTextStream& outputStream, QSharedPointer<GenerationPort> port, bool isLast) const
{
    outputStream << indentation();

    PortVerilogWriter writer(port);
    writer.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(port->description);
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (port->description.isEmpty())
    {
        outputStream << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInternalWiresAndComponentInstances()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInternalWiresAndComponentInstances(QTextStream& outputStream) const
{
    outputStream << endl;

    WriterGroup::write(outputStream);
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeRegisters()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeRegisters(QTextStream& outputStream) const
{
    if ( component_->remaps.size() < 1 )
    {
        return;
    }

    outputStream << indentation() << "localparam MEMORY_SIZE = " << component_->totalRange << ";" << endl;
    outputStream << indentation() << "localparam AUB = " << component_->aub << ";" << endl;
    outputStream << indentation() << "reg [0:AUB-1] dat [0:MEMORY_SIZE-1];" << endl;

    outputStream << indentation() << "genvar gen_iterator1;" << endl;
    outputStream << indentation() << "genvar gen_iterator2;" << endl;

    foreach(QSharedPointer<GenerationRemap> grm, component_->remaps)
    {
        foreach(QSharedPointer<GenerationAddressBlock> gab, grm->blocks)
        {
            QString abName = grm->name + "_" + gab->name;

            outputStream << endl;

            outputStream << indentation() << "localparam " << abName << "_BASE = " << gab->baseAddress_ << ";" << endl;

            foreach(QSharedPointer<GenerationRegister> gr, gab->registers)
            {
               QString regName = abName + "_" + gr->name;

                outputStream << indentation(2) << "localparam " << regName << "_DIM = " << gr->dimension_ << ";" << endl;
                outputStream << indentation(2) << "localparam " << regName << "_OFFSET = " << gr->offset_ << ";" << endl;
                outputStream << indentation(2) << "localparam " << regName << "_WIDTH = " << gr->size_ << ";" << endl;
                outputStream << indentation(2) << "localparam " << regName << "_AU_WIDTH = $ceil($ceil(" << gr->size_ << ")/AUB);" << endl;
                outputStream << indentation(2) << "wire [0:" << regName << "_WIDTH-1] " << regName << " [0:" << regName << "_DIM-1];" << endl;

                outputStream << endl;

                outputStream << indentation(2) << "generate" << endl;
                outputStream << indentation(2) << "for (gen_iterator1= 0; gen_iterator1 < " << regName << "_DIM; gen_iterator1 = gen_iterator1 + 1)" << endl;
                outputStream << indentation(2) << "begin" << endl;
                outputStream << indentation(3) << "for (gen_iterator2 = 0; gen_iterator2 < " << regName << "_AU_WIDTH; gen_iterator2 = gen_iterator2 + 1)" << endl;
                outputStream << indentation(3) << "begin" << endl;
                outputStream << indentation(4) << "assign " << regName << "[gen_iterator1][(gen_iterator2*AUB)+:AUB] = dat[gen_iterator1*" << regName << "_AU_WIDTH+gen_iterator2];" << endl;
                outputStream << indentation(3) << "end" << endl;
                outputStream << indentation(2) << "end" << endl;
                outputStream << indentation(2) << "endgenerate" << endl;

                outputStream << endl;

                foreach(QSharedPointer<GenerationField> gf, gr->fields)
                {
                    QString fieldName = regName + "_" + gf->name;
                    outputStream << indentation(2) << "localparam " << fieldName << "_WIDTH = " << gf->bitWidth_ << ";" << endl;
                    outputStream << indentation(2) << "localparam " << fieldName << "_OFFSET = " << gf->bitOffset_ << ";" << endl;;

                    outputStream << endl;
                }
            }
        }
    }

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleEnd()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}
