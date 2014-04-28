//-----------------------------------------------------------------------------
// File: CSourceTextEdit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.2.2011
//
// Description:
// C source text edit widget.
//-----------------------------------------------------------------------------

#include <QWidget>

#include <common/widgets/assistedTextEdit/AssistedTextEdit.h>

class CSourceContentMatcher;
class CSourceHighlighter;

//-----------------------------------------------------------------------------
//! CSourceTextEdit class.
//-----------------------------------------------------------------------------
class CSourceTextEdit : public AssistedTextEdit
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] mainWnd         The program main window. Must not be null.
     *      @param [in] parent          The parent widget. Can be null.
     */
    CSourceTextEdit(QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~CSourceTextEdit();

    /*!
     *  Returns the content matcher.
     */
    CSourceContentMatcher& getMatcher();

    /*!
     *  Returns the highlighter.
     */
    CSourceHighlighter& getHighlighter();

private:
    // Disable copying.
    CSourceTextEdit(CSourceTextEdit const& rhs);
    CSourceTextEdit& operator=(CSourceTextEdit const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The MCAPI highlighter.
    CSourceHighlighter* m_highlighter;

    //! The used content matcher.
    QSharedPointer<CSourceContentMatcher> m_matcher;
};
