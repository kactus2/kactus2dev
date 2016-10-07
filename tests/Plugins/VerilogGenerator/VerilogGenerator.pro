#-----------------------------------------------------------------------------
# File: VerilogGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running unit tests for Verilog generation.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS +=  VerilogGeneratorPlugin/tst_VerilogGeneratorPlugin.pro \
		   VerilogGenerator/tst_VerilogGenerator.pro \
		   VerilogHeaderWriter/tst_VerilogHeaderWriter.pro \
		   InterfaceDirectionNameSorter/tst_InterfaceDirectionNameSorter.pro \
		   CommentWriter/tst_CommentWriter.pro
