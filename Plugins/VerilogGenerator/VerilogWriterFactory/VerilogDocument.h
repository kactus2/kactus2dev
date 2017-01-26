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

#include <QTextStream>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogAssignmentWriter/VerilogAssignmentWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/VerilogTopDefaultWriter.h>
#include <Plugins/VerilogGenerator/VerilogInterconnectionWriter/VerilogInterconnectionWriter.h>

//-----------------------------------------------------------------------------
// Verilog document.
//-----------------------------------------------------------------------------
class VerilogDocument
{
public:
    //! The name of the file for the document
    QString fileName_;

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

    //! Witers for Verilog instances.
    QMap<QSharedPointer<ComponentInstanceVerilogWriter>, QSharedPointer<Writer> > instanceHeaderWriters_;

	/*!
	 *  The constructor.
     */
    VerilogDocument();

    //! The destructor.
    ~VerilogDocument();
    
    /*!
     *  Writes the Verilog to files.
     *
     *      @remark If prepares are not called before generate(), nothing is generated.
     */
	void write(QTextStream& outputStream) const;

private:

    // Disable copying.
    VerilogDocument(VerilogDocument const& rhs);
    VerilogDocument& operator=(VerilogDocument const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // VerilogDocument_H
