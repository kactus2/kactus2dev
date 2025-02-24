//-----------------------------------------------------------------------------
// File: VerilogHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// Verilog language-specific highlight rules.
//-----------------------------------------------------------------------------

#ifndef VERILOGSOURCEHIGHLIGHT_H
#define VERILOGSOURCEHIGHLIGHT_H

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTextCharFormat>

#include <Plugins/common/SourceHighlightStyle.h>

//-----------------------------------------------------------------------------
//! VerilogSourceHighlight class.
//-----------------------------------------------------------------------------
class VerilogSourceHighlight : public SourceHighlightStyle
{
public:
   
    //! The constructor.
    VerilogSourceHighlight();

    //! The destructor.
    virtual ~VerilogSourceHighlight();

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
    VerilogSourceHighlight(VerilogSourceHighlight const& rhs);
    VerilogSourceHighlight& operator=(VerilogSourceHighlight const& rhs);

};

//-----------------------------------------------------------------------------

#endif // VERILOGSOURCEHIGHLIGHT_H
