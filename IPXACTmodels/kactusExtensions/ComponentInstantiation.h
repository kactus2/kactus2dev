//-----------------------------------------------------------------------------
// File: ComponentInstantiation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.02.2015
//
// Description:
// Component instance element in IP-XACT 2014.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATION_H
#define COMPONENTINSTANTIATION_H

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>

#include <QSharedPointer>
#include <QList>
#include <QDomNode>

class ModelParameter;
class ModuleParameter;
class Parameter;

class IPXACTMODELS_EXPORT ComponentInstantiation : public VendorExtension, public NameGroup
{
public:

    ComponentInstantiation();

	//! The constructor.
	ComponentInstantiation(QDomNode& node);

	//! The destructor.
	virtual ~ComponentInstantiation();

    /*!
     *  Creates a copy of the component instantiation.
     *
     *      @return A copy of the component instantiation.
     */
    virtual VendorExtension* clone() const;

    /*!
     *  Returns a type identifier for the component instantiation.
     *
     *      @return A type identifier of the component instantiation.
     */
    virtual QString type() const;
    
    /*!
     *  Writes the component instantiation to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Adds a module parameter to the component instantiation.
     *
     *      @param [in] moduleParameter   The module parameter to add.
     */
    void addModuleParameter(QSharedPointer<ModuleParameter> moduleParameter);

    /*!
     *  Gets the module parameters in the component instantiation.
     *
     *      @return The module parameters.
     */
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > getModuleParameters() const;

private:

	// Disable copying.
	ComponentInstantiation(ComponentInstantiation const& rhs);
	ComponentInstantiation& operator=(ComponentInstantiation const& rhs);

    //! The module parameters in the component instantiation.
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > moduleParameters_;

};

#endif // COMPONENTINSTANTIATION_H
