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

#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

#include <QSharedPointer>

namespace
{
    const QString INDENT = "    ";
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<const Component> component) :
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::~ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::~ComponentVerilogWriter()
{

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

    writeParameterDeclarations(outputStream);

    writePortDeclarations(outputStream);

    writeModuleEnd(outputStream);
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
void ComponentVerilogWriter::writeModuleDeclaration( QTextStream& outputStream ) const
{
    outputStream << "module " << component_->getVlnv()->getName() << "(" << portNames() << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortNames()
//-----------------------------------------------------------------------------
QString ComponentVerilogWriter::portNames() const
{
    QStringList portNames;
    foreach(QSharedPointer<Port> port, sortedPorts())
    {
        portNames.append(port->getName());
    }

    return portNames.join(", ");
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::sortedPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > ComponentVerilogWriter::sortedPorts() const
{
    QMap<VhdlPortSorter, QSharedPointer<Port> > ports;
    foreach(QSharedPointer<Port> port, component_->getPorts())
    {
        QString portName = port->getName();
        VhdlPortSorter sorter(component_->getInterfaceNameForPort(portName), portName, port->getDirection());
        ports.insertMulti(sorter, port);
    }

    return ports.values();
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentVerilogWriterGenerator::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
{
    foreach(QSharedPointer<ModelParameter> parameter, component_->getModelParameters())
    {
        outputStream << INDENT;
        ModelParameterVerilogWriter writer(parameter);
        writer.write(outputStream);
    } 
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    QString previousInterfaceName = "";

    foreach(QSharedPointer<Port> port, sortedPorts())
    {    
        writeInterfaceIntroduction(component_->getInterfaceNameForPort(port->getName()), previousInterfaceName, 
            outputStream);

        outputStream << INDENT;
        PortVerilogWriter writer(port);
        writer.write(outputStream);
    }

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInterfaceIntroduction()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
    QTextStream& outputStream) const
{
    bool isFirstInterface = previousInterfaceName.isEmpty();
  
    if (previousInterfaceName.compare(interfaceName) != 0)
    {
        if (!isFirstInterface)
        {
            outputStream << endl;
        }

        if (interfaceName == "none")
        {
            outputStream << INDENT << "// These ports are not in any interface" << endl;                
        }
        else if (interfaceName == "several")
        {
            outputStream << INDENT << "// There ports are contained in many interfaces" << endl;       
        }
        else
        {
            outputStream << INDENT << "// Interface: " << interfaceName << endl;

            QSharedPointer<const BusInterface> busInterface = component_->getBusInterface(interfaceName);
            if (!busInterface->getDescription().isEmpty())
            {
                HDLUtils::writeDescription(busInterface->getDescription(), outputStream, INDENT + "//");
            }
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleEnd()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}
