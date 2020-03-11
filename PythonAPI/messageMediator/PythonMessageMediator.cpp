//-----------------------------------------------------------------------------
// File: PythonMessageMediator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Output for user messages in python API.
//-----------------------------------------------------------------------------

#include "PythonMessageMediator.h"

#include <iostream>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: PythonMessageMediator::PythonMessageMediagor()
//-----------------------------------------------------------------------------
PythonMessageMediator::PythonMessageMediator()
{

}

//-----------------------------------------------------------------------------
// Function: PythonMessageMediator::showMessage()
//-----------------------------------------------------------------------------
void PythonMessageMediator::showMessage(QString const& message) const
{
    static QTextStream out(stdout);
    out << message << endl;  
}

//-----------------------------------------------------------------------------
// Function: PythonMessageMediator::showError()
//-----------------------------------------------------------------------------
void PythonMessageMediator::showError(QString const& error) const
{
    static QTextStream err(stderr);
    err << error << endl;
}

//-----------------------------------------------------------------------------
// Function: PythonMessageMediator::showFailure()
//-----------------------------------------------------------------------------
void PythonMessageMediator::showFailure(QString const& error) const
{
    showError(error);
}

//-----------------------------------------------------------------------------
// Function: PythonMessageMediator::showStatusMessage()
//-----------------------------------------------------------------------------
void PythonMessageMediator::showStatusMessage(QString const& status) const
{
    showMessage(status);
}
