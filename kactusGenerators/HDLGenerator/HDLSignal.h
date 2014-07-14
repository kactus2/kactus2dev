//-----------------------------------------------------------------------------
// File: HDLSignal.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDL signal/wire implementations.
//-----------------------------------------------------------------------------

#ifndef HDLSignal_H
#define HDLSignal_H

#include "HDLObject.h"

//-----------------------------------------------------------------------------
//! Base class for HDL signal/wire implementations.
//-----------------------------------------------------------------------------
class HDLSignal : public HDLObject {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent       The parent object.
	 *      @param [in] name         Name of the signal.
	 *      @param [in] type         Type of the signal.
	 *      @param [in] leftBound    The left bound of the signal.
	 *      @param [in] rightBound   The right bound of the signal.
	 *      @param [in] description  The description of this signal.
	 *      @param [in] defaultValue The default value of the signal.	 
	 */
	HDLSignal(QObject* parent, 
		const QString& name = QString(),
		const QString& signalType = QString(),
		int leftBound = -1,
		int rightBound = -1,
		const QString& description = QString(),
		const QString& defaultValue = QString());

	//! The destructor
	virtual ~HDLSignal();

	/*! Get the left bound of the signal
	 *
	 *      @return int the left bound.
	*/
	int left() const;

	/*! Set the left bound of the signal.
	 *
	 *      @param [in] left The left bound to set.
	 *
	*/
	void setLeft(int left);

	/*! Get the right bound of the signal.
	 *
	 *      @return int the right bound
	*/
	int right() const;

	/*! Set the right bound of the signal.
	 *
	 *      @param [in] right The right bound to set.
	 *
	*/
	void setRight(int right);

	/*! Set bounds for the signal.
	 *
	 *      @param [in] left The left bound for signal.
	 *      @param [in] right The right bound for signal.
	 *
	*/
	virtual void setBounds(int left, int right);

private:
	//! No copying
	HDLSignal(const HDLSignal& other);

	//! No assignment
	HDLSignal& operator=(const HDLSignal& other);

	//! The left bound of the signal.
	int left_;

	//! The right bound of the signal.
	int right_;
};

#endif // HDLSignal_H
