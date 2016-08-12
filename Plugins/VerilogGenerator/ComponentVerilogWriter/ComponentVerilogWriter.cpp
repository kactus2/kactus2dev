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
#include "editors/ComponentEditor/common/ExpressionParser.h"
#include "IPXACTmodels/Component/View.h"
#include "IPXACTmodels/Component/ComponentInstantiation.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<GenerationComponent> component,
	QSharedPointer<ExpressionFormatter> expressionFormatter) :
component_(component),
childWriters_(),
formatter_(expressionFormatter)
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

    writeModuleDeclaration(outputStream);

    writeInternalWiresAndComponentInstances(outputStream);

	if ( implementation_ )
	{
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
QString ComponentVerilogWriter::indentation() const
{
    return "    ";
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    QString previousInterfaceName = "";

    outputStream << "(";

    foreach(QString portName, component_->sortedPortNames)
	{    
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            component_->component->getInterfacesUsedByPort(portName);

        if (busInterfaces->size() == 1)
		{
            writeInterfaceIntroduction(busInterfaces->first()->name(), previousInterfaceName, outputStream);
		}
        else if (!busInterfaces->isEmpty())
        {
            writeInterfaceIntroduction("several", previousInterfaceName, outputStream);
        }
		else
		{
			writeInterfaceIntroduction("none", previousInterfaceName, outputStream);
		}

        bool lastPortToWrite = portName == component_->sortedPortNames.last();
        writePort(outputStream, component_->component->getPort(portName), lastPortToWrite);
    }
    
    outputStream << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInterfaceIntroduction()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
    QTextStream& outputStream) const
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

            QSharedPointer<const BusInterface> busInterface = component_->component->getBusInterface(interfaceName);
            CommentWriter descriptionWriter(busInterface->description());
            descriptionWriter.setIndent(4);
            descriptionWriter.write(outputStream);
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePort()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePort(QTextStream& outputStream, QSharedPointer<Port> port, bool isLast) const
{
    outputStream << indentation();

    PortVerilogWriter writer(port, formatter_);
    writer.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(port->description());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (port->description().isEmpty())
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

    outputStream << indentation() << "localparam memory_size = " << component_->totalRange << ";" << endl;
    outputStream << indentation() << "localparam AUB = " << component_->aub << ";" << endl;

    foreach(QSharedPointer<GenerationRemap> grm, component_->remaps)
    {
        foreach(QSharedPointer<GenerationAddressBlock> gab, grm->blocks)
        {
            outputStream << endl;

            outputStream << indentation() << "localparam " << grm->name << "_"  << gab->name << "_base = " << gab->baseAddress_ << ";" << endl;

            foreach(QSharedPointer<GenerationRegister> gr, gab->registers)
            {
                outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_dim = " << gr->dimension_ << ";" << endl;
                outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_offset = " << gr->offset_ << ";" << endl;
                outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_width = " << gr->size_ << ";" << endl;
                outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_AU_WIDTTH = $ceil($ceil(" << gr->size_ << ")/AUB);" << endl;

                outputStream << endl;

                foreach(QSharedPointer<GenerationField> gf, gr->fields)
                {
                    outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_" << gf->name << "_width = " << gf->bitWidth_ << ";" << endl;
                    outputStream << indentation() << indentation() << "localparam " << grm->name << "_" << gab->name << "_" << gr->name << "_" << gf->name << "_offset = " << gf->bitOffset_ << ";" << endl;
                }

                outputStream << endl;
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
