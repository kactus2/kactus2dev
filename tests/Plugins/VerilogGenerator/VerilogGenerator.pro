#-----------------------------------------------------------------------------
# File: VerilogGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 28.07.2014
#
# Description:
# Qt project file for running unit tests for Verilog generation.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_VerilogWriterFactory.pro \
          tst_VerilogHeaderWriter.pro \
          tst_InterfaceDirectionNameSorter.pro \
          tst_CommentWriter.pro \
          tst_ImplementationSelection.pro
