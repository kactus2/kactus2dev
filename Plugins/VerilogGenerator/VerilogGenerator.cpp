//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(): name_(), portDeclarations_(), portWriters_(), parameterDeclarations_(), componentInstances_(), parseCalled_(false), component_()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::write()
//-----------------------------------------------------------------------------
void VerilogGenerator::write(QTextStream& outputStream) const
{
    if (!parseCalled_)
    {
        return;
    }

    writeModuleBegin(outputStream);

    writePortDeclarations(outputStream);

    writeParameterDeclarations(outputStream);

    writeComponentInstances(outputStream);

    writeModuleEnd(outputStream);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<const Component> component, QSharedPointer<const Design> design)
{
    component_ = component;

    name_ = component_->getVlnv()->getName();

    foreach(QSharedPointer<Port> port, sortedPorts())
    {
        portWriters_.append(PortVerilogWriter(port));
    }

    foreach(QSharedPointer<ModelParameter> parameter, component_->getModelParameters())
    {
        parameterDeclarations_.append("    ");
        parameterDeclarations_.append("parameter ");
        parameterDeclarations_.append(parameter->getName() + " ");
        parameterDeclarations_.append("= " + parameter->getValue());
        parameterDeclarations_.append(";\n");
    }

    if (design)
    {        
        componentInstances_.append("\n");
        foreach(ComponentInstance instance, design->getComponentInstances())
        {
            componentInstances_.append("    ");
            componentInstances_.append(instance.getInstanceName() + " " + instance.getComponentRef().getName()  + 
                "(");           
            componentInstances_.append(");\n");
            componentInstances_.append("\n");
        }        
    }

    parseCalled_ = true;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writeModuleBegin()
//-----------------------------------------------------------------------------
void VerilogGenerator::writeModuleBegin(QTextStream& outputStream) const
{
    outputStream << "module " << name_ << "(";

    writePortNames(outputStream);

    
    outputStream << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writePortNames()
//-----------------------------------------------------------------------------
void VerilogGenerator::writePortNames(QTextStream& outputStream) const
{
    QStringList portNames;
    foreach(QSharedPointer<Port> port, sortedPorts())
    {
        portNames.append(port->getName());
    }

    outputStream << portNames.join(", ");
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writePortDeclarations()
//-----------------------------------------------------------------------------
void VerilogGenerator::writePortDeclarations(QTextStream& outputStream) const
{
    foreach(PortVerilogWriter portWriter, portWriters_)
    {
        portWriter.write(outputStream);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writeModuleEnd()
//-----------------------------------------------------------------------------
void VerilogGenerator::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void VerilogGenerator::writeParameterDeclarations(QTextStream& outputStream) const
{
    outputStream << parameterDeclarations_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::writeComponentInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::writeComponentInstances(QTextStream& outputStream) const
{
    outputStream << componentInstances_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::sortedPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > VerilogGenerator::sortedPorts() const
{
    QMap<VhdlPortSorter, QSharedPointer<Port> > ports;
    foreach(QSharedPointer<Port> port, component_->getPorts())
    {
        VhdlPortSorter sorter(component_->getInterfaceNameForPort(port->getName()), port->getName(), port->getDirection());
        ports.insertMulti(sorter, port);
    }

    return ports.values();
}
