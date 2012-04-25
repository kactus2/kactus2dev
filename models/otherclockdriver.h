/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#ifndef OTHERCLOCKDRIVER_H_
#define OTHERCLOCKDRIVER_H_

#include "generaldeclarations.h"

#include <QDomNode>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:otherClockDriver element in IP-Xact specification
 *
 * Consists of a number of sub-elements which define the format of the clock
 * waveform.
 */
class OtherClockDriver {

public:

	/*! \brief The constructor
	 *
	 * \param clockNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	OtherClockDriver(QDomNode &clockNode);

	/*! \brief The default constructor.
	 * 
	 * Constructs an empty element which is in invalid state.
	 *
	*/
	OtherClockDriver();

	//! \brief Copy constructor
	OtherClockDriver(const OtherClockDriver &other);

	//! \brief Assignment operator
	OtherClockDriver &operator=(const OtherClockDriver &other);

	/*! \brief The destructor
	 *
	 */
	~OtherClockDriver();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the other clock driver is in a valid state.
	*
	* \param errorList The list to add the possible error messages to.
	* \param parentIdentifier String from parent to help to identify the location of the error.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the OtherClockDriver-element is in valid state.
	*
	* \return True if the instance is in valid state.
	*/
	bool isValid() const;

	/*! \brief Get the name of the clock
	 *
	 * \return QString containing the name
	 */
	QString getClockName() const;

	/*! \brief Get the clock period
	 *
	 * \return A pointer to the struct containing the clock period info
	 */
	General::ClockStruct *getClockPeriod();

	/*! \brief Get the clock pulse duration
	 *
	 * \return A pointer to the struct containing the clock pulse duration.
	 */
	General::ClockStruct *getClockPulseDuration();

	/*! \brief Get the clock pulse offset
	 *
	 * \return A pointer to the struct containing the clock pulse offset.
	 */
	General::ClockStruct *getClockPulseOffset();

	/*! \brief Get the clock pulse value
	 *
	 * \return A pointer to the struct containing the clock pulse value.
	 */
	General::ClockPulseValue *getClockPulseValue();

	/*! \brief Get the clock source
	 *
	 * \return QString containing the path and name of the clock generation
	 * cell.
	 */
	QString getClockSource() const;

	/*! \brief Set the clock name
	 *
	 * \param clockName Set the name of the clock name.
	 */
	void setClockName(const QString &clockName);

	/*! \brief Set the clock period
	 *
	 * \param clockPeriod A pointer to the struct containing the clock period.
	 */
	void setClockPeriod(General::ClockStruct *clockPeriod);

	/*! \brief Set the clock pulse duration
	 *
	 * \param clockPulseDuration A pointer to the struct containing the
	 * clock pulse duration.
	 */
	void setClockPulseDuration(General::ClockStruct *clockPulseDuration);

	/*! \brief Set the clock pulse offset
	 *
	 * \param clockPulseOffset A pointer to the struct containing the clock
	 * pulse offset.
	 */
	void setClockPulseOffset(General::ClockStruct *clockPulseOffset);

	/*! \brief Set the clock pulse value
	 *
	 * \param A pointer to the struct containing the clock pulse value.
	 */
	void setClockPulseValue(General::ClockPulseValue *clockPulseValue);

	/*! \brief Set the clock source
	 *
	 * \param clockSource QString containing the name and path of the clock
	 * generation cell.
	 */
	void setClockSource(const QString &clockSource);

private:

	/*!
	 * MANDATORY spirit:clockName
	 * The name of the clock for reference by a constraint
	 */
	QString clockName_;

	/*!
	 * OPTIONAL spirit:clockSource
	 * Defines the physical path and name of the clock generation cell
	 */
	QString clockSource_;

	/*!
	 * MANDATORY spirit:clockPeriod
	 * Specifies the overall length of one cycle of the waveform.
	 */
	QSharedPointer<General::ClockStruct> clockPeriod_;

	/*!
	 * MANDATORY spirit:clockPulseOffset
	 * Specifies the time delay from the start of the waveform to the first
	 * transition.
	 */
	QSharedPointer<General::ClockStruct> clockPulseOffset_;

	/*!
	 * MANDATORY spirit:clockPulseValue
	 * Specifies the logic value to which the port transitions
	 */
	QSharedPointer<General::ClockPulseValue> clockPulseValue_;

	/*!
	 * MANDATORY spirit:clockPulseDuration
	 * Specifies how long the waveform remains at the value specified by
	 * clockPulseValue
	 */
	QSharedPointer<General::ClockStruct> clockPulseDuration_;

};

#endif /* OTHERCLOCKDRIVER_H_ */
