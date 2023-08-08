//-----------------------------------------------------------------------------
// File: Mode.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Mode describes the location and size of an area in memory.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Mode : public NameGroup, public Extendable
{

public:

	/*!
	 *  The default constructor.
	 *
	 *      @param [in] name	Name of the mode.
	 */
	Mode(QString const& name = QString());
	
	//! The destructor.
	virtual ~Mode();

	//! The copy constructor.
	Mode(const Mode& other);

	//! The assignment operator.
	Mode& operator=(const Mode& other);


private:


};

#endif // MODE_H