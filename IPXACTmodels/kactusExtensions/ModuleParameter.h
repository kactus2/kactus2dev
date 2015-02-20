//-----------------------------------------------------------------------------
// File: ModuleParameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.02.2015
//
// Description:
// Component instance module parameter element in IP-XACT 2014.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETER_H
#define MODULEPARAMETER_H

#include <IPXACTmodels/modelparameter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Component instance module parameter element in IP-XACT 2014.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ModuleParameter : public ModelParameter
{
public:

	//! The constructor.
	ModuleParameter(QDomNode& node);

	//! The destructor.
	~ModuleParameter();

    /*!
     *  Creates a copy of the module parameter.
     *
     *      @return Copy of the module parameter.
     */
    ModuleParameter* clone() const;

    /*!
     *  Gets the general name of the IP-Xact element represented by the model parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementName() const;

protected:
                            
    /*!
     *  Gets the identifier for the IP-Xact element represented by the model parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementIdentifier() const;

private:

	// Disable assignment.
	ModuleParameter& operator=(ModuleParameter const& rhs);
    ModuleParameter(ModuleParameter const& other);

};

#endif // MODULEPARAMETER_H
