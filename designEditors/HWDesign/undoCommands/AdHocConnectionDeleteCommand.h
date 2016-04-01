//-----------------------------------------------------------------------------
// File: AdHocConnectionDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.02.2016
//
// Description:
// Undo command for removing a AdHocConnection in design diagram.
//-----------------------------------------------------------------------------

#ifndef AdHocConnectionDELETECOMMAND_H
#define ADHOCAdHocConnectionDELETECOMMAND_H

#include <QObject>
#include <QSharedPointer>
#include <QUndoCommand>

#include <IPXACTmodels/generaldeclarations.h>

class DesignDiagram;
class AdHocConnectionItem;
class PortMap;

//-----------------------------------------------------------------------------
//! Undo command for removing a AdHocConnection in design diagram.
//-----------------------------------------------------------------------------
class AdHocConnectionDeleteCommand : public QUndoCommand
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] diagram             The diagram containing the AdHocConnection item.
     *      @param [in] adHocConnection     The AdHocConnection to remove.
     *      @param [in] parent              The parent command.
     */
    AdHocConnectionDeleteCommand(DesignDiagram* diagram, AdHocConnectionItem* AdHocConnection,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AdHocConnectionDeleteCommand();

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
    AdHocConnectionDeleteCommand(AdHocConnectionDeleteCommand const& rhs);
    AdHocConnectionDeleteCommand& operator=(AdHocConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection.
    AdHocConnectionItem* adHocConnection_;

    //! The port maps for the hierarchical end point (if any).
    QList< QSharedPointer<PortMap> > portMaps_;

    //! The graphics scene.
    DesignDiagram* diagram_;

    //! Boolean flag for indicating if the AdHocConnection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // AdHocConnectionDELETECOMMAND_H
