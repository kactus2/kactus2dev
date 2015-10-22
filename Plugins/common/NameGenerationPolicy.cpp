#include "NameGenerationPolicy.h"

namespace NameGenerationPolicy
{
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
}