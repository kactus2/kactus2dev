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
	 * \param portLeft The left bound of the port.
	 * \param portRight The right bound of the port.
	 * \param signalLeft The left bound of the signal.
	 * \param signalRight The right bound of the signal.
	 *
	*/
	VhdlConnectionEndPoint(const QString& instanceName,	
		const QString& portName, 
		const QString& portLeft = "-1",
		const QString& portRight = "-1",
		const QString& signalLeft = "-1",
		const QString& signalRight = "-1");

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
	QString signalLeft() const;

	/*! \brief Get the right bound of the signal.
	 *
	 * \return int The right bound.
	*/
	QString signalRight() const;

	/*! \brief Get the left bound of the port.
	 *
	 * \return int The left bound.
	*/
	QString portLeft() const;

	/*! \brief Get the right bound of the port.
	 *
	 * \return int The right bound.
	*/
	QString portRight() const;

private:

	//! \brief The name of the component instance for the end point.
	QString instanceName_;

	//! \brief The name of the port for the end point.
	QString portName_;

	//! \brief The left bound used from the signal.
	QString signalLeft_;

	//! \brief The right bound used from the signal.
	QString signalRight_;

	//! \brief The left bound used from the port.
	QString portLeft_;

	//! \brief THe right bound used from the port.
	QString portRight_;

};

#endif // VHDLCONNECTIONENDPOINT_H
