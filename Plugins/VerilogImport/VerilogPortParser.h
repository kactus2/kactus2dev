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

#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <QSharedPointer>
#include <QString>

class Component;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Parser for Verilog ports.
//-----------------------------------------------------------------------------
class VerilogPortParser: public HighlightSource
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
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Sets the given highlighter to be used by the port parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);
    
    /*!
     *  Sets the parser to use for expressions.
     *
     *      @param [in] parser   The parser to use.
     */
    void setExpressionParser(QSharedPointer<ExpressionParser> parser);

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
    int findStartOfPortList(QString const& input) const;

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
     *  Removes all ignored lines e.g. comments from a given input.
     *
     *      @param [in] input   The input to remove lines from.
     *
     *      @return The input without ignored lines.
     */
    QString removeIgnoredLines(QString portSection) const ;
    
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
     *  Highlights the given port declaration.
     *
     *      @param [in] portDeclaration   The port declaration to highlight.
     */
    void highlight(QString const& portDeclaration);

    /*!
     *  Parses the port direction from a Verilog port declaration.
     *
     *      @param [in] declaration   The Verilog port declaration to parse.
     *
     *      @return The port direction in the declaration.
     */
    General::Direction parseDirection(QString const& portDeclaration) const;
    
    /*!
     *  Parses the port array bounds from a Verilog port declaration.
     *
     *      @param [in] portDeclaration     The Verilog port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port left and right array bound value in the declaration.
     */
    QPair<QString, QString> parseArrayBounds(QString const& portDeclaration,
        QSharedPointer<Component> targetComponent) const;

    /*!
     *  Parses the left and right bounds of a bounds that is of format [left:right].
     *  Any name references will be replaced with id references.
     *
     *      @param [in] bounds              The expression to parse.
     *      @param [in] targetComponent     The owner component of the port.
     *
     *      @return The left and right bound value in the bounds.
     */
    QPair<QString, QString> parseLeftAndRight(QString const& bounds, 
        QSharedPointer<Component> targetComponent) const;

    /*!
     *  Parses the port vector bounds from a Verilog port declaration.
     *
     *      @param [in] portDeclaration     The Verilog port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port left and right vector bound value in the declaration.
     */
    QPair<QString, QString> parseVectorBounds(QString const& portDeclaration,
        QSharedPointer<Component> targetComponent) const;

    /*!
     *  Replaces any parameter names with their ids in an expression.
     *
     *      @param [in] expression          The expression to replace the names in.
     *      @param [in] targetComponent     The containing component of the parameters.
     *
     *      @return The expression where the names have been replaced with corresponding ids.
     */
    QString replaceModelParameterNamesWithIds(QString const& expression, 
        QSharedPointer<Component> targetComponent) const;

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
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> parser_;
};

#endif // VERILOGPORTPARSER_H
