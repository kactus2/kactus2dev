//-----------------------------------------------------------------------------
// File: ParameterCompleter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.01.2015
//
// Description:
// Completer for selecting parameter references.
//-----------------------------------------------------------------------------

#include "ParameterCompleter.h"

//#include <editors/ComponentEditor/common/ParameterResolver.h>
#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

#include <QHeaderView>
#include <QFrame>
#include <QFont>
#include <QTableView>

//-----------------------------------------------------------------------------
// Function: ParameterCompleter::ParameterCompleter()
//-----------------------------------------------------------------------------
ParameterCompleter::ParameterCompleter(QObject *parent)
    : QCompleter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterCompleter::~ParameterCompleter()
//-----------------------------------------------------------------------------
ParameterCompleter::~ParameterCompleter()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterCompleter::setModel()
//-----------------------------------------------------------------------------
void ParameterCompleter::setModel(QAbstractItemModel* model)
{
    QCompleter::setModel(model);
    setupPopup();
}

//-----------------------------------------------------------------------------
// Function: ParameterCompleter::setupPopup()
//-----------------------------------------------------------------------------
void ParameterCompleter::setupPopup()
{
    QTableView* popup = new QTableView();

    setPopup(popup);

    popup->horizontalHeader()->hide();
    popup->verticalHeader()->hide();

    popup->setCornerButtonEnabled(false);
    popup->setShowGrid(false);

    popup->setColumnHidden(ComponentParameterColumns::ID, true);

    popup->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    popup->verticalHeader()->setDefaultSectionSize(popup->fontMetrics().height() + 2);
    popup->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    popup->horizontalHeader()->setStretchLastSection(true);

    popup->setMinimumWidth(300);

    popup->setFrameShadow(QFrame::Raised);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFont(QFont("Tahoma", 8));

    popup->setStyleSheet("selection-color: black; selection-background-color: "
        "QLinearGradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d6e7ff, stop: 1 #84aede);");
}
