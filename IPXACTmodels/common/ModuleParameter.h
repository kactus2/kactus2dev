//-----------------------------------------------------------------------------
// File: ModuleParameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Implementation for ipxact:moduleParameter element.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETER_H
#define MODULEPARAMETER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include "Parameter.h"

//-----------------------------------------------------------------------------
//! Implementation for ipxact:moduleParameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ModuleParameter : public Parameter
{
public:

    /*!
     *  The default constructor.
     */
    ModuleParameter();

	//! Copy constructor
    ModuleParameter(const ModuleParameter& other);

    //! The destructor
    virtual ~ModuleParameter();

	//! Assignment operator
    ModuleParameter& operator=(const ModuleParameter& other);

    /*!
     *  Get the data type attribute.
     *
     *      @return The data type.
     */
    QString getDataType() const;

    /*!
     *  Set the data type attribute.
     *
     *      @param [in] newDataType     The new data type.
     */
    void setDataType(QString const& newDataType);

    /*!
     *  Get the usage type attribute.
     *
     *      @return The usage type.
     */
    QString getUsageType() const;

    /*!
     *  Set the usage type attribute.
     *
     *      @param [in] newUsageType    The new usaget type.
     */
    void setUsageType(QString const& newUsageType);

    /*!
     *  Get the presence.
     *
     *      @return The presence.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence expression.
     */
    void setIsPresent(QString const& newIsPresent);

private:
 
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The presence of the module parameter.
    QString isPresent_;
};

#endif /* PARAMETER_H */
