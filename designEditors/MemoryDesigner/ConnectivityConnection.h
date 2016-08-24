//-----------------------------------------------------------------------------
// File: ConnectivityConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
	ConnectivityConnection(QString const& name, QSharedPointer<ConnectivityInterface> startPoint, QSharedPointer<ConnectivityInterface> endPoint);

	//! The destructor.
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
    QSharedPointer<ConnectivityInterface> getFirstInterface() const;

    /*!
     *  Gets the second interface in the connection.
     *
     *      @return The second interface in the connection.
     */
    QSharedPointer<ConnectivityInterface> getSecondInterface() const;

private:

	// Disable copying.
	ConnectivityConnection(ConnectivityConnection const& rhs);
	ConnectivityConnection& operator=(ConnectivityConnection const& rhs);

    //! The name of the connection.
    QString name_;

    //! The first interface in the connection.
    QSharedPointer<ConnectivityInterface> firstInterface_;

    //! The second interface in the connection.
    QSharedPointer<ConnectivityInterface> secondInterface_;
};

#endif // CONNECTION_H
