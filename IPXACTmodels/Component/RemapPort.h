//-----------------------------------------------------------------------------
// File: RemapPort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:remapPort element.
//-----------------------------------------------------------------------------

#ifndef REMAPPORT_H
#define REMAPPORT_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>

//-----------------------------------------------------------------------------
//! Describes the ipxact:remapPort element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RemapPort
{

public:

    /*!
     *  The default constructor.
     */
    RemapPort(QString const& portReference = QString());

	//! Copy constructor.
	RemapPort(const RemapPort &other);

    //! Assignment operator.
	RemapPort &operator=(const RemapPort &other);

	/*! 
     *  The destructor
	 */
	~RemapPort();

	/*!
     *  Get the value of the port reference.
	 *
	 *      @return The port reference.
	 */
	QString getPortNameRef() const;

	/*!
     *  Set the portNameRef for this remapPort.
	 *
	 *      @param [in] newPortNameRef  The new port reference.
	 */
	void setPortNameRef(QString const& newPortNameRef);
    
	/*!
     *  Get index of a port.
	 *
	 *      @return The port index.
	 */
    QString getPortIndex() const;

	/*!
     *  Set the port index.
	 *
	 *      @param [in] newPortIndex    The new port index.
	 */
    void setPortIndex(QString const& newPortIndex);

	/*!
     *  Get the value of the remapPort element.
	 *
	 *      @return The value of the remap port.
	 */
	QString getValue() const;

	/*! 
     *  Set the value for the remapPort element.
	 *
	 *      @param [in] value   The new value.
	 */
	void setValue(QString const& newValue);

private:

    //! The value that activates the remap state.
	QString value_;

    //! Name of the referenced port.
	QString portRef_;

    //! The index of the referenced port.
    QString portIndex_;
};

#endif // REMAPPORT_H
