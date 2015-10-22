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
	QString systemViewFilesetName( QString systemViewName );
	
    /*!
     *  Returns name of a instance file set, based on the name of it. and its system view.
     *
     *      @param [in] systemViewName  Name of the system view.
	 *      @param [in] instanceName   	Name of the component instance.
	 *
	 *      @return The resolved name for the file set.
	 */
	QString instanceFilesetName( QString systemViewName, QString instanceName );
};

#endif