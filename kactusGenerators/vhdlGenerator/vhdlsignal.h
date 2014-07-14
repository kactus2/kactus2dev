/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.h
 *		Project: Kactus 2
 */

#ifndef VHDLSIGNAL_H
#define VHDLSIGNAL_H

#include <kactusGenerators/HDLGenerator/HDLSignal.h>

class VhdlSignal : public HDLSignal {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * @param [in] parent       Pointer to the owner of this vhdl object.
	 * @param [in] name         Name for this vhdl object.
	 * @param [in] type         Type for this vhdl object.
	 * @param [in] leftBound    The left bound for the signal.
	 * @param [in] rightBound   The right bound for the signal.
	 * @param [in] description  The description for this vhdl object.
	 * @param [in] defaultValue The default value for the signal.
	 *
	*/
	VhdlSignal(QObject* parent, 
		QString const& name = QString(),
		QString const& signalType = QString(),
		int leftBound = -1,
		int rightBound = -1,
		QString const& description = QString(),
		QString const& defaultValue = QString());

	
	//! The destructor
	virtual ~VhdlSignal();

	/*! Write the declaration of the signal to the text stream.
	 *
	 * @param [in] stream   The text stream to write the signal declaration into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

	/*! Set bounds for the signal.
	 *
	 * @param [in] left     The left bound for signal.
	 * @param [in] right    The right bound for signal.
	 *
	*/
	virtual void setBounds(int left, int right);

private:
	//! No copying
	VhdlSignal(const VhdlSignal& other);

	//! No assignment
	VhdlSignal& operator=(const VhdlSignal& other);
};

#endif // VHDLSIGNAL_H
