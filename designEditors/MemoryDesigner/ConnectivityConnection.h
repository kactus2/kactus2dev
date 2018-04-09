//-----------------------------------------------------------------------------
// File: ConnectivityConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A connection in a connectivity graph.
//-----------------------------------------------------------------------------

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSharedPointer>
#include <QString>

class ConnectivityInterface;

//-----------------------------------------------------------------------------
//! A connection in a connectivity graph.
//-----------------------------------------------------------------------------
class ConnectivityConnection 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name        The name of the connection.
	 *      @param [in] startPoint  The starting interface in the connection.
	 *      @param [in] endPoint    The end interface in the connection.
	 */
    ConnectivityConnection(QString const& name, QSharedPointer<ConnectivityInterface const> startPoint,
        QSharedPointer<ConnectivityInterface const> endPoint);

	/*!
     *  The destructor.
     */
	~ConnectivityConnection();

    /*!
     *  Gets the name of the connection.
     *
     *      @return The name of the connection.
     */
    QString getName() const;

    /*!
     *  Gets the first interface in the connection.
     *
     *      @return The first interface in the connection.
     */
    QSharedPointer<ConnectivityInterface const> getFirstInterface() const;

    /*!
     *  Gets the second interface in the connection.
     *
     *      @return The second interface in the connection.
     */
    QSharedPointer<ConnectivityInterface const> getSecondInterface() const;

private:

	// Disable copying.
	ConnectivityConnection(ConnectivityConnection const& rhs);
	ConnectivityConnection& operator=(ConnectivityConnection const& rhs);

    //! The name of the connection.
    QString name_;

    //! The first interface in the connection.
    QSharedPointer<ConnectivityInterface const> firstInterface_;

    //! The second interface in the connection.
    QSharedPointer<ConnectivityInterface const> secondInterface_;
};

#endif // CONNECTION_H
