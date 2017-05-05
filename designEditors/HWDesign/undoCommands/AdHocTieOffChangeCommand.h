//-----------------------------------------------------------------------------
// File: AdHocTieOffChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.05.2016
//
// Description:
// Undo command for changing tie off in ad hoc editor.
//-----------------------------------------------------------------------------

#ifndef ADHOCTIEOFFCHANGECOMMAND_H
#define ADHOCTIEOFFCHANGECOMMAND_H

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <designEditors/HWDesign/undoCommands/AdHocTiedValueCommand.h>

#include <QUndoCommand>
#include <QSharedPointer>

class AdHocItem;
class AdHocConnection;
class Design;
class HWDesignDiagram;
class PortReference;

//-----------------------------------------------------------------------------
//! Undo command for changing tie off in ad hoc editor.
//-----------------------------------------------------------------------------
class AdHocTieOffChangeCommand : public AdHocTiedValueCommand, public QUndoCommand
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] portItem            The selected port item.
     *      @param [in] connection          The tie off connection.
     *      @param [in] newTieOffValue      A new value for tie off.
     *      @param [in] newParsedTieOff     Parsed value of the new tie off.
     *      @param [in] newFormattedTieOff  New tie off with the referenced parameter UUIDs changed into names.
     *      @param [in] newBase             The base number of the parsed new tie off.
     *      @param [in] oldTieOffValue      The old value for the tie off.
     *      @param [in] oldParsedTieOff     Parsed value of the old tie off.
     *      @param [in] oldFormattedTieOff  Old tie off with the referenced parameter UUIDs changed into names.
     *      @param [in] oldBase             The base number of the parsed old tie off.
     *      @param [in] design              The containing design.
     *      @param [in] parent              Parent command.
     */
    AdHocTieOffChangeCommand(AdHocItem* portItem, QSharedPointer<AdHocConnection> connection,
        QString const& newTieOffValue, QString newParsedTieOff, QString newFormattedTieOff, int newBase,
        QString const& oldTieOffValue, QString oldParsedTieOff, QString oldFormattedTieOff, int oldBase,
        HWDesignDiagram* designDiagram, QUndoCommand* parent = 0);

    /*!
     *  The destructor.
     */
    ~AdHocTieOffChangeCommand();

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
    AdHocTieOffChangeCommand(AdHocTieOffChangeCommand const& rhs);
    AdHocTieOffChangeCommand& operator=(AdHocTieOffChangeCommand const& rhs);

    /*!
     *  Change the tie off symbols in the connected ports.
     *
     *      @param [in] tieOffValue         The tie off value.
     *      @param [in] parsedTieOff        The parsed tie off value.
     *      @param [in] formattedTieOff     The tie off with referenced parameter UUIDs changed to names.
     *      @param [in] tieOffBase          The base number of the tie off.
     */
    void changeTieOffSymbolsInConnectedPorts(QString const& tieOffValue, QString const& parsedTieOff,
        QString const& formattedTieOff, int tieOffBase) const;

    /*!
     *  Draw a tie off symbol matching the given value.
     *
     *      @param [in] portItem            The port item whose symbol is being redrawn.
     *      @param [in] tieOffValue         The tie off expression.
     *      @param [in] parsedTieOff        The parsed tie off value.
     *      @param [in] formattedTieOff     The tie off with the referenced parameter UUIDs changed to names.
     *      @param [in] tieOffBase          The base number of the tie off.
     */
    void drawTieOffSymbol(AdHocItem* portItem, QString const& tieOffValue, QString parsedTieOff,
        QString formattedTieOff, int tieOffBase) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design diagram containing the related ad hoc port item.
    HWDesignDiagram* containingDiagram_;

    //! The old value for the tie off.
    QString oldTieOff_;

    //! Parsed value of the old tie off.
    QString parsedOldTieOff_;

    //! Old tie off with the referenced parameter UUIDs changed to parameter names.
    QString formattedOldTieOff_;

    //! The base number of the old tie off.
    int oldBase_;

    //! A new value for tie off.
    QString newTieOff_;

    //! Parsed value of the new tie off.
    QString parsedNewTieOff_;

    //! New tie off with the referenced parameter UUIDs changed to parameter names.
    QString formattedNewTieOff_;

    //! The base number of the new tie off.
    int newBase_;

    //! Formats values to the base number of the expression.
    ValueFormatter valueFormatter_;
};

#endif // ADHOCTIEOFFCHANGECOMMAND_H
