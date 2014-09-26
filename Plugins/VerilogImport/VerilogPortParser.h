//-----------------------------------------------------------------------------
// File: VerilogPortParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Parser for Verilog ports.
//-----------------------------------------------------------------------------

#ifndef VERILOGPORTPARSER_H
#define VERILOGPORTPARSER_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>

class Component;

//-----------------------------------------------------------------------------
//! Parser for Verilog ports.
//-----------------------------------------------------------------------------
class VerilogPortParser
{
public:

    //! The constructor.
    VerilogPortParser();

    //! The destructor.
    ~VerilogPortParser();

    /*!
     *  Runs the port parsing for the given input and adds the parsed ports to the given component.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to add all the imported ports to.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent);

private:

    // Disable copying.
    VerilogPortParser(VerilogPortParser const& rhs);
    VerilogPortParser& operator=(VerilogPortParser const& rhs);

    /*!
     *  Finds all port declarations from input.
     *
     *      @param [in] input   The input to search for port declarations.
     *
     *      @return Found port declarations.
     */
    QStringList findPortDeclarations(QString const& input) const;
    
    /*!
     *  Finds the section defining the ports of an module.
     *
     *      @param [in] input   The input to search for port section.
     *
     *      @return The sections containing all ports in module.
     */
    QString findPortsSection(QString const& input) const;

    /*!
     *  Checks if the given input has ports declared in Verilog-1995 style.
     *
     *      @param [in] input   The input to check.
     *
     *      @return True, if Verilog-1995 style ports are found, otherwise false.
     */
    bool hasVerilog1995Ports(QString const& input) const;
    
    /*!
     *  Finds the end of module declaration in given input.
     *
     *      @param [in] input   The input to search for module declaration end.
     *
     *      @return The character index in the input for the end of module declaration.
     */
    int findEndOfModuleDeclaration(QString const& input) const;

    /*!
     *  Finds the section in the input containing all Verilog-1995 style ports.
     *
     *      @param [in] input   The input to search for.
     *
     *      @return The section of the input containing all Verilog-1995 style ports.
     */
    QString findVerilog1995PortsSectionInModule(QString const& input) const;

    /*!
     *  Finds the section in the input containing all Verilog-2001 style ports.
     *
     *      @param [in] input   The input to search for.
     *
     *      @return The section of the input containing all Verilog-2001 style ports.
     */
    QString findVerilog2001PortsSection(QString const& input) const;

    /*!
     *  Removes all comment lines from a given input.
     *
     *      @param [in] input   The input to remove comments from.
     *
     *      @return The input without comment lines.
     */
    QString removeCommentLines(QString portSection) const ;
    
    /*!
     *  Finds all port declarations from a ports section without comment lines.
     *
     *      @param [in] portSectionWithoutCommentLines   The section of the Verilog containing the ports in module
     *                                                   without comment lines.
     *
     *      @return The Verilog port declarations.
     */
    QStringList portDeclarationsIn(QString const& portSection) const;

    /*!
     *  Creates a port from a given Verilog declaration and adds it to the target component.
     *
     *      @param [in] declaration         The declaration from which to create a port.
     *      @param [in] targetComponent     The component to which add the port.
     */
    void createPortFromDeclaration(QString const& portDeclaration, 
        QSharedPointer<Component> targetComponent) const;    

    /*!
     *  Parses the port direction from a Verilog port declaration.
     *
     *      @param [in] declaration   The Verilog port declaration to parse.
     *
     *      @return The port direction in the declaration.
     */
    General::Direction parseDirection(QString const& portDeclaration) const;

    /*!
     *  Parses the port left bound value from a Verilog port declaration.
     *
     *      @param [in] portDeclaration     The Verilog port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port left bound value in the declaration.
     */
    int parseLeftBound(QString const& portDeclaration) const;

    /*!
     *  Parses the port right bound value from a Verilog port declaration.
     *
     *      @param [in] portDeclaration     The Verilog port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port right bound value in the declaration.
     */
    int parseRightBound(QString const& portDeclaration) const;

    /*!
     *  Parses the port names from a Verilog port declaration.
     *
     *      @param [in] declaration   The Verilog port declaration to parse.
     *
     *      @return The port names in the declaration.
     */
    QStringList parsePortNames(QString const& portDeclaration) const;

    /*!
     *  Parses the port description from a comment tailing a Verilog port declaration.
     *
     *      @param [in] declaration   The Verilog port declaration to parse.
     *
     *      @return The port description.
     */
    QString parseDescription(QString const& portDeclaration) const;
    
};

#endif // VERILOGPORTPARSER_H
