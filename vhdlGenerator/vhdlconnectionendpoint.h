/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlconnectionendpoint.h
 *		Project: Kactus 2
 */

#ifndef VHDLCONNECTIONENDPOINT_H
#define VHDLCONNECTIONENDPOINT_H

#include <QString>

/*! \brief Used to identify end points for a signal.
 * 
 * This class is used when parsing the connections between component instance 
 * ports. One end point represents one port or part of it.
 */
class VhdlConnectionEndPoint {

public:

	/*! \brief The constructor
	 *
	*/
	VhdlConnectionEndPoint();

	/*! \brief The constructor
	 *
	 * \param instanceName The name of the instance for the end point.
	 * \param portName The name of the port for the end point.
	 *
	*/
	VhdlConnectionEndPoint(const QString& instanceName,	const QString& portName);

	//! \brief Copy constructor
	VhdlConnectionEndPoint(const VhdlConnectionEndPoint& other);

	//! \brief Assignment operator
	VhdlConnectionEndPoint& operator=(const VhdlConnectionEndPoint& other);

	//! \brief The == operator
	bool operator==(const VhdlConnectionEndPoint& other) const;

	//! \brief The != operator
	bool operator!=(const VhdlConnectionEndPoint& other) const;

	//! \brief The < operator
	bool operator<(const VhdlConnectionEndPoint& other) const;

	//! \brief The > operator
	bool operator>(const VhdlConnectionEndPoint& other) const;

	//! \brief The destructor
	virtual ~VhdlConnectionEndPoint();

	/*! \brief Get the instance name of the end point.
	 *
	 * \return QString contains the instance name.
	*/
	QString instanceName() const;

	/*! \brief Get the port name of the end point.
	 *
	 * \return QString contains the port name.
	*/
	QString portName() const;

	/*! \brief Get the left bound of the signal.
	 *
	 * \return int The left bound.
	*/
	int signalLeft() const;

	/*! \brief Get the right bound of the signal.
	 *
	 * \return int The right bound.
	*/
	int signalRight() const;

	/*! \brief Get the left bound of the port.
	 *
	 * \return int The left bound.
	*/
	int portLeft() const;

	/*! \brief Get the right bound of the port.
	 *
	 * \return int The right bound.
	*/
	int portRight() const;

private:

	//! \brief The name of the component instance for the end point.
	QString instanceName_;

	//! \brief The name of the port for the end point.
	QString portName_;

	//! \brief The left bound used from the signal.
	int signalLeft_;

	//! \brief The right bound used from the signal.
	int signalRight_;

	//! \brief The left bound used from the port.
	int portLeft_;

	//! \brief THe right bound used from the port.
	int portRight_;

};

#endif // VHDLCONNECTIONENDPOINT_H
