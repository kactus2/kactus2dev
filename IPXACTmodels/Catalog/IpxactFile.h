//-----------------------------------------------------------------------------
// File: IpxactFile.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Implementation for ipxact:ipxactFile element.
//-----------------------------------------------------------------------------

#ifndef IPXACTFILE_H
#define IPXACTFILE_H


#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/ipxactmodels_global.h>
 
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:ipxactFile element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT IpxactFile: public Extendable
{
public:

	//! The default constructor.
	IpxactFile();

	//! Copy constructor.
	IpxactFile(IpxactFile const& other);

    //! Move constructor.
    IpxactFile(IpxactFile&& other) = default;

	//! Assignment operator.
	IpxactFile &operator=(IpxactFile const& other);

    //! Move assignment operator.
    IpxactFile &operator=(IpxactFile&& other) = default;

	//! The destructor.
	virtual ~IpxactFile() = default;

	/*! Set the VLNV for the file.
	*
	*      @param [in] vlnv     The vlnv to set.
	*/
	void setVlnv(const VLNV& vlnv);
    	
    /*! Get the VLNV for the file.
	 *
	 *      @return The vlnv referenced in the file.
	 */
    VLNV getVlnv() const;
    	
    /*! Set the name (path) for the file.
	 *
	 *       @param [in] name   The name to set.
	 */
    void setName(QString const& name);
    	
    /*! Get the name (path) for the file.
	 *
	 *      @return The path to the xml file.
	 */
    QString getName() const;
    	
    /*! Set the description for the file.
	 *
	 *       @param [in] description   The description to set.
	 */
    void setDescription(QString const& description);
    	
    /*! Get the description for the file.
	 *
	 *       @return   The description for the file.
	 */
    QString getDescription() const;

private:

    //! The VLNV referenced in the file.
    VLNV vlnv_;

    //! Path to the XML file.
    QString name_;

    //! Description of the XML file.
    QString description_;
};

#endif // IPXACTFILE_H
