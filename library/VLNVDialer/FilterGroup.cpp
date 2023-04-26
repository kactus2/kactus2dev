// File: FilterGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 24.04.2023
//
// Description:
// Base class for library filter groups
//-----------------------------------------------------------------------------


#include "FilterGroup.h"

//-----------------------------------------------------------------------------
// Function: FilterGroup::FilterGroup()
//-----------------------------------------------------------------------------
FilterGroup::FilterGroup(QString const& title, QWidget *parent):
QGroupBox(title, parent)
{
    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: FilterGroup::setupButton()
//-----------------------------------------------------------------------------
void FilterGroup::setupButton(QPushButton* button, QString const& tooltip)
{
    button->setToolTip(tooltip);
    button->setFlat(true);
    button->setCheckable(true);

    const int ICON_SIZE = 24;
    button->setMinimumHeight(ICON_SIZE);
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
}
