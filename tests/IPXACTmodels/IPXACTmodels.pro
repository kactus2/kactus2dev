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
		   Design\tst_DesignWriter.pro \
		   Design\tst_DesignReader.pro \
		   Component\Component.pro \
