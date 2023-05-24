//-----------------------------------------------------------------------------
// File: DocumentNameGroupEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 23.05.2023
//
// Description:
// Editor for document name group.
//-----------------------------------------------------------------------------

#include "DocumentNameGroupEditor.h"

#include <IPXACTmodels/common/Document.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QGridLayout>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::DocumentNameGroupEditor()
//-----------------------------------------------------------------------------
DocumentNameGroupEditor::DocumentNameGroupEditor(QWidget* parent) :
    QGroupBox(parent)
{
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::setDocumentNameGroup()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::setDocumentNameGroup(QSharedPointer<Document> document, QString const& documentPath)
{
    document_ = document;
    setVlnv(document_->getVlnv());

    displayNameEditor_.setText(document_->getDisplayName());
    shortDescriptionEditor_.setText(document_->getShortDescription());

    path_.setText(documentPath);
    descriptionEditor_.setPlainText(document_->getDescription());
    
    compatibility_.setText(Document::toString(document->getRevision()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::setVlnv()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::setVlnv(VLNV const& vlnv)
{
    vendor_.setText(vlnv.getVendor());
    library_.setText(vlnv.getLibrary());
    name_.setText(vlnv.getName());
    version_.setText(vlnv.getVersion());

    if (!vlnv.isValid())
    {
        if (vlnv.getVendor().isEmpty())
        {
            vendor_.setText("unassigned");
        }
        if (vlnv.getLibrary().isEmpty())
        {
            library_.setText("unassigned");
        }
        if (vlnv.getName().isEmpty())
        {
            name_.setText("unassigned");
        }
        if (vlnv.getVersion().isEmpty())
        {
            version_.setText("unassigned");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::setupLayout()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout();

    QFormLayout* docNameGroupInfoLayout = new QFormLayout();
    docNameGroupInfoLayout->addRow(QStringLiteral("Vendor:"), &vendor_);
    docNameGroupInfoLayout->addRow(QStringLiteral("Library:"), &library_);
    docNameGroupInfoLayout->addRow(QStringLiteral("Name:"), &name_);
    docNameGroupInfoLayout->addRow(QStringLiteral("Version:"), &version_);
    docNameGroupInfoLayout->addRow(QStringLiteral("Compatibility:"), &compatibility_);

    if (!path_.text().isEmpty()) {
        docNameGroupInfoLayout->addRow(QStringLiteral("Path:"), &path_);
    }

    if (document_->getRevision() == Document::Revision::Std22)
    {
        docNameGroupInfoLayout->addRow(QStringLiteral("Display name:"), &displayNameEditor_);
        docNameGroupInfoLayout->addRow(QStringLiteral("Short Description:"), &shortDescriptionEditor_);
    }
    
    QWidget* docNameGroupInfo = new QWidget(this);
    docNameGroupInfo->setLayout(docNameGroupInfoLayout);

    QGroupBox* descriptionEditorBox = new QGroupBox(QStringLiteral("Description"), this);
    descriptionEditorBox->setFlat(true);
    
    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionEditorBox);
    descriptionLayout->addWidget(&descriptionEditor_);

    topLayout->addWidget(docNameGroupInfo);
    topLayout->addWidget(descriptionEditorBox);
    
    setLayout(topLayout);
}

