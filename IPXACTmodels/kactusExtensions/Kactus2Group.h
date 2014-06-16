//-----------------------------------------------------------------------------
// File: Kactus2Group.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.06.2014
//
// Description:
// Kactus2 vendor extension for a group of extensions.
//-----------------------------------------------------------------------------

#ifndef KACTUS2GROUP_H
#define KACTUS2GROUP_H

#include <IPXACTmodels/VendorExtension.h>

#include <QMap>

//-----------------------------------------------------------------------------
// class Kactus2Group.
//-----------------------------------------------------------------------------
class Kactus2Group : public VendorExtension
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
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Adds a vendor extension to the group.
     *
     *      @param [in] extension   The extension to add.
     */
    void addToGroup(QSharedPointer<VendorExtension> extension);

    /*!
     *  Removes a vendor extension from the group.
     *
     *      @param [in] extension   The extension to remove.     
     */
    void removeFromGroup(QSharedPointer<VendorExtension> extension);

    /*!
     *  Gets all the extensions with the given type.
     *
     *      @param [in] type   The type to search for.
     *
     *      @return The vendor extensions with the given type.
     */
    QList<QSharedPointer<VendorExtension> > getByType(QString const& type) const;

private:
	//! Disable assignment.
	Kactus2Group& operator=(Kactus2Group const& rhs);

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! Name of the group.
    QString name_;

    //! The extensions composing the group.
    QList<QSharedPointer<VendorExtension> > groupExtensions_;

};
#endif // KACTUS2GROUP_H
