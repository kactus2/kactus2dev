//-----------------------------------------------------------------------------
// File: PortSlice.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Describes the ipxact:portSlice element.
//-----------------------------------------------------------------------------

#ifndef PORTSLICE_H
#define PORTSLICE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/PartSelect.h>

#include <QString>
#include <QList>

//-----------------------------------------------------------------------------
//! Describes the ipxact:PortSlice element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortSlice : public NameGroup
{

public:

    /*!
     *  The default constructor.
     */
    PortSlice(QString const& name = QString());

	//! Copy constructor.
	PortSlice(const PortSlice &other);

    //! Assignment operator.
	PortSlice &operator=(const PortSlice &other);

	/*! 
     *  The destructor
	 */
	~PortSlice();

	/*!
     *  Get the value of the port reference.
	 *
	 *      @return The port reference.
	 */
	QString getPortRef() const;

	/*!
     *  Set the portNameRef for this PortSlice.
	 *
	 *      @param [in] newPortNameRef  The new port reference.
	 */
	void setPortRef(QString const& newPortNameRef);
    

	QSharedPointer<PartSelect> getPartSelect() const;

	void setPartSelect(QSharedPointer<PartSelect> partSelect);

    /*!
     *  Get the left value of the range.
     *
     *      @return The left value of the range.
     */
    QString getLeftRange() const;

    /*!
     *  Get the right value of the range.
     *
     *      @return The right value of the range.
     */
    QString getRightRange() const;

    /*!
     *  Set the left value for the range.
     *
     *      @param [in] newLeftRange    The new value for the left value of the range.
     */
    void setLeftRange(QString const& newLeftRange);

    /*!
     *  Set the right value for the range.
     *
     *      @param [in] newRightRange   The new value for the right value of the range.
     */
    void setRightRange(QString const& newRightRange);

private:

    //! Name of the referenced port.
	QString portRef_;

	//! The part select for the port.
	QSharedPointer<PartSelect> partSelect_ = nullptr;
};

#endif // PORTSLICE_H
