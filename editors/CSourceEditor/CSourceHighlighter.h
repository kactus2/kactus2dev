//-----------------------------------------------------------------------------
// File: CSourceHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#ifndef CSOURCEHIGHLIGHTER_H
#define CSOURCEHIGHLIGHTER_H

#include <Plugins/common/LanguageHighlighter.h>

#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTextCharFormat>

class ApiDefinition;

//-----------------------------------------------------------------------------
//! CSourceHighlighter class.
//-----------------------------------------------------------------------------
class CSourceHighlighter : public LanguageHighlighter
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent text document.
     */
    CSourceHighlighter(QTextDocument* parent);

    /*!
     *  Destructor.
     */
    virtual ~CSourceHighlighter();

    /*!
     *  Registers data types and functions from the given API definition for syntax highlight.
     *
     *      @param [in] apiDef The API definition.
     */
    void registerAPI(QSharedPointer<ApiDefinition const> apiDef);

private:

    // Disable copying.
    CSourceHighlighter(CSourceHighlighter const& rhs);
    CSourceHighlighter& operator=(CSourceHighlighter const& rhs);

    /*!
     *  Applies MCAPI style to the highlighter.
     */
    void applyMCAPIStyle();
};

//-----------------------------------------------------------------------------

#endif // CSOURCEHIGHLIGHTER_H
