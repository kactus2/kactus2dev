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
SUBDIRS += validators\tst_ParameterValidator.pro \
		   validators\tst_ParameterValidator2014.pro \
		   common\tst_ParameterWriter.pro \
		   common\tst_ParameterReader.pro \
		   BusDefinition\tst_BusDefinitionWriter.pro \
		   BusDefinition\tst_BusDefinitionReader.pro \
		   AbstractionDefinition\tst_AbstractionDefinitionWriter.pro \
		   AbstractionDefinition\tst_AbstractionDefinitionReader.pro \
		   DesignConfiguration\tst_DesignConfigurationWriter.pro \
		   DesignConfiguration\tst_DesignConfigurationReader.pro \
		   Design\tst_DesignWriter.pro