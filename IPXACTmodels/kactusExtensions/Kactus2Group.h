//-----------------------------------------------------------------------------
// File: Kactus2Group.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.5.2014
//
// Description:
// Kactus2 composite of vendor extensions.
//-----------------------------------------------------------------------------

#ifndef KACTUS2GROUP_H
#define KACTUS2GROUP_H

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

//-----------------------------------------------------------------------------
// class Kactus2Placeholder.
//-----------------------------------------------------------------------------
class Kactus2Group : public Kactus2Placeholder
{
public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] name   The name of the placeholder.
	 */
    Kactus2Group(QString name);

    //! Copy constructor.
    Kactus2Group(Kactus2Group const& other);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Group();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Group* clone() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

private:

	//! Disable assignment.
	Kactus2Group& operator=(Kactus2Group const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

};
#endif // KACTUS2GROUP_H
