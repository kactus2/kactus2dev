//-----------------------------------------------------------------------------
// File: VHDLGenericParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.09.2014
//
// Description:
// Parser for VHDL generics.
//-----------------------------------------------------------------------------

#ifndef VHDLGENERICPARSER_H
#define VHDLGENERICPARSER_H

#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <QSharedPointer>
#include <QString>

class Component;
class ModelParameter;

//-----------------------------------------------------------------------------
//! Parser for VHDL generics.
//-----------------------------------------------------------------------------
class VHDLGenericParser : public QObject, public HighlightSource
{    
    Q_OBJECT
public:

	//! The constructor.
	VHDLGenericParser(QObject* parent = 0);

	//! The destructor.
	virtual ~VHDLGenericParser();

    /*!
     *  Runs the generic parser for the given input and adds the model parameters to the given component.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to add the model parameters to.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Sets the given highlighter to be used by the generic parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

private:

	// Disable copying.
	VHDLGenericParser(VHDLGenericParser const& rhs);
	VHDLGenericParser& operator=(VHDLGenericParser const& rhs);

    /*!
     *  Finds all generic declarations from input.
     *
     *      @param [in] input   The input to search for generic declarations.
     *
     *      @return Found generic declarations.
     */
    QStringList findGenericDeclarations(QString const& input) const;

    /*!
     *  Finds the section defining the generics of an entity.
     *
     *      @param [in] input   The input to search for generic section.
     *
     *      @return The sections containing all generics in entity.
     */
    QString findGenericsSection(QString const& input) const;

    /*!
     *  Removes all comment lines from a given input.
     *
     *      @param [in] input   The input to remove comments from.
     *
     *      @return The input without comment lines.
     */
    QString removeCommentLines(QString section) const;

    
    /*!
     *  Finds all generic declarations from a generics section without comment lines.
     *
     *      @param [in] sectionWithoutCommentLines   The section of the VHDL containing the generics in entity
     *                                               without comment lines.
     *
     *      @return The VHDL generic declarations.
     */
    QStringList genericDeclarationsIn(QString const& sectionWithoutCommentLines) const;

    /*!
     *  Creates a model parameter from a given VHDL declaration and adds it to the target component.
     *
     *      @param [in] declaration         The declaration from which to create a model parameter.
     *      @param [in] targetComponent     The component to which add the model parameter.
     */
    void createModelParameterFromDeclaration(QString const& declaration, 
        QSharedPointer<Component> targetComponent);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;
    
};

#endif // VHDLGENERICPARSER_H
