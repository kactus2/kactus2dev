#-----------------------------------------------------------------------------
# File: Core.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 08.05.2018
#
# Description:
# Qt project file template for running unit tests for Kactus2 core.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_CommandLineParser.pro \
		   tst_SystemVerilogExpressionParser.pro \
		   tst_IPXactSystemVerilogParser.pro \
		   tst_ParameterCompleter.pro \
		   tst_ExpressionEditor.pro \
		   tst_ValueFormatter.pro \
		   tst_ComponentParameterModel.pro \
		   tst_ParameterReferenceTree.pro \
		   tst_VLNVComparator.pro \
		   tst_ViewComparator.pro \
		   tst_PortComparator.pro \
		   tst_ComponentComparator.pro \
		   tst_FieldGraphItem.pro \
		   tst_RegisterGraphItem.pro \
		   tst_AddressBlockGraphItem.pro \
		   tst_MemoryMapGraphItem.pro \
		   tst_HierarchicalSaveBuildStrategy.pro \
		   tst_DocumentTreeBuilder.pro \
		   tst_DocumentGenerator.pro \
                   tst_MarkdownGenerator.pro \
		   tst_MasterSlavePathSearch.pro
