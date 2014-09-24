#-----------------------------------------------------------------------------
# File: VerilogImport.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 23.09.2014
#
# Description:
# Qt project file template for running unit tests for Verilog import.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += VerilogImporter/tst_VerilogImporter.pro \
		   VerilogPortParser/tst_VerilogPortParser.pro 
