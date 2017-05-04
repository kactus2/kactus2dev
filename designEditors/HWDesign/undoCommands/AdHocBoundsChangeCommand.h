//-----------------------------------------------------------------------------
// File: AdHocBoundsChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.05.2017
//
// Description:
// Undo command for changing left and right bounds of an ad hoc port.
//-----------------------------------------------------------------------------

#ifndef ADHOCBOUNDSCHANGECOMMAND_H
#define ADHOCBOUNDSCHANGECOMMAND_H

class PortReference;
class PartSelect;

#include <QUndoCommand>
#include <QSharedPointer>
#include <QString>

//-----------------------------------------------------------------------------
//! Undo command for changing left and right bounds of an ad hoc port.
//-----------------------------------------------------------------------------
class AdHocBoundsChangeCommand : public QUndoCommand
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] referencedPort  The changed ad hoc port.
     *      @param [in] newLeftBound    New value for the left bound of the tie off.
     *      @param [in] newRightBound   New value for the right bound of the tie off.
     *      @param [in] parent          The parent command.
     */
    AdHocBoundsChangeCommand(QSharedPointer<PortReference> referencedPort, QString const& newLeftBound,
        QString const& newRightBound, QUndoCommand* parent = 0);

    /*!
     *  The destructor.
     */
    ~AdHocBoundsChangeCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    
    // Disable copying.
    AdHocBoundsChangeCommand(AdHocBoundsChangeCommand const& rhs);
    AdHocBoundsChangeCommand& operator=(AdHocBoundsChangeCommand const& rhs);

    /*!
     *  Setup the part select for the selected port reference.
     *
     *      @param [in] newLeftValue    New value for the left bound.
     *      @param [in] newRightValue   New value for the right bound.
     */
    void setupPartSelectForPortReference(QString const& newLeftValue, QString const& newRightValue);

    /*!
     *  Set a new part select for the referenced port.
     *
     *      @param [in] partSelect  The new part select.
     */
    void setPartSelectForReferencedPort(QSharedPointer<PartSelect> partSelect);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The selected port.
    QSharedPointer<PortReference> referencedPort_;

    //! The old part select of the referenced port.
    QSharedPointer<PartSelect> oldPartSelect_;

    //! The new part select of the referenced port.
    QSharedPointer<PartSelect> newPartSelect_;
};

#endif // ADHOCTIEOFFBOUNDSCHANGECOMMAND_H
