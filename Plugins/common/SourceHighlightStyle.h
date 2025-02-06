//-----------------------------------------------------------------------------
// File: SourceHighlightStyle.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// Base class for language-specific highlighters.
//-----------------------------------------------------------------------------

#ifndef SOURECEHIGHLIGHSTYLE_H
#define SOURECEHIGHLIGHSTYLE_H

#include "LanguageHighlighter.h"

//-----------------------------------------------------------------------------
//! SourceHighlightStyle class.
//-----------------------------------------------------------------------------
class SourceHighlightStyle
{
public:
    
    //! The constructor.
    SourceHighlightStyle();

    //! The destructor.
    virtual ~SourceHighlightStyle() = default;

    // Disable copying.
    SourceHighlightStyle(SourceHighlightStyle const& rhs) = delete;
    SourceHighlightStyle& operator=(SourceHighlightStyle const& rhs) = delete;

    /*!
     *  Apply the language style rules on the given highlighter.
     *
     *      @param [in] highlighter     The highlighter to apply the highlight style rules to.
     */
    void apply(LanguageHighlighter* highlighter) const;

protected:
   
    /*!
     *  Get the language specific keywords.
     *
     *      @return The language keywords.
     */
    virtual QStringList keywords() const = 0;
       
    /*!
     *  Get the language specific preprocessor directives.
     *
     *      @return The preprocessor directives.
     */
    virtual QStringList preprocessorDirectives() const = 0;

    /*!
     *  Apply other language specific rules to highlighting.
     *
     *      @param [in] highlighter   The highlighter to apply the rules to.
     */
    virtual void applyLanguageSpecificRules(LanguageHighlighter* highlighter) const; 


};

//-----------------------------------------------------------------------------

#endif // SOURECEHIGHLIGHSTYLE_H
