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

#include <KactusAPI/include/HighlightSource.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QObject>

class Component;
class ComponentInstantiation;
class Port;

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
	explicit VHDLPortParser(QObject* parent = 0);

	//! The destructor.
	virtual ~VHDLPortParser() = default;

    // Disable copying.
    VHDLPortParser(VHDLPortParser const& rhs) = delete;
    VHDLPortParser& operator=(VHDLPortParser const& rhs) = delete;

    /*!
     *  Runs the port parsing for the given input and adds the parsed ports to the given component.
     *
     *      @param [in] input                               The input text to parse.
     *      @param [in/out] targetComponent                 The component to add all the imported ports to.
     *      @param [in] targetComponentInstantiation        The component instantiation for the import.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent,
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

    /*!
     *  Sets the given highlighter to be used by the port parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

private:

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
     *      @param [in] declaration             The declaration from which to create a port.
     *      @param [in/out] targetComponent     The component to which add the port.
     */
    void createPortFromDeclaration(QString const& declaration,
        QSharedPointer<Component> targetComponent) const;

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
    DirectionTypes::Direction parsePortDirection(QString const& declaration) const;

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
    QString parseDescription(QString const& declaration) const;

    /*!
     *  Parses the port default value from a VHDL port declaration.
     *
     *      @param [in] declaration   The VHDL port declaration to parse.
     *
     *      @return The port default value in the declaration.
     */
    QString parseDefaultValue(QString const& input, QSharedPointer<Component> targetComponent) const;

    /*!
     *  Replaces all name references in the given expression to parameter id references.
     *
     *      @param [in] expression          The expression whose references to replace.
     *      @param [in] targetComponent     The component containing the referenced parameters.
     *
     *      @return The expression with replaced references.
     */
    QString replaceNameReferencesWithIds(QString const& expression, QSharedPointer<Component> targetComponent) const;

    /*!
     *  Parses the port bounds from a VHDL port declaration.
     *
     *      @param [in] declaration         The VHDL port declaration to parse.
     *      @param [in] targetComponent     The component to which add the port.
     *
     *      @return The port bounds <left, right> in the given declaration.
     */
    QPair<QString, QString> parsePortBounds(QString const& declaration,
        QSharedPointer<Component> targetComponent) const;
   
    int findMatchingEndParenthesis(QString const& equation, int parenthesesStart) const;

    /*!
     *  Creates a default type definition for the given type.
     *
     *      @param [in] type   The type whose type definition to create.
     *
     *      @return The default type defition for the type, if any.
     */
    QString createDefaultTypeDefinition(QString const& type) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // VHDLPORTPARSER_H
