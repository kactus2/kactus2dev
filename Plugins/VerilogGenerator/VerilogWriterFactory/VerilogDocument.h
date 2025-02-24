//-----------------------------------------------------------------------------
// File: VerilogDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers.
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
    //! This will contain the found implementation, if success.
    QString implementation_;
    //! This may contain the text after the module definition, if success.
    QString postModule_;

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

    //! The constructor.
    VerilogDocument();
    
    //! The destructor.
    virtual  ~VerilogDocument() = default;

    /*!
     *  Writes the content. 
     *
     *    @param [in] outputDirectory         The possible output directory.
     */
	virtual void write(QString const& outputDirectory) override;
    
    /*!
     *  Finds position for body text highlight in document, if any exists.
     *
	 *    @param [out] begin                  The position where the highlight begins, if successful.
     *    @param [out] end                    The position where the highlight ends, if successful.
     */
    virtual void getBodyHighlight(int& begin, int& end) const override;

    /*!
     *  Finds the module implementation in Verilog code.
     *
     *    @param [in] code		            The code that will be inspected.
	 *    @param [out] implementation         The found module implementation, if successful.
     *    @param [out] postModule		        Anything that exists after the module declaration.
     *    @param [out] error                  If fails, this will contain an error message.
     *
     *    @return True, if a single implementation could be found, else false.
     */
	bool selectImplementation(QString const& code, QString& implementation,
        QString& postModule, QString& error);
};

#endif // VERILOGDOCUMENT_H
