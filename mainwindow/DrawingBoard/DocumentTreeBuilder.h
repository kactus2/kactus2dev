//-----------------------------------------------------------------------------
// File: DocumentTreeBuilder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.05.2015
//
// Description:
// Creates an object tree from IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef DOCUMNENTREEBUILDER_H
#define DOCUMNENTREEBUILDER_H

#include <IPXACTmodels/vlnv.h>

#include <QList>

class DesignConfiguration;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Creates an object tree from IP-XACT documents.
//-----------------------------------------------------------------------------
class DocumentTreeBuilder 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library   The current IP-XACT library.
	 */
	DocumentTreeBuilder(LibraryInterface* library);

	//! The destructor.
	~DocumentTreeBuilder();

    /*!
     *  Creates an object tree starting at the given document.
     *
     *      @param [in] root   The VLNV of the root document.
     *
     *      @return The object tree representing the document tree.
     */
    QObject* createFrom(VLNV const& root) const;

private:

	// Disable copying.
	DocumentTreeBuilder(DocumentTreeBuilder const& rhs);
	DocumentTreeBuilder& operator=(DocumentTreeBuilder const& rhs);
    
    /*!
     *  Creates an object tree starting at the given component.
     *
     *      @param [in] root   The VLNV of the component document.
     *
     *      @return The object tree representing the document tree starting at the component.
     */
    QObject* createFromComponent(VLNV const &root) const;
        
    /*!
     *  Creates an object tree starting at the given design configuration.
     *
     *      @param [in] designConfiguration     The VLNV of the design configuration document.
     *
     *      @return The object tree representing the document tree starting at the design configuration.
     */
    QObject* createFromDesignConfiguration(VLNV const& designConfiguration) const;
        
    /*!
     *  Creates an object tree starting at the given design.
     *
     *      @param [in] designRef       The VLNV of the design document.
     *      @param [in] configuration   The design configuration for the design, if any.
     *
     *      @return The object tree representing the document tree starting at the design.
     */
    QObject* createFromDesign(VLNV const& designRef, QSharedPointer<DesignConfiguration const> configuration) const;

    //! The IP-XACT library in use.
    LibraryInterface* library_;
};

#endif // DOCUMNENTREEBUILDER_H
