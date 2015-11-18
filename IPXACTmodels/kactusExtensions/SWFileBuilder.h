//-----------------------------------------------------------------------------
// File: SWFileBuilder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.10.2015
//
// Description:
// Specifies the build commands for software objects.
//-----------------------------------------------------------------------------

#ifndef SWFILEBUILDER_H
#define SWFILEBUILDER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/FileBuilder.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QString>

//-----------------------------------------------------------------------------
//! Specifies the build commands for software objects.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SWFileBuilder: public FileBuilder, public VendorExtension
{

public:

    //! The default constructor.	
    SWFileBuilder();

	/*!
	 *  The xml constructor.
	 *
	 *      @param [in] swBuildNode   The XML description of the software file builder.
     *
	 */
	SWFileBuilder(QDomNode const& swBuildNode);

	//! Copy constructor
	SWFileBuilder(const SWFileBuilder& other);

	//! Assignment operator
	SWFileBuilder& operator=(const SWFileBuilder& other);
    
    /*!
     *  Clones the file builder.
     *
     *      @return The clone copy of the file builder.
     */
    virtual VendorExtension* clone() const;

	//! The destructor
	virtual ~SWFileBuilder();

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

	/*! Write the contents of the class using the writer.
	*
	*      @param [in] writer   The writer to write the document into XML.
	*/
    virtual void write(QXmlStreamWriter& writer) const;

	/*! Check if the build command is in a valid state.
	*
	*      @param [in] errorList T          he list to add the possible error messages to.
	*      @param [in] parentIdentifier     String from parent to help to identify the location of the error.
	*
	*       @return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QVector<QString>& errorList, QString const& parentIdentifier) const;

	/*! Check if the build command is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

};

#endif /* SWFILEBUILDER_H */