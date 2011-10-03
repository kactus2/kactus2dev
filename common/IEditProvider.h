//-----------------------------------------------------------------------------
// File: IEditProvider.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.7.2011
//
// Description:
// IEditProvider interface for providing editing capabilities (undo & redo).
//-----------------------------------------------------------------------------

#ifndef IEDITPROVIDER_H
#define IEDITPROVIDER_H

#include <QObject>

//-----------------------------------------------------------------------------
//! IEditProvider interface.
//-----------------------------------------------------------------------------
class IEditProvider : public QObject
{
    Q_OBJECT

public:
    /*!
     *  Destructor.
     */
    virtual ~IEditProvider() {}

    /*!
     *  Performs an undo.
     */
    virtual void undo() = 0;

    /*!
     *  Performs a redo.
     */
    virtual void redo() = 0;

    /*!
     *  Returns true if undo is possible.
     */
    virtual bool canUndo() const = 0;

    /*!
     *  Returns true if redo is possible.
     */
    virtual bool canRedo() const = 0;

signals:
    //! Signaled when the state of the provider has changed.
    void editStateChanged();
};

//-----------------------------------------------------------------------------

#endif // IEDITPROVIDER_H
