/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlconnectionendpoint.h
 *		Project: Kactus 2
 */

#ifndef VHDLCONNECTIONENDPOINT_H
#define VHDLCONNECTIONENDPOINT_H

#include <QString>

/*! Used to identify end points for a signal.
 * 
 * This class is used when parsing the connections between component instance 
 * ports. One end point represents one port or part of it.
 */
class VhdlConnectionEndPoint {

public:

	/*! The constructor
	 *
	*/
	VhdlConnectionEndPoint();

	/*! The constructor
	 *
	 * @param [in] instanceName The name of the instance for the end point.
	 * @param [in] portName The name of the port for the end point.
	 * @param [in] portLeft The left bound of the port.
	 * @param [in] portRight The right bound of the port.
	 * @param [in] signalLeft The left bound of the signal.
	 * @param [in] signalRight The right bound of the signal.
	 *
	*/
	VhdlConnectionEndPoint(const QString& instanceName,	
		const QString& portName, 
		const QString& portLeft = "-1",
		const QString& portRight = "-1",
		const QString& signalLeft = "-1",
		const QString& signalRight = "-1");

	//! Copy constructor
	VhdlConnectionEndPoint(const VhdlConnectionEndPoint& other) = default;

	//! Assignment operator
	VhdlConnectionEndPoint& operator=(const VhdlConnectionEndPoint& other) = default;

	//! The == operator
	bool operator==(const VhdlConnectionEndPoint& other) const;

	//! The != operator
	bool operator!=(const VhdlConnectionEndPoint& other) const;

	//! The < operator
	bool operator<(const VhdlConnectionEndPoint& other) const;

	//! The > operator
	bool operator>(const VhdlConnectionEndPoint& other) const;

	//! The destructor
	virtual ~VhdlConnectionEndPoint() = default;

	/*! Get the instance name of the end point.
	 *
	 * @return QString contains the instance name.
	*/
	QString instanceName() const;

	/*! Get the port name of the end point.
	 *
	 * @return QString contains the port name.
	*/
	QString portName() const;

	/*! Get the left bound of the signal.
	 *
	 * @return int The left bound.
	*/
	QString signalLeft() const;

	/*! Get the right bound of the signal.
	 *
	 * @return int The right bound.
	*/
	QString signalRight() const;

	/*! Get the left bound of the port.
	 *
	 * @return int The left bound.
	*/
	QString portLeft() const;

	/*! Get the right bound of the port.
	 *
	 * @return int The right bound.
	*/
	QString portRight() const;

private:

	//! The name of the component instance for the end point.
	QString instanceName_;

	//! The name of the port for the end point.
	QString portName_;

	//! The left bound used from the signal.
	QString signalLeft_;

	//! The right bound used from the signal.
	QString signalRight_;

	//! The left bound used from the port.
	QString portLeft_;

	//! THe right bound used from the port.
	QString portRight_;

};

#endif // VHDLCONNECTIONENDPOINT_H
