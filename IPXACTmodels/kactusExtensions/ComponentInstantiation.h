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

#include <IPXACTmodels/NameGroup.h>

#include <QSharedPointer>
#include <QList>

class ModelParameter;
class ModuleParameter;
class Parameter;

class IPXACTMODELS_EXPORT ComponentInstantiation : public VendorExtension
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
     *  Sets the name of the component instantiation.
     *
     *      @param [in] name   The name to set.
     */
    void setName(QString const& name);

    //! Gets the name of the component instantiation.
    QString getName() const;

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

    //! Contains the name, display name and description of the component instantiation.
    NameGroup nameGroup_;

    //! The module parameters in the component instantiation.
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > moduleParameters_;

};

#endif // COMPONENTINSTANTIATION_H
