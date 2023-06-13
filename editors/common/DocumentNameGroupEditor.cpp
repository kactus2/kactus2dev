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

#include <QVBoxLayout>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::DocumentNameGroupEditor()
//-----------------------------------------------------------------------------
DocumentNameGroupEditor::DocumentNameGroupEditor(QWidget* parent) :
    QGroupBox(parent)
{
    connect(&displayNameEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onDisplayNameChanged()), Qt::UniqueConnection);
    connect(&shortDescriptionEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onShortDescriptionChanged()), Qt::UniqueConnection);
    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
    setupLayout();
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

    if (!documentPath.isEmpty())
    {
        pathTitleLabel_.show();
        path_.setText(documentPath);
    }
    else
    {
        pathTitleLabel_.hide();
        path_.hide();
    }

    bool showStd22Fields = true;
    if (document_->getRevision() != Document::Revision::Std22)
    {
        showStd22Fields = false;
    }

    displayNameEditor_.setVisible(showStd22Fields);
    shortDescriptionEditor_.setVisible(showStd22Fields);
    displayNameTitleLabel_.setVisible(showStd22Fields);
    shortDescriptionTitleLabel_.setVisible(showStd22Fields);

    descriptionEditor_.setPlainText(document_->getDescription());
    
    compatibility_.setText(Document::toString(document_->getRevision()));
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::setPlaceholderDescription()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::setPlaceholderDescription(QString const& placeholder)
{
    descriptionEditor_.setPlaceholderText(placeholder);
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::onDisplayNameChanged()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::onDisplayNameChanged()
{
    document_->setDisplayName(displayNameEditor_.text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::onShortDescriptionChanged()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::onShortDescriptionChanged()
{
    document_->setShortDescription(shortDescriptionEditor_.text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DocumentNameGroupEditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void DocumentNameGroupEditor::onDescriptionChanged()
{
    document_->setDescription(descriptionEditor_.toPlainText());
    emit contentChanged();
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

    docNameGroupInfoLayout->addRow(&pathTitleLabel_, &path_);
    docNameGroupInfoLayout->addRow(&displayNameTitleLabel_, &displayNameEditor_);
    docNameGroupInfoLayout->addRow(&shortDescriptionTitleLabel_, &shortDescriptionEditor_);
    
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

