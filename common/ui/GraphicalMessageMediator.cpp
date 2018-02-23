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
GraphicalMessageMediator::GraphicalMessageMediator(): console_(0), statusBar_(0)
{

}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::~GraphicalMessageMediator()
//-----------------------------------------------------------------------------
GraphicalMessageMediator::~GraphicalMessageMediator()
{

}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showMessage()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showMessage(QString const& message) const
{
    if (console_)
    {
        console_->onNoticeMessage(message);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::showError()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::showError(QString const& error) const
{
    if (console_)
    {
        console_->onErrorMessage(error);
    }
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
    if (statusBar_)
    {
        statusBar_->showMessage(status);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::setMessageConsole()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::setMessageConsole(MessageConsole* console)
{
    console_ = console;
}

//-----------------------------------------------------------------------------
// Function: GraphicalMessageMediator::setStatusBar()
//-----------------------------------------------------------------------------
void GraphicalMessageMediator::setStatusBar(QStatusBar* statusBar)
{
    statusBar_ = statusBar;
}
