//-----------------------------------------------------------------------------
// File: ParameterResolver.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.01.2015
//
// Description:
// Finds a parameter in a component using an id.
//-----------------------------------------------------------------------------

#ifndef PARAMETERRESOLVER_H
#define PARAMETERRESOLVER_H

#include <QString>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Finds a parameter in a component using an id.
//-----------------------------------------------------------------------------
class ParameterResolver 
{
public:

	//! The constructor.
	ParameterResolver(QSharedPointer<Component> targetComponent);

	//! The destructor.
	~ParameterResolver();

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id   The id to search for.
     *
     *      @return True, if a parameter with the given id exists, otherwise false.
     */
    bool hasId(QString const& id) const;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id   The id to search for.
     *
     *      @return The name of the parameter.
     */
    QString nameForId(QString const& id) const;

private:

	// Disable copying.
	ParameterResolver(ParameterResolver const& rhs);
	ParameterResolver& operator=(ParameterResolver const& rhs);

    //! The component whose parameters can be resolved.
    QSharedPointer<Component> component_;
};

#endif // PARAMETERRESOLVER_H
