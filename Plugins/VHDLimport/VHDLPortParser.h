//-----------------------------------------------------------------------------
// File: VHDLPortParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.09.2014
//
// Description:
// Parser for VHDL ports.
//-----------------------------------------------------------------------------

#ifndef VHDLPORTPARSER_H
#define VHDLPORTPARSER_H

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>


#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <IPXACTmodels/generaldeclarations.h>

class Component;
class Port;
class HDLEquationParser;
//-----------------------------------------------------------------------------
//! Parser for VHDL ports.
//-----------------------------------------------------------------------------
class VHDLPortParser : public QObject, public HighlightSource
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent object.	 
	 */
	VHDLPortParser(QObject* parent = 0);

	//! The destructor.
	virtual ~VHDLPortParser();

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
     *  Parses the port left bound value from a VHDL port declaration.
     *
     *      @param [in] declaration         The VHDL port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port left bound value in the declaration.
     */
    int parseLeftBound(QString const& input, QSharedPointer<Component> ownerComponent) const;

    /*!
     *  Parses the port right bound value from a VHDL port declaration.
     *
     *      @param [in] declaration         The VHDL port declaration to parse.
     *      @param [in] ownerComponent      The owner component of the port.
     *
     *      @return The port right bound value in the declaration.
     */
    int parseRightBound(QString const& input, QSharedPointer<Component> ownerComponent) const;

    /*!
     *  Parses the port default value from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port default value in the declaration.
     */
    QString parseDefaultValue(QString const& input) const;

    /*!
     *  Removes all previously parsed ports from the visualization.
     */
    void removePreviousPorts();

signals:

    //! Emitted when a port has been added to target component.
    void add(QSharedPointer<Port> port, QString const& declaration) const;

private:

	// Disable copying.
	VHDLPortParser(VHDLPortParser const& rhs);
	VHDLPortParser& operator=(VHDLPortParser const& rhs);

    /*!
     *  Finds all port declarations from input.
     *
     *      @param [in] input   The input to search for port declarations.
     *
     *      @return Found port declarations.
     */
    QStringList findPortDeclarations(QString const& input) const;

    /*!
     *  Finds the section defining the ports of an entity.
     *
     *      @param [in] input   The input to search for port section.
     *
     *      @return The sections containing all ports in entity.
     */
    QString findPortsSection(QString const& input) const;

    /*!
     *  Removes all comment lines from a given input.
     *
     *      @param [in] input   The input to remove comments from.
     *
     *      @return The input without comment lines.
     */
    QString removeCommentLines(QString input) const;

    /*!
     *  Finds all port declarations from a ports section without comment lines.
     *
     *      @param [in] portSectionWithoutCommentLines   The section of the VHDL containing the ports in entity
     *                                                   without comment lines.
     *
     *      @return The VHDL port declarations.
     */
    QStringList portDeclarationsIn(QString const& portSectionWithoutCommentLines) const;

    /*!
     *  Creates a port from a given VHDL declaration and adds it to the target component.
     *
     *      @param [in] declaration         The declaration from which to create a port.
     *      @param [in] targetComponent     The component to which add the port.
     */
    void createPortFromDeclaration(QString const& declaration, QSharedPointer<Component> targetComponent);

    /*!
     *  Parses the port names from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port names in the declaration.
     */
    QStringList parsePortNames(QString const& declaration) const;

    /*!
     *  Parses the port direction from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port direction in the declaration.
     */
    General::Direction parsePortDirection(QString const& declaration) const;

    /*!
     *  Parses the port type from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port type in the declaration.
     */
    QString parsePortType(QString const& declaration) const;

    /*!
     *  Parses the port description from a comment tailing a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port description.
     */
    QString parseDescription(QString const& declaration);

    /*!
     *  Parses the port left bound value from VHDL vector bounds.
     *
     *      @param [in] vectorBounds    The vector bounds to parse.
     *      @param [in] ownerComponent  The owner component of the port.
     *
     *      @return The port left bound value in the vector bounds.
     */
    int parseLeftValue(QString const& vectorBounds, QSharedPointer<Component> ownerComponent) const;
  
    /*!
     *  Parses the port right bound value from VHDL vector bounds.
     *
     *      @param [in] vectorBounds    The vector bounds to parse.
     *      @param [in] ownerComponent  The owner component of the port.
     *
     *      @return The port right bound value in the vector bounds.
     */
    int parseRightValue(QString const& vectorBounds, QSharedPointer<Component> ownerComponent) const;

    /*!
     *  Parses the vector bounds from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The vector bounds.
     */
    QString parseVectorBounds(QString const& declaration) const;

    //! The highlighter to use.
    Highlighter* highlighter_;

};

#endif // VHDLPORTPARSER_H
