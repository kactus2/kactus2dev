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

#include <IPXACTmodels/Component/Region.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Segment describes the location and size of an area in containing address space.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Segment : public Region
{

public:

	/*!
	 *  The default constructor.
	 *
	 *    @param [in] name            Name of the segment.
	 *    @param [in] addressOffset   Address offset of the segment.
	 *    @param [in] range           Range of the segment.
	 */
	Segment(QString const& name = QString(), QString const& addressOffset = QString(),
        QString const& range = QString());
	
	//! The destructor.
	virtual ~Segment();

	//! The copy constructor.
	Segment(const Segment& other);

	//! The assignment operator.
	Segment& operator=(const Segment& other);

    /*!
     *  Gets the presence.
     *
     *    @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *    @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);
    
	/*!
     *  Get the attributes of the address offset.
	 *
	 *    @return Reference to the QMap containing the attributes as name-value pairs.
	 */
	QMap<QString, QString> getOffsetAttributes() const;
    
	/*!
     *  Set the attributes of the address offset.
	 *
	 *    @param [in] offsetAttributes    QMap containing the attributes as name-value pairs.
	 */
	void setOffsetAttributes( QMap<QString, QString> offsetAttributes );

	/*!
     *  Get the attributes of the range.
	 *
	 *    @return Reference to the QMap containing the attributes as name-value pairs.
	 */
	QMap<QString, QString> getRangeAttributes() const;

	/*!
     *  Set the attributes of the range.
	 *
	 *    @param [in] rangeAttributes     QMap containing the attributes as name-value pairs.
	 */
	void setRangeAttributes( QMap<QString, QString> rangeAttributes );

private:

	//! Presence of the segment.
	QString isPresent_;

	//! The attributes for the offset
	QMap<QString, QString> offsetAttributes_;

	//! The attributes for the range.
	QMap<QString, QString> rangeAttributes_;
};

Q_DECLARE_METATYPE(QSharedPointer<Segment>);

#endif // SEGMENT_H