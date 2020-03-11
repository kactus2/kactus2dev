//-----------------------------------------------------------------------------
// File: SimplePort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.02.2020
//
// Description:
// Simplified port data model.
//-----------------------------------------------------------------------------

// #pragma once

#include "../pythonapi_global.h"

#include <QString>
#include <QSharedPointer>

#include "SimpleNameGroup.h"

class Port;

//-----------------------------------------------------------------------------
//! Simplified port data model.
//-----------------------------------------------------------------------------
// class PYTHONAPI_EXPORT PythonAPI
class SimplePort : public SimpleNameGroup
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the port.
     *      @param [in] description     Description of the port.
     *      @param [in] bitWidth        Bit width of the port.
     *      @param [in] direction       Direction of the port.
     *      @param [in] dataType        Data type of the port.
     */
    SimplePort(QString const& name, QString const& description, int const& bitWidth, QString const& direction,
        QString const& dataType = QString(""));

    /*!
     *  The destructor.
     */
    ~SimplePort() = default;

    /*!
     *  Get the data type.
     *
     *      @return Data type.
     */
    QString getDataType() const;

    /*!
     *  Get the bit width.
     *
     *      @return The bit width.
     */
    int getBitWidth() const;

    /*!
     *  Get the direction.
     *
     *      @return The direction.
     */
    QString getDirection() const;

private:

    //! Data type of the port.
    QString dataType_;
    
    //! Bit width of the port.
    int bitWidth_;
    
    //! Direction of the port.
    QString direction_;
};
