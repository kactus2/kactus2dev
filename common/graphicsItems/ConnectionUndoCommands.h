//-----------------------------------------------------------------------------
// File: ConnectionUndoCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.6.2012
//
// Description:
// Undo commands for graphics connections.
//-----------------------------------------------------------------------------

#ifndef CONNECTIONUNDOCOMMANDS_H
#define CONNECTIONUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QPointF>

class GraphicsConnection;

//-----------------------------------------------------------------------------
//! ConnectionMoveCommand class.
//-----------------------------------------------------------------------------
class ConnectionMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn      The interconnection.
     *      @param [in] oldRoute  The old route of the interconnection.
     *      @param [in] parent    The parent command.
     */
    ConnectionMoveCommand(GraphicsConnection* connection, QList<QPointF> const& oldRoute,
                          QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionMoveCommand();

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
    ConnectionMoveCommand(ConnectionMoveCommand const& rhs);
    ConnectionMoveCommand& operator=(ConnectionMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection.
    GraphicsConnection* conn_;

    //! The old route of the connection.
    QList<QPointF> oldRoute_;

    //! The new route of the connection.
    QList<QPointF> newRoute_;
};

//-----------------------------------------------------------------------------
//! Undo command for changes made to a DiagramInterConnection.
//-----------------------------------------------------------------------------
class ConnectionChangeCommand : public QUndoCommand {

public:

	/*! \brief The constructor
	 *
	 * \param connection Pointer to the connection that was changed.
	 * \param newName The new name of the connection.
	 * \param newDescription The new description of the connection.
	 * \param parent Pointer to the parent of this undo command.
	 *
	*/
	ConnectionChangeCommand(GraphicsConnection* connection,
		                    const QString& newName,
		                    const QString& newDescription,
		                    QUndoCommand* parent = 0);

	//! \brief The destructor
	virtual ~ConnectionChangeCommand();

	/*! \brief Undoes the command.
	 *
	*/
	virtual void undo();

	/*! \brief Redoes the command.
	 *
	 * Redo is automatically executed when this command is added to the undo stack.
	*/
	virtual void redo();

private:
	//! \brief No copying
	ConnectionChangeCommand(const ConnectionChangeCommand& other);

	//! \brief No assignment
	ConnectionChangeCommand& operator=(const ConnectionChangeCommand& other);

	//! \brief Pointer to the connection to change.
	GraphicsConnection* connection_;

	//! \brief The new name for the connection.
	QString newName_;

	//! \brief The new description for the connection.
	QString newDescription_;

	//! \brief The old name of the connection.
	QString oldName_;

	//! \brief The old description of the connection.
	QString oldDescription_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIONUNDOCOMMANDS_H
