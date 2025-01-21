//-----------------------------------------------------------------------------
// File: PythonSourceHighlight.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.06.2022
//
// Description:
// Python language-specific highlight rules.
//-----------------------------------------------------------------------------

#ifndef PYTHONSOURCEHIGHLIGHT_H
#define PYTHONSOURCEHIGHLIGHT_H

#include <Plugins/common/SourceHighlightStyle.h>

class LanguageHighlighter;

//-----------------------------------------------------------------------------
//! PythonSourceHighlight class.
//-----------------------------------------------------------------------------
class PythonSourceHighlight: public SourceHighlightStyle
{
public:

    //! The constructor.
    PythonSourceHighlight();

    //! The destructor.
    virtual ~PythonSourceHighlight() = default;

    // Disable copying.
    PythonSourceHighlight(PythonSourceHighlight const& rhs) = delete;
    PythonSourceHighlight& operator=(PythonSourceHighlight const& rhs) = delete;

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

};

//-----------------------------------------------------------------------------

#endif // PYTHONSOURCEHIGHLIGHT_H
