//-----------------------------------------------------------------------------
// File: NameGenerationPolicy.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 22.10.2015
//
// Description:
// Used to generate names with predictable patterns.
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
	
    /*!
     *  Returns name of a verilog structural view, based on name of another view.
     *
     *      @param [in] viewName  Name of the other view.
	 *
	 *      @return The resolved name for the verilog structural view.
	 */
	QString verilogStructuralViewName( QString viewName );
	
    /*!
     *  Returns name of a generic flat view.
	 *
	 *      @return See above.
	 */
	QString flatViewName();
	
    /*!
     *  Returns name of a verilog component instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString verilogComponentInstantiationName( QString viewName );
	
    /*!
     *  Returns name of a vhdl component instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString vhdlComponentInstantiationName( QString viewName );
	
    /*!
     *  Returns name of a design instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString designInstantiationName( QString viewName );
	
    /*!
     *  Returns name of a design configuration instantiation, based on name of its associated view.
     *
     *      @param [in] viewName  Name of the view.
	 *
	 *      @return The resolved name for the instantiation.
	 */
	QString designConfigurationInstantiationName( QString viewName );
};

#endif