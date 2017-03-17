//-----------------------------------------------------------------------------
// File: generaleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#include "generaleditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSplitter>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: GeneralEditor::GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::GeneralEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, QWidget *parent):
ItemEditor(component, libHandler, parent),
    vlnvDisplayer_(new VLNVDisplayer(this, component->getVlnv())),
    attributeEditor_(new KactusAttributeEditor(this)),
    authorEditor_(new QLineEdit(this)),
    licenseEditor_(new QLineEdit(this)),
    descriptionEditor_(new QPlainTextEdit(this)),
    headerEditor_(new QPlainTextEdit(this)),
    previewBox_(new ComponentPreviewBox(libHandler, this))
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(component != 0);

    const QString xmlPath = libHandler->getPath(component->getVlnv());
    vlnvDisplayer_->setTitle(tr("Component VLNV and location"));
	vlnvDisplayer_->setPath(xmlPath);

    previewBox_->setInteractive(true);
    previewBox_->setComponent(component);
    //previewBox_->setFixedWidth(280);
    previewBox_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setupLayout();

    connect(previewBox_, SIGNAL(endpointsRearranged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()), Qt::UniqueConnection);
    connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChange()), Qt::UniqueConnection);
    connect(headerEditor_, SIGNAL(textChanged()), this, SLOT(onHeaderChange()), Qt::UniqueConnection);
    connect(authorEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onAuthorChange()), Qt::UniqueConnection);
    connect(licenseEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onLicenseChange()), Qt::UniqueConnection);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::~GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::~GeneralEditor()
{

}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::isValid()
//-----------------------------------------------------------------------------
bool GeneralEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::refresh()
//-----------------------------------------------------------------------------
void GeneralEditor::refresh()
{
	disconnect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()));

    if (component()->getImplementation() == KactusAttribute::HW)
	{
        KactusAttribute::ProductHierarchy prodHier = component()->getHierarchy();
        KactusAttribute::Firmness firmness = component()->getFirmness();
		attributeEditor_->setAttributes(prodHier, firmness);
	}
	else
	{
		attributeEditor_->hideAttributes();
	}

    attributeEditor_->setImplementation(component()->getImplementation());
	connect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()), Qt::UniqueConnection);

	disconnect(authorEditor_, SIGNAL(textChanged(const QString &)), this, SLOT(onAuthorChange()));
	authorEditor_->setText(component()->getAuthor());
	connect(authorEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onAuthorChange()), Qt::UniqueConnection);

    disconnect(licenseEditor_, SIGNAL(textChanged(const QString &)), this, SLOT(onAuthorChange()));
    licenseEditor_->setText(component()->getLicense());
    connect(licenseEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onLicenseChange()), Qt::UniqueConnection);

	disconnect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChange()));
	descriptionEditor_->setPlainText(component()->getDescription());
	connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChange()), Qt::UniqueConnection);

	disconnect(headerEditor_, SIGNAL(textChanged()), this, SLOT(onHeaderChange()));
	QString comment = component()->getTopComments().join("\n");
	headerEditor_->setPlainText(comment);
	connect(headerEditor_, SIGNAL(textChanged()), this, SLOT(onHeaderChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onAttributesChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onAttributesChange()
{
    if (component()->getImplementation() == KactusAttribute::HW)
	{
        component()->setHierarchy(attributeEditor_->getProductHierarchy());
        component()->setFirmness(attributeEditor_->getFirmness());
	}
	emit contentChanged();

	QSettings settings;
	emit hierarchyChanged(settings);
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onDescriptionChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onDescriptionChange()
{
	component()->setDescription(descriptionEditor_->toPlainText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::showEvent()
//-----------------------------------------------------------------------------
void GeneralEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/general.html");

    previewBox_->updatePreview();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onHeaderChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onHeaderChange()
{
	component()->setTopComments(headerEditor_->toPlainText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onAuthorChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onAuthorChange()
{
    component()->setAuthor(authorEditor_->text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onLicenseChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onLicenseChange()
{
    component()->setLicense(licenseEditor_->text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::setupLayout()
//-----------------------------------------------------------------------------
void GeneralEditor::setupLayout()
{
    QWidget* topWidget = new QWidget(this);
    QGridLayout* layout = new QGridLayout(topWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* descriptionBox = new QGroupBox("Description", this);

    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionBox);
    descriptionLayout->addWidget(descriptionEditor_);

    QGroupBox* commentBox = new QGroupBox("XML header", this);

    QVBoxLayout* commentLayout = new QVBoxLayout(commentBox);
    commentLayout->addWidget(headerEditor_);

    QGroupBox* authorBox = new QGroupBox("Copyright information", this);

    QFormLayout* copyrightLayout = new QFormLayout(authorBox);
    copyrightLayout->addRow(tr("Author:"), authorEditor_);
    copyrightLayout->addRow(tr("License:"), licenseEditor_);

    layout->addWidget(new SummaryLabel(tr("Component summary"), this), 0, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(vlnvDisplayer_, 1, 0, 1, 2);
    layout->addWidget(attributeEditor_, 2, 0, 1, 1);
    layout->addWidget(authorBox, 2, 1, 1, 1);    
    layout->addWidget(descriptionBox, 3, 0, 1, 1);
    layout->addWidget(commentBox, 3, 1, 1, 1);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);    
    splitter->addWidget(topWidget);
    splitter->addWidget(previewBox_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(splitter);
    topLayout->setContentsMargins(4, 0, 0, 0);
}
