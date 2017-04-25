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
	QString NameGenerationPolicy::systemViewFilesetName(QString const& systemViewName)
	{
		return systemViewName + SYSVIEW_FILESET_SUFFIX;
	}
		
	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::instanceFilesetName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::instanceFilesetName(QString const& systemViewName, QString const& instanceName)
	{
		return systemViewName + "_" + instanceName + INSTANCE_FILESET_SUFFIX;
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::flatViewName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::flatViewName(QString const& qualifierName)
	{
        if (qualifierName.isEmpty())
        {
		    return "flat";
        }

        return "flat_" + qualifierName;
    }

    //-----------------------------------------------------------------------------
    // Function: NameGenerationPolicy::flatViewName()
    //-----------------------------------------------------------------------------
    QString NameGenerationPolicy::hierarchicalViewName(QString const& qualifierName)
    {
        if (qualifierName.isEmpty())
        {
            return "hierarchical";
        }

        return "hierarchical_" + qualifierName;
    }

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::componentInstantiationName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::componentInstantiationName(QString const& qualifierName)
    {
        if (qualifierName.isEmpty())
        {
            return "implementation";
        }

		return qualifierName + "_implementation";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::designInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::designInstantiationName(QString const& qualifierName)
	{
		return qualifierName + "_design";
	}

	//-----------------------------------------------------------------------------
	// Function: NameGenerationPolicy::designConfigurationInstanceName()
	//-----------------------------------------------------------------------------
	QString NameGenerationPolicy::designConfigurationInstantiationName(QString const& qualifierName)
	{
		return qualifierName + "_design_configuration";
	}

