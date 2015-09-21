/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <IPXACTmodels/common/NameGroup.h>

#include <QString>
#include <QStringList>

/*! \brief Equals the spirit:channel element in IP-Xact specification
 *
 * This struct contains a list of all the mirrored bus interfaces in the
 * containing component that belong to the same channel
 */
class IPXACTMODELS_EXPORT Channel : public NameGroup {

public:

	/*! \brief The default constructor
	 *
	 * Constructs an empty channel which is in invalid state.
	*/
	Channel();

	//! Copy constructor
	Channel(const Channel& other);

	//! Assignment operator
	Channel& operator=(const Channel& other);

	//! The destructor
	~Channel();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*! \brief Check if the channel is in a valid state.
	 *
	 * \param interfaceNames Contains the names of the bus interfaces in the component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& interfaceNames,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the channel is in a valid state.
	 * 
	 * \param interfaceNames Contains the names of the bus interfaces in the component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& interfaceNames) const;

	/*! \brief Get the references to bus interfaces that belong to this channel.
	 *
	 * \return Reference to QStringList containing the interface references.
	*/
	const QStringList& getInterfaces() const;

	/*! \brief Check if the channel contains the specified interface.
	 *
	 * Method: 		containsInterface
	 * Full name:	Channel::containsInterface
	 * Access:		public 
	 *
	 * \param interfaceName Identifies the bus interface searched for.
	 *
	 * \return True if the channel contains the bus interface.
	*/
	bool containsInterface(const QString& interfaceName) const;

	/*! \brief Set the interfaces that belong to this channel.
	 *
	 * \param interfaceNames QStringList containing the references to interfaces.
	 *
	*/
	void setInterfaces(const QStringList& interfaceNames);


private:
	//! Presence of the channel.
	QString isPresent_;

	//! \brief List of references to mirrored bus interfaces.
	QStringList busInterfaces_;

};

#endif /* CHANNEL_H_ */
