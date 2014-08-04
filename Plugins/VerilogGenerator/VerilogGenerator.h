//-----------------------------------------------------------------------------
// File: VerilogGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H


#include <QTextStream>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>

#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogGenerator
{
public:
    //! The constructor.
    VerilogGenerator();

    //! The destructor.
    ~VerilogGenerator();

    /*!
     *  Generates the component Verilog to a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void generate(QString const& outputPath) const;

    /*!
     *  Parses a given component for generation.
     *
     *      @param [in] component   The component to parse for generation.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void parse(QSharedPointer<const Component> component);

private:
    
    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Writer for generating file header.
    VerilogHeaderWriter* headerWriter_;

    //! Writer for top level component.
    ComponentVerilogWriter* topWriter_;

    //! The top level component.
    QSharedPointer<const Component> topComponent_;
};

#endif // VERILOGGENERATOR_H
