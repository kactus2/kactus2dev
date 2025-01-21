//-----------------------------------------------------------------------------
// File: CSourceHighlight.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// C source language-specific highlight rules.
//-----------------------------------------------------------------------------

#ifndef CSOURCEHIGHLIGHT_H
#define CSOURCEHIGHLIGHT_H

#include <Plugins/common/SourceHighlightStyle.h>

class LanguageHighlighter;

//-----------------------------------------------------------------------------
//! CSourceHighlight class.
//-----------------------------------------------------------------------------
class CSourceHighlight: public SourceHighlightStyle
{
public:

    //! The constructor.
    CSourceHighlight();

    //! The destructor.
    virtual ~CSourceHighlight();

protected:

    /*!
     *  Get the language specific keywords.
     *
     *    @return The language keywords.
     */   
    virtual QStringList keywords() const;
            
    /*!
     *  Get the language specific preprocessor directives.
     *
     *    @return The preprocessor directives.
     */
    virtual QStringList preprocessorDirectives() const;

    /*!
     *  Apply other language specific rules to highlighting.
     *
     *    @param [in] highlighter   The highlighter to apply the rules to.
     */
    virtual void applyLanguageSpecificRules(LanguageHighlighter* highlighter) const; 

private:

    // Disable copying.
    CSourceHighlight(CSourceHighlight const& rhs);
    CSourceHighlight& operator=(CSourceHighlight const& rhs);
};

//-----------------------------------------------------------------------------

#endif // CSOURCEHIGHLIGHT_H
