//-----------------------------------------------------------------------------
// File: SimpleNameGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.02.2020
//
// Description:
// Simplified name group data model.
//-----------------------------------------------------------------------------

#ifndef SIMPLENAMEGROUP_H
#define SIMPLENAMEGROUP_H

#include "../pythonapi_global.h"

#include <QString>

//-----------------------------------------------------------------------------
//! Simplified name group data model.
//-----------------------------------------------------------------------------
// class PYTHONAPI_EXPORT PythonAPI
class SimpleNameGroup
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            The name.
     *      @param [in] description     The description.
     */
    SimpleNameGroup(QString const& name, QString const& description);

    /*!
     *  The destructor.
     */
    virtual ~SimpleNameGroup() = default;

    /*!
     *  Get the name.
     *
     *      @return The name.
     */
    QString getName() const;

    /*!
     *  Get the description.
     *
     *      @return The description.
     */
    QString getDescription() const;

private:

    //! Name.
    QString name_;

    //! Description.
    QString description_;
};

#endif // SIMPLENAMEGROUP_H
