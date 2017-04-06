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
     *  Returns name of a flat view, based on language.
     *  If the language is empty, returns a name without language.
     *
     *      @param [in] languageName    The supposed implementation language of the view.
	 */
	QString flatViewName(QString const& languageName = QString(""));
	
    /*!
     *  Returns name of a hierarchical view, based on language.
     *  If the language is empty, returns a name without language.
     *
     *      @param [in] languageName    The supposed implementation language of the view.
	 */
	QString hierarchicalViewName(QString const& languageName = QString(""));
	
    /*!
     *  Returns name of a component instantiation, based on language.
     *  If the language is empty, returns a name without language.
     *
     *      @param [in] viewName  Name of the view.
	 */
	QString componentInstantiationName(QString const& languageName = QString(""));
	
    /*!
     *  Returns name of a design instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString designInstantiationName(QString const& viewName);
	
    /*!
     *  Returns name of a design configuration instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString designConfigurationInstantiationName(QString const& viewName);
};

#endif