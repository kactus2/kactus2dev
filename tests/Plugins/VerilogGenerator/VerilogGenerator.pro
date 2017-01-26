#-----------------------------------------------------------------------------
# File: VerilogGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 28.07.2014
#
# Description:
# Qt project file for running unit tests for Verilog generation.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS +=  VerilogGeneratorPlugin/tst_VerilogGeneratorPlugin.pro \
		   VerilogWriterFactory/tst_VerilogWriterFactory.pro \
		   VerilogHeaderWriter/tst_VerilogHeaderWriter.pro \
		   InterfaceDirectionNameSorter/tst_InterfaceDirectionNameSorter.pro \
		   CommentWriter/tst_CommentWriter.pro \
           ImplementationSelection/tst_ImplementationSelection.pro
