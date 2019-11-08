/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.h
 *		Project: Kactus 2
 */

#ifndef VHDLSIGNAL_H
#define VHDLSIGNAL_H

#include "VhdlTypedObject.h"

class VhdlSignal : public VhdlTypedObject
{

public:

	/*! The constructor
	 *
	 * @param [in] parent Pointer to the owner of this vhdl object.
	 * @param [in] name Name for this vhdl object.
	 * @param [in] type Type for this vhdl object.
	 * @param [in] leftBound The left bound for the signal.
	 * @param [in] rightBound The right bound for the signal.
	 * @param [in] description The description for this vhdl object.
	 * @param [in] defaultValue The default value for the signal.
	 *
	*/
	VhdlSignal(const QString& name = QString(),
		const QString& signalType = QString(),
		int leftBound = -1,
		int rightBound = -1,
		const QString& description = QString(),
		const QString& defaultValue = QString());

	
	//! The destructor
	virtual ~VhdlSignal() = default;

	/*! Write the declaration of the signal to the text stream.
	 *
	 * @param [in] stream The text stream to write the signal declaration into.
	 *
	*/
	virtual void write(QTextStream& stream) const override final;

	/*! Get the left bound of the signal
	 *
	 *
	 * @return int the left bound.
	*/
	int left() const;

	/*! Set the left bound of the signal.
	 *
	 * @param [in] left The left bound to set.
	 *
	*/
	void setLeft(int left);

	/*! Get the right bound of the signal.
	 *
	 *
	 * @return int the right bound
	*/
	int right() const;

	/*! Set the right bound of the signal.
	 *
	 * @param [in] right The right bound to set.
	 *
	*/
	void setRight(int right);

	/*! Set bounds for the signal.
	 *
	 * @param [in] left The left bound for signal.
	 * @param [in] right The right bound for signal.
	 *
	*/
	void setBounds(int left, int right);

private:
	//! No copying
	VhdlSignal(const VhdlSignal& other);

	//! No assignment
	VhdlSignal& operator=(const VhdlSignal& other);

	//! The left bound of the signal.
	int left_;

	//! The right bound of the signal.
	int right_;
};

#endif // VHDLSIGNAL_H
