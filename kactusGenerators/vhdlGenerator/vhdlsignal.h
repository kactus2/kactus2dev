/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlsignal.h
 *		Project: Kactus 2
 */

#ifndef VHDLSIGNAL_H
#define VHDLSIGNAL_H

#include "vhdlobject.h"

class VhdlSignal : public VhdlObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl object.
	 * \param name Name for this vhdl object.
	 * \param type Type for this vhdl object.
	 * \param leftBound The left bound for the signal.
	 * \param rightBound The right bound for the signal.
	 * \param description The description for this vhdl object.
	 * \param defaultValue The default value for the signal.
	 *
	*/
	VhdlSignal(QObject* parent, 
		const QString& name = QString(),
		const QString& signalType = QString(),
		int leftBound = -1,
		int rightBound = -1,
		const QString& description = QString(),
		const QString& defaultValue = QString());

	
	//! \brief The destructor
	virtual ~VhdlSignal();

	/*! \brief Write the declaration of the signal to the text stream.
	 *
	 * \param stream The text stream to write the signal declaration into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

	/*! \brief Get the left bound of the signal
	 *
	 *
	 * \return int the left bound.
	*/
	int left() const;

	/*! \brief Set the left bound of the signal.
	 *
	 * \param left The left bound to set.
	 *
	*/
	void setLeft(int left);

	/*! \brief Get the right bound of the signal.
	 *
	 *
	 * \return int the right bound
	*/
	int right() const;

	/*! \brief Set the right bound of the signal.
	 *
	 * \param right The right bound to set.
	 *
	*/
	void setRight(int right);

	/*! \brief Set bounds for the signal.
	 *
	 * \param left The left bound for signal.
	 * \param right The right bound for signal.
	 *
	*/
	void setBounds(int left, int right);

private:
	//! \brief No copying
	VhdlSignal(const VhdlSignal& other);

	//! \brief No assignment
	VhdlSignal& operator=(const VhdlSignal& other);

	//! \brief The left bound of the signal.
	int left_;

	//! \brief The right bound of the signal.
	int right_;
};

#endif // VHDLSIGNAL_H
