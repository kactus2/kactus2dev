//-----------------------------------------------------------------------------
// File: TextEditProvider.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.7.2011
//
// Description:
// Specialized edit provider for QPlainTextEdit.
//-----------------------------------------------------------------------------

#ifndef TEXTEDITPROVIDER_H
#define TEXTEDITPROVIDER_H

#include "IEditProvider.h"

#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! TextEditProvider class.
//-----------------------------------------------------------------------------
class TextEditProvider : public IEditProvider
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] textEdit The text editor to which the provider applies.
     */
    TextEditProvider(QPlainTextEdit& textEdit);

    /*!
     *  Destructor.
     */
    ~TextEditProvider();

    /*!
     *  Performs an undo.
     */
    virtual void undo();

    /*!
     *  Performs a redo.
     */
    virtual void redo();

    /*!
     *  Returns true if undo is possible.
     */
    virtual bool canUndo() const;

    /*!
     *  Returns true if redo is possible.
     */
    virtual bool canRedo() const;

private:
    // Disable copying.
    TextEditProvider(TextEditProvider const& rhs);
    TextEditProvider& operator=(TextEditProvider const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The text edit.
    QPlainTextEdit& textEdit_;    
};

//-----------------------------------------------------------------------------

#endif // TEXTEDITPROVIDER_H
