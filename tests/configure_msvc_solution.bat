@echo off
REM -----------------------------------------------------------------------------
REM  File: configure_msvc_solution.bat
REM -----------------------------------------------------------------------------
REM  Project: Kactus 2
REM  Author: Esko Pekkarinen
REM  Date: 24.27.2014
REM 
REM  Description:
REM  Regenerates the visual studio project and solution files for all unit tests.
REM -----------------------------------------------------------------------------
qmake -tp vc -r
