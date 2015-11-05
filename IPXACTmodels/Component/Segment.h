//-----------------------------------------------------------------------------
// File: Segment.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.10.2015
//
// Description:
// Segment describes the location and size of an area in containing address space.
//-----------------------------------------------------------------------------

#ifndef SEGMENT_H
#define SEGMENT_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

// #include "ipxactmodels_global.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

// class VendorExtension;

//-----------------------------------------------------------------------------
//! Segment describes the location and size of an area in containing address space.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Segment : public NameGroup, public Extendable
{

public:

	/*!
	 *  The default constructor.
	 *
	 *      @param [in] name            Name of the segment.
	 *      @param [in] addressOffset   Address offset of the segment.
	 *      @param [in] range           Range of the segment.
	 */
	Segment(QString const& name = QString(""), QString const& addressOffset = QString(""),
        QString const& range = QString(""));
	
	//! The destructor.
	~Segment();

	//! The copy constructor.
	Segment(const Segment& other);

	//! The assignment operator.
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

	/*!
     *  Check if the address space is in a valid state.
	 *
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    String from parent to help to identify the location of the error.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	 */
// 	bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

	/*!
     *  Check if the address space is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	 */
// 	bool isValid() const;

	/*!
     *  Get the address offset of the segment within the containing address space.
	 *
	 *      @return QString contains the address offset.
	 */
	QString getAddressOffset() const;

    /*!
     *  Set the address offset for the segment.
	 *
	 *      @param [in] addressOffset   The offset within the containing address space.
	 */
	void setOffset(QString const& addressOffset);
    
	/*!
     *  Get the attributes of the address offset.
	 *
	 *      @return Reference to the QMap containing the attributes as name-value pairs.
	 */
	QMap<QString, QString> getOffsetAttributes() const;
    
	/*!
     *  Set the attributes of the address offset.
	 *
	 *      @param [in] offsetAttributes    QMap containing the attributes as name-value pairs.
	 */
	void setOffsetAttributes( QMap<QString, QString> offsetAttributes );

	/*!
     *  Get the range of the segment.
	 *
	 *      @return QString Contains the number of addressable units the segment has.
	 */
	QString getRange() const;
    
	/*!
     *  Set the range for the segment.
	 *
	 *      @param [in] range   The number of addressable units the segment contains.
	 */
	void setRange(QString const& range);

	/*!
     *  Get the attributes of the range.
	 *
	 *      @return Reference to the QMap containing the attributes as name-value pairs.
	 */
	QMap<QString, QString> getRangeAttributes() const;

	/*!
     *  Set the attributes of the range.
	 *
	 *      @param [in] rangeAttributes     QMap containing the attributes as name-value pairs.
	 */
	void setRangeAttributes( QMap<QString, QString> rangeAttributes );

private:

	//! Presence of the segment.
	QString isPresent_;

	//! The offset of the segment from the beginning of the containing address space.
	QString addressOffset_;

	//! The attributes for the offset
	QMap<QString, QString> offsetAttributes_;

	//! The range of the segment in addressable units.
	QString range_;

	//! The attributes for the range.
	QMap<QString, QString> rangeAttributes_;

    /*!
	 * OPTIONAL (ipxact: vendorExtensions)
	 * Segment vendor extensions.
	 */
//     QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif // SEGMENT_H