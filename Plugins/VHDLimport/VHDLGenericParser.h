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
#include "IPXACTmodels/Component/ComponentInstantiation.h"

class Component;
class ModuleParameter;
class ComponentInstantiation;

//-----------------------------------------------------------------------------
//! Parser for VHDL generics.
//-----------------------------------------------------------------------------
class VHDLGenericParser : public QObject, public HighlightSource
{    
    Q_OBJECT
public:

	//! The constructor.
	explicit VHDLGenericParser(QObject* parent = 0);

	//! The destructor.
	virtual ~VHDLGenericParser() = default;

    /*!
     *  Runs the generic parser for the given input and adds the module parameters to the given component.
     *
     *      @param [in] input                           The input text to parse.
     *      @param [in/out] targetComponent             The component to add the parameters to.
     *      @param [in] targetComponentInstantiation    The component instantiation to add the module parameters to.
     */
    void import(QString const& input, QSharedPointer<Component> targetComponent,
		QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

    /*!
     *  Sets the given highlighter to be used by the generic parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter) override final;

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
     *  Creates a module parameter from a given VHDL declaration and adds it to the target component instantiation.
     *  An equivalent parameter will be added to the component.
     *
     *      @param [in] declaration                         The declaration from which to create a model parameter.
     *      @param [in/out] targetComponentInstantiation    The component instantiation to which add the module parameter.
     */
    void createModelParameterFromDeclaration(QString const& declaration, 
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const;

    /*!
     *  Finds the given module parameter.
     *
     *      @param [in] name                            The name of the module parameter to find.
     *      @param [in] targetComponentInstantiation    The instantiation to search in.
     *
     *      @return The found module parameter.
     */
    QSharedPointer<ModuleParameter> findModuleParameter(QString const& name,
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const;

    /*!
     *  Replaces all name references in the module parameters to parameter id references.
     *
     *      @param [in] targetComponent                 The component containing the parameters.
     *      @param [in] targetComponentInstantiation    The component instantiation containing the module parameters.
     */
    void replaceNamesReferencesWithIds(QSharedPointer<Component> targetComponent, 
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const;
    
    /*!
     *  Finds the given parameter.
     *
     *      @param [in] name        The name of the parameter to find.
     *      @param [in] component   The component to search in.
     *
     *      @return The found parameter.
     */
    QSharedPointer<Parameter> findParameter(QString const& name, QSharedPointer<Component> component) const;
    
    /*!
     *  Replaces all name references in the given parameter to parameter id references.
     *
     *      @param [in] parameter    The parameter whose references to replace.
     *      @param [in] component    The component containing the referenced parameters.
     */
    void replaceNameReferencesWithParameterIds(QSharedPointer<Parameter> parameter,
        QSharedPointer<Component> targetComponent) const;

    /*!
     *  Replaces the given name in the given id in the referenced parameter.
     *
     *      @param [in] expression      The expression to replace names in.
     *      @param [in] namePattern     The pattern for the given name.
     *      @param [in] referenced      The referenced parameter.
     *
     *      @return The expression where name references have been replaced with ids.
     */
    QString replaceNameWithId(QString const& expression, QRegularExpression& namePattern,
        QSharedPointer<Parameter> referenced) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;
    
};

#endif // VHDLGENERICPARSER_H
