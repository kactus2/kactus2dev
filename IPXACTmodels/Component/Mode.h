//-----------------------------------------------------------------------------
// File: Mode.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Describes the ipxact:mode element.
//-----------------------------------------------------------------------------

#ifndef MODE_H
#define MODE_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/Component/FieldSlice.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include "PortSlice.h"

#include <QString>
#include <QMap>


//-----------------------------------------------------------------------------
//! Mode describes the location and size of an area in memory.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Mode : public NameGroup, public Extendable
{

public:

	using List = QSharedPointer<QList<QSharedPointer<Mode> > >;

	/*!
	 *  The default constructor.
	 *
	 *    @param [in] name	Name of the mode.
	 */
	Mode(QString const& name = QString());
	
	//! The destructor.
	virtual ~Mode();

	//! The copy constructor.
	Mode(const Mode& other);

	//! The assignment operator.
	Mode& operator=(const Mode& other);

	/*!
	 * Get the condition expression.
	 *
	 *    @return The condition expression.
	 */
	QString getCondition() const;

	/*!
	 *  Sets the condition expression.
	 *
	 *    @param [in] conditionExpression The expression to set.
	 */
	void setCondition(QString const& conditionExpression);

	/*!
	 *  Get the port slices.
	 *
	 *    @return The port slices.
	 */
	PortSlice::List getPortSlices() const;


	QSharedPointer<QList<QSharedPointer<FieldSlice> > > getFieldSlices() const;

private:

	//! The condition to activate the mode.
	QString condition_;

	//! The port slices available in the condition.
	PortSlice::List portSlices_ = PortSlice::List(new QList<QSharedPointer<PortSlice> >());

    //! The field slices available in the condition.
	FieldSlice::List fieldSlices_ = FieldSlice::List(new QList<QSharedPointer<FieldSlice> >());

};

#endif // MODE_H