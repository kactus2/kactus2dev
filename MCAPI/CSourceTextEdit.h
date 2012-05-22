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

class MCAPIContentMatcher;
class MCAPIHighlighter;

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
     *      @param [in] mainWnd       The program main window. Must not be null.
     *      @param [in] parent        The parent widget. Can be null.
     */
    CSourceTextEdit(QSharedPointer<MCAPIContentMatcher> mcapiMatcher,
                  QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~CSourceTextEdit();

    /*!
     *  Returns the content matcher.
     */
    MCAPIContentMatcher& getMatcher();

    /*!
     *  Returns the highlighter.
     */
    MCAPIHighlighter& getHighlighter();

private:
    // Disable copying.
    CSourceTextEdit(CSourceTextEdit const& rhs);
    CSourceTextEdit& operator=(CSourceTextEdit const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The MCAPI highlighter.
    MCAPIHighlighter* m_highlighter;

    //! The used content matcher.
    QSharedPointer<MCAPIContentMatcher> m_matcher;
};
