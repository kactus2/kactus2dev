/* 
 *  	Created on: 17.2.2012
 *      Author: Antti Kamppi
 * 		filename: segment.h
 *		Project: Kactus 2
 */


#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include "ipxactmodels_global.h"

#include <QString>
#include <QMap>

class VendorExtension;

/*! \brief Segment describes the location and size of an area in containing address space.
 *
 */
class IPXACTMODELS_EXPORT Segment : public NameGroup, public Extendable{

public:

	/*! \brief The default constructor
	 *
	*/
	Segment();
	
	//! \brief The destructor
	~Segment();

	//! \brief The copy constructor
	Segment(const Segment& other);

	//! \brief The assignment operator
	Segment& operator=(const Segment& other);

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

	/*! \brief Check if the address space is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the address space is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the address offset of the segment within the containing address space.
	 *
	 * \return QString contains the address offset.
	*/
	QString getAddressOffset() const;

	/*! \brief Get the range of the segment.
	 *
	 * \return QString Contains the number of addressable units the segment has.
	*/
	QString getRange() const;

	/*! \brief Get the attributes of the address offset.
	 *
	 * \return Reference to the QMap containing the attributes as name-value pairs.
	*/
	const QMap<QString, QString>& getOffsetAttributes() const;

	/*! \brief Get the attributes of the range.
	 *
	 * \return Reference to the QMap containing the attributes as name-value pairs.
	*/
	const QMap<QString, QString>& getRangeAttributes() const;


	/*! \brief Set the address offset for the segment.
	 *
	 * \param addressOffset The offset within the containing address space.
	 *
	*/
	void setOffset(const QString& addressOffset);

	/*! \brief Set the range for the segment.
	 *
	 * \param range The number of addressable units the segment contains.
	 *
	*/
	void setRange(const QString& range);

	/*! \brief Set the attributes of the address offset.
	 *
	 * \param offsetAttributes QMap containing the attributes as name-value pairs.
	*/
	void setOffsetAttributes( QMap<QString, QString> offsetAttributes );

	/*! \brief Set the attributes of the range.
	 *
	 * \param rangeAttributes QMap containing the attributes as name-value pairs.
	*/
	void setRangeAttributes( QMap<QString, QString> rangeAttributes );

private:

	//! Presence of the CPU.
	QString isPresent_;

	//! \brief The offset of the segment from the beginning of the containing address space.
	QString addressOffset_;

	//! \brief The attributes for the offset
	QMap<QString, QString> offsetAttributes_;

	//! \brief The range of the segment in addressable units.
	QString range_;

	//! \brief The attributes for the range
	QMap<QString, QString> rangeAttributes_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Segment vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* SEGMENT_H_ */