//-----------------------------------------------------------------------------
// File: VerilogGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H


#include <QTextStream>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>

#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

class VerilogGenerator
{
public:
    VerilogGenerator();
    ~VerilogGenerator();

    void write(QTextStream& outputStream) const;

    void writeComponentInstances(QTextStream& outputStream) const;

    void writeParameterDeclarations(QTextStream& outputStream) const;

    void writeModuleEnd(QTextStream& outputStream) const;

    void writePortDeclarations(QTextStream& outputStream) const;

    void writeModuleBegin(QTextStream& outputStream) const;

    void parse(QSharedPointer<const Component> component, 
        QSharedPointer<const Design> design = QSharedPointer<const Design>());

private:
    
    QString name_;

    QList<PortVerilogWriter> portWriters_;
    QString portDeclarations_;

    QString parameterDeclarations_;

    QString componentInstances_;

    bool parseCalled_;

    QSharedPointer<const Component> component_;
};

#endif // VERILOGGENERATOR_H
