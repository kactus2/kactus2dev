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

#include "NameGenerationPolicy.h"


	// File set containing design configuration specific files = name + suffix.
	const QString SYSVIEW_FILESET_SUFFIX = "_general_files";
	// File set containing instance specific files = name + suffix.
	const QString INSTANCE_FILESET_SUFFIX = "_headers";
		
	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::systemViewFilesetName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::systemViewFilesetName( QString systemViewName )
	{
		return systemViewName + SYSVIEW_FILESET_SUFFIX;
	}
		
	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::systemViewFilesetName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::instanceFilesetName( QString systemViewName, QString instanceName )
	{
		return systemViewName + "_" + instanceName + INSTANCE_FILESET_SUFFIX;
	}
		
	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::verilogStructuralViewName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::verilogStructuralViewName( QString viewName )
	{
		return viewName + "_verilog";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::flatViewName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::flatViewName()
	{
		return "flat";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::verilogComponentInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::verilogComponentInstantiationName( QString viewName )
	{
		return viewName + "_verilog_component";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::vhdlComponentInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::vhdlComponentInstantiationName( QString viewName )
	{
		return viewName + "_vhdl_component";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::designInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::designInstantiationName( QString viewName )
	{
		return viewName + "_design";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::designConfigurationInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::designConfigurationInstantiationName( QString viewName )
	{
		return viewName + "_design_configuration";
	}

