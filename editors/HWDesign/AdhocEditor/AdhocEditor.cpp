//-----------------------------------------------------------------------------
// File: AdhocEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 27.04.2023
//
// Description:
// Editor for ad-hoc port visibilities and tie off values.
//-----------------------------------------------------------------------------

#include "AdhocEditor.h"

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: AdhocEditor::AdhocEditor()
//-----------------------------------------------------------------------------
AdhocEditor::AdhocEditor(QSharedPointer<ParameterFinder> designFinder, QWidget* parent) : QWidget(parent),
visibilityEditor_(this),
portEditor_(designFinder, this)
{
    connect(&portEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setVisibilityData()
//-----------------------------------------------------------------------------
void AdhocEditor::setVisibilityData(AdHocEnabled* dataSource, QSharedPointer<Design> design,
    QSharedPointer<IEditProvider> editProvider, bool lockEditor)
{
    visibilityEditor_.setDataSource(dataSource, design, editProvider, lockEditor);
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::clearVisibilityData()
//-----------------------------------------------------------------------------
void AdhocEditor::clearVisibilityData()
{
    visibilityEditor_.clear();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setPortData()
//-----------------------------------------------------------------------------
void AdhocEditor::setPortData(AdHocItem* endPoint, HWDesignDiagram* containingDiagram,
    QSharedPointer<IEditProvider> editProvider)
{
    portEditor_.setAdhocPort(endPoint, containingDiagram, editProvider);
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::clearPortData()
//-----------------------------------------------------------------------------
void AdhocEditor::clearPortData()
{
    portEditor_.clear();
}

//-----------------------------------------------------------------------------
// Function: AdhocEditor::setupLayout()
//-----------------------------------------------------------------------------
void AdhocEditor::setupLayout()
{
    auto layout = new QGridLayout(this);
    layout->addWidget(&visibilityEditor_, 0, 0);
    layout->addWidget(&portEditor_, 1, 0);
    layout->setRowStretch(0, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
}
