/* 
 *  	Created on: 17.2.2012
 *      Author: Antti Kamppi
 * 		filename: segment.h
 *		Project: Kactus 2
 */


#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "generaldeclarations.h"

#include <common/Global.h>

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QMap>

class VendorExtension;

/*! \brief Segment describes the location and size of an area in containing address space.
 *
 */
class KACTUS2_API Segment {

public:

	/*! \brief The constructor
	 *
	 * \param addrSpaceNode The node that contains the details of the segment.
	 *
	*/
	Segment(QDomNode &segmentNode);

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

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to writer the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

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

	/*! \brief Get the name of the segment.
	 *
	 * \return QString contains the segment name.
	*/
	QString getName() const;

	/*! \brief Get the display name of the segment.
	 *
	 * \return QString contains the display name of the segment.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the segment.
	 *
	 * \return QString contains the description of the segment.
	*/
	QString getDescription() const;

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

	/*! \brief Get the last address contained in the segment.
	 * 
	 * The last address is offset + range - 1.
	 * If offset and range are undefined then 0 is returned.
	 * 
	 * \return The last address contained in the segment.
	*/
	quint64 getLastAddress() const;

	/*! \brief Set the name for the segment.
	 *
	 * \param name The name for the segment.
	 *
	*/
	void setName(const QString& name);
	
	/*! \brief Set the display name for the segment.
	 *
	 * \param dispName The display name for the segment.
	 *
	*/
	void setDisplayName(const QString& dispName);

	/*! \brief Set the description for the segment.
	 *
	 * \param description The description for the segment.
	 *
	*/
	void setDescription(const QString& description);

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

private:

	//! \brief Contains the name and description of the segment
	General::NameGroup nameGroup_;

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