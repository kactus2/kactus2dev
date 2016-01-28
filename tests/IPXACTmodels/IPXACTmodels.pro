#-----------------------------------------------------------------------------
# File: IPXACTmodels.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests in IPXACTmodels.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += common\Common.pro \
		   BusDefinition\tst_BusDefinitionWriter.pro \
		   BusDefinition\tst_BusDefinitionReader.pro \
		   BusDefinition\validators\tst_BusDefinitionValidator.pro \
		   AbstractionDefinition\tst_AbstractionDefinitionWriter.pro \
		   AbstractionDefinition\tst_AbstractionDefinitionReader.pro \
		   AbstractionDefinition\validators\tst_AbstractionDefinitionValidator.pro \
		   DesignConfiguration\tst_DesignConfigurationWriter.pro \
		   DesignConfiguration\tst_DesignConfigurationReader.pro \
		   DesignConfiguration\validators\tst_DesignConfigurationValidator.pro \
		   Design\tst_DesignWriter.pro \
		   Design\tst_DesignReader.pro \
		   Design\validators\tst_DesignValidator.pro \
		   Component\Component.pro \
		   kactusExtensions\tst_ComDefinitionWriter.pro \
		   kactusExtensions\tst_ComDefinitionReader.pro \
		   kactusExtensions\tst_ComDefinitionValidator.pro \
		   kactusExtensions\tst_ApiDefinitionWriter.pro \
		   kactusExtensions\tst_ApiDefinitionReader.pro \
		   kactusExtensions\tst_ApiDefinitionValidator.pro \
