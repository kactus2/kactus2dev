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
#include <Plugins/VerilogGenerator/VerilogPortWriter/VerilogPortWriter.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<MetaComponent> component,
    bool useInterfaces) :
component_(component),
useInterfaces_(useInterfaces),
childWriters_(),
sorter_(new InterfaceDirectionNameSorter)
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

	if (implementation_)
    {
        // Implementing -> may need remap states.
        writeRemapSates(outputStream);

		// If an implementation exists, there must be a warning about overwriting as well.
		outputStream << "// " << VerilogSyntax::TAG_OVERRIDE << endl;

        implementation_->write(outputStream);
	}
		
	writeModuleEnd(outputStream);

	if (postModule_)
	{
		postModule_->write(outputStream);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setImplementation
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setImplementation(QSharedPointer<TextBodyWriter> implementation)
{
	implementation_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setPostModule
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setPostModule(QSharedPointer<TextBodyWriter> postModule)
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
    outputStream << "module " << VerilogSyntax::legalizeName(component_->getModuleName());
    
    writeParameterDeclarations(outputStream);

    writePortDeclarations(outputStream);     
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
{
	if (component_->getMetaParameters()->isEmpty())
    {
        return;
    }

    outputStream << " #(" << endl;

    foreach(QSharedPointer<Parameter> parameter, *component_->getMetaParameters())
    {
        bool isLastParameter = parameter == component_->getMetaParameters()->last();
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
    ModuleParameterVerilogWriter parameterWriter(parameter);
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

    // Pick the ports in sorted order.
    QList<QSharedPointer<Port> > ports = sorter_->sortedPorts(component_->getComponent());

    foreach(QSharedPointer<Port> cPort, ports)
    {
        if (cPort->getDirection() == DirectionTypes::DIRECTION_PHANTOM || cPort->getTransactional())
        {
            ports.removeOne(cPort);
        }
    }

    foreach(QSharedPointer<Port> cPort, ports)
    {
        QSharedPointer<MetaPort> mPort = component_->getPorts()->value(cPort->name());

        if (!mPort)
        {
            // TODO: error
            continue;
        }

        QString interfaceName;
        QString interfaceDescription;
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            component_->getComponent()->getInterfacesUsedByPort(cPort->name());

        if (busInterfaces->count() < 1 )
        {
            interfaceName = "none";
        }
        else if (useInterfaces_)
        {
            continue;
        }
        else
        {
            if (busInterfaces->count() == 1)
            {
                interfaceName  = busInterfaces->first()->name();
                interfaceDescription = busInterfaces->first()->description();
            }
            else
            {
                interfaceName = "several";
            }
        }

        writeInterfaceIntroduction(interfaceName, interfaceDescription, previousInterfaceName, outputStream);
        bool lastPortToWrite = cPort == ports.last();
        writePort(outputStream, mPort, lastPortToWrite);
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

            if (!interfaceDescription.isEmpty())
            {
                CommentWriter descriptionWriter(interfaceDescription);
                descriptionWriter.setIndent(4);
                descriptionWriter.write(outputStream);
            }
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePort()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePort(QTextStream& outputStream, QSharedPointer<MetaPort> port, bool isLast) const
{
    outputStream << indentation();

    VerilogPortWriter writer(port);
    writer.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(port->port_->description());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (port->port_->description().isEmpty())
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
// Function: ComponentVerilogWriter:writeRemapSates()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeRemapSates(QTextStream& outputStream) const
{
    if (component_->getRemapStates()->isEmpty())
    {
       return;
    }

    outputStream << indentation() << "// Remap states:" << endl;

    foreach (QSharedPointer<FormattedRemapState> grms, *component_->getRemapStates())
    {
        QString condition;

        QSharedPointer<QPair<QSharedPointer<Port>, QString> > parsedPort;
        foreach (parsedPort, grms->ports_)
        {
            condition += "(" + parsedPort->first->name() + " == " + parsedPort->second + ")";

            if (parsedPort != grms->ports_.last())
            {
                condition += " && ";
            }
        }

        outputStream << indentation() << "`define " << grms->state_->name() << " " << condition << endl;
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
