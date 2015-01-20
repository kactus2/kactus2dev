//-----------------------------------------------------------------------------
// File: CompleterPopupTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Popup table.
//-----------------------------------------------------------------------------

#include "CompleterPopupTable.h"

//-----------------------------------------------------------------------------
// Function: CompleterPopupTable::CompleterPopupTable()
//-----------------------------------------------------------------------------
CompleterPopupTable::CompleterPopupTable(QWidget *parent /* = 0 */):
    QTableView(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CompleterPopupTable::~CompleterPopupTable()
//-----------------------------------------------------------------------------
CompleterPopupTable::~CompleterPopupTable()
{

}

//-----------------------------------------------------------------------------
// Function: CompleterPopupTable::showEvent()
//-----------------------------------------------------------------------------
void CompleterPopupTable::showEvent(QShowEvent *event)
{
    emit (resetParameterModel());

    QTableView::showEvent(event);
}