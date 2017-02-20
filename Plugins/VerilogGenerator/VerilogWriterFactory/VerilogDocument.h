//-----------------------------------------------------------------------------
// File: VerilogDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers, logic for calling them.
//-----------------------------------------------------------------------------

#ifndef VERILOGDOCUMENT_H
#define VERILOGDOCUMENT_H

#include "../veriloggeneratorplugin_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/OutputControl.h>

#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>

//-----------------------------------------------------------------------------
// Verilog document.
//-----------------------------------------------------------------------------
struct VerilogDocument : public GenerationOutput
{
    //! Writer for generating file header.
    QSharedPointer<VerilogHeaderWriter> headerWriter_;

    //! Writer for the component, is to contain everything that comes to document, except header writer.
    QSharedPointer<ComponentVerilogWriter> topWriter_;

    //! Writer for top defaults.
    QSharedPointer<WriterGroup> topDefaultWriters_;

    //! Writers for Verilog interconnections.
    QSharedPointer<WriterGroup> interconnectionWriters_;

    //! Writers for interconnection wires.
    QSharedPointer<WriterGroup> connectionWireWriters_;

    //! Writers for interconnection wires.
    QSharedPointer<WriterGroup> adHocWireWriters_;

    //! Writers for port assignment wires.
    QSharedPointer<WriterGroup> portWireWriters_;

    //! Writers for hierarchical port assignments.
    QSharedPointer<WriterGroup> topAssignmentWriters_;

    //! Writers for instance port assignments.
    QSharedPointer<WriterGroup> instanceAssignmentWriters_;

    //! Writers for Verilog instances.
    QList<QSharedPointer<ComponentInstanceVerilogWriter> > instanceWriters_;

    //! Writers for Verilog instances.
    QMap<QSharedPointer<ComponentInstanceVerilogWriter>, QSharedPointer<Writer> > instanceHeaderWriters_;

    /*!
     *  Writes the content.
     */
	void write();
};

#endif // VerilogDocument_H
