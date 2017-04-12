//-----------------------------------------------------------------------------
// File: NameGenerationPolicy.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 22.10.2015
//
// Description:
// Used to generate names for new elements.
//-----------------------------------------------------------------------------

#ifndef NAMEGENERATIION_POLICY_H
#define NAMEGENERATIION_POLICY_H

#include <QString>

namespace NameGenerationPolicy
{
    /*!
     *  Returns name of a system view file set, based on the name of it.
     *
     *      @param [in] systemViewName  Name of the system view.
	 *
	 *      @return The resolved name for the file set.
	 */
	QString systemViewFilesetName(QString const& systemViewName);
	
    /*!
     *  Returns name of a instance file set, based on the name of it. and its system view.
     *
     *      @param [in] systemViewName  Name of the system view.
	 *      @param [in] instanceName   	Name of the component instance.
	 *
	 *      @return The resolved name for the file set.
	 */
	QString instanceFilesetName(QString const& systemViewName, QString const& instanceName);
	
    /*!
     *  Returns name of a flat view, based on a qualifier.
     *  If the qualifier is empty, returns a name without language.
     *
     *      @param [in] qualifierName       The qualifier that separates the view from others.
	 */
	QString flatViewName(QString const& qualifierName = QString(""));
	
    /*!
     *  Returns name of a hierarchical view, on a qualifier.
     *  If the qualifier is empty, returns a name without language.
     *
     *      @param [in] qualifierName       The qualifier that separates the view from others.
	 */
	QString hierarchicalViewName(QString const& qualifierName = QString(""));
	
    /*!
     *  Returns name of a component instantiation, based on a qualifier.
     *  If the qualifier is empty, returns a name without language.
     *
     *      @param [in] qualifierName       The qualifier that separates the instantiation from others.
	 */
	QString componentInstantiationName(QString const& qualifierName = QString(""));
	
    /*!
     *  Returns name of a design instantiation, based on a qualifier.
     *
     *      @param [in] qualifierName       The qualifier that separates the instantiation from others.
	 */
	QString designInstantiationName(QString const& qualifierName);
	
    /*!
     *  Returns name of a design configuration instantiation, based on a qualifier.
     *
     *      @param [in] qualifierName       The qualifier that separates the instantiation from others.
	 */
	QString designConfigurationInstantiationName(QString const& qualifierName);
};

#endif