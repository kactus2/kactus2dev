//-----------------------------------------------------------------------------
// File: AdHocVisibilityChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for changing port adhoc visibility in design diagram.
//-----------------------------------------------------------------------------

#ifndef ADHOCVISIBILITYCHANGECOMMAND_H
#define ADHOCVISIBILITYCHANGECOMMAND_H

#include <QPointF>
#include <QUndoCommand>

class AdHocItem;
class AdHocEnabled;
class GraphicsConnection;

//-----------------------------------------------------------------------------
//! Undo command for changing port adhoc visibility in design diagram.
//-----------------------------------------------------------------------------
class AdHocVisibilityChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] dataSource    The ad-hoc enabled data source.
     *      @param [in] portName      The name of the port.
     *      @param [in] newVisiblity  The new ad-hoc visibility of the port.
     *      @param [in] parent        The parent undo command.
     */
    AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName, bool newVisibility,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AdHocVisibilityChangeCommand();

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
    AdHocVisibilityChangeCommand(AdHocVisibilityChangeCommand const& rhs);
    AdHocVisibilityChangeCommand& operator=(AdHocVisibilityChangeCommand const& rhs);

    /*!
     *  Create a delete command for a connected connection.
     *
     *      @param [in] connection  The selected connection.
     */
    void createConnectionDeleteCommand(GraphicsConnection* connection);

    /*!
     *  Change the visibility status of the selected port item.
     *
     *      @param [in] currentVisibility   The new visibility of the port item.
     */
    void changePortItemVisibility(bool currentVisibility);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component containing the port.
    AdHocEnabled* dataSource_;

    //! The name of the port.
    QString portName_;

    //! The saved port position.
    QPointF pos_;

    //! The new ad-hoc visibility for the port.
    bool newVisibility_;

    //! The selected ad hoc port item.
    AdHocItem* adhocPort_;
};

#endif // ADHOCVISIBILITYCHANGECOMMAND_H
