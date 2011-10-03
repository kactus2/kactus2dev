//-----------------------------------------------------------------------------
// File: MCAPITextEdit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.2.2011
//
// Description:
// MCAPI text edit widget.
//-----------------------------------------------------------------------------

#include <QWidget>

#include <common/widgets/assistedTextEdit/AssistedTextEdit.h>

class MCAPIContentMatcher;
class MCAPIHighlighter;

//-----------------------------------------------------------------------------
//! MCAPITextEdit class.
//-----------------------------------------------------------------------------
class MCAPITextEdit : public AssistedTextEdit
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] mainWnd       The program main window. Must not be null.
     *      @param [in] parent        The parent widget. Can be null.
     */
    MCAPITextEdit(QSharedPointer<MCAPIContentMatcher> mcapiMatcher,
                  QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~MCAPITextEdit();

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
    MCAPITextEdit(MCAPITextEdit const& rhs);
    MCAPITextEdit& operator=(MCAPITextEdit const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The MCAPI highlighter.
    MCAPIHighlighter* m_highlighter;

    //! The used content matcher.
    QSharedPointer<MCAPIContentMatcher> m_matcher;
};
