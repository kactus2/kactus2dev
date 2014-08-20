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
SUBDIRS += VerilogGenerator/tst_VerilogGenerator.pro \
		   VerilogHeaderWriter/tst_VerilogHeaderWriter.pro \
		   ComponentVerilogWriter/tst_ComponentVerilogWriter.pro \
           PortVerilogWriter/tst_PortVerilogWriter.pro \
		   ModelParameterVerilogWriter/tst_ModelParameterVerilogWriter.pro \
		   ComponentInstanceVerilogWriter/tst_ComponentInstanceVerilogWriter.pro \
		   InterfaceDirectionNameSorter/tst_InterfaceDirectionNameSorter.pro \
		   CommentWriter/tst_CommentWriter.pro
