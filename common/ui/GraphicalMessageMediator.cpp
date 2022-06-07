//-----------------------------------------------------------------------------
// File: GraphicalMessageMediator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.02.2018
//
// Description:
// Graphical output for user messages.
//-----------------------------------------------------------------------------

#include "GraphicalMessageMediator.h"

#include <mainwindow/MessageConsole/messageconsole.h>

#include <QApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::GraphicalMessageMediator()
//-----------------------------------------------------------------------------
GraphicalMessageMediator::GraphicalMessageMediator(QObject* parent): QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showMessage()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showMessage(QString const& message) const
{
    emit noticeMessage(message);
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showError()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showError(QString const& error) const
{
    emit errorMessage(error);
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showFailure()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showFailure(QString const& error) const
{
    QMessageBox msgBox(QMessageBox::Critical, QApplication::applicationName(),
        error, QMessageBox::Ok, 0);

    msgBox.exec();
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showStatusMessage()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showStatusMessage(QString const& status) const
{    
    emit statusMessage(status);
}
