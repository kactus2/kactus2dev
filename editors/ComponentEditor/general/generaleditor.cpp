//-----------------------------------------------------------------------------
// File: generaleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#include "generaleditor.h"

#include <IPXACTmodels/common/TagData.h>

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/ParameterCache.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <IPXACTmodels/Component/Component.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QSplitter>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: GeneralEditor::GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::GeneralEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, QWidget* parent) :
    ItemEditor(component, libHandler, parent),
    vlnvDisplayer_(new VLNVDisplayer(this, component->getVlnv())),
    attributeEditor_(new KactusAttributeEditor(this)),
    authorEditor_(new QLineEdit(this)),
    licenseEditor_(new QLineEdit(this)),
    descriptionEditor_(new QPlainTextEdit(this)),
    headerEditor_(new QPlainTextEdit(this)),
    finder_(new ParameterCache(component)),
    parser_(new IPXactSystemVerilogParser(finder_)),
    validator_(new ComponentValidator(parser_, libHandler))

{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(component != 0);

    const QString xmlPath = libHandler->getPath(component->getVlnv());
    vlnvDisplayer_->setTitle(tr("Component VLNV and location"));
	vlnvDisplayer_->setPath(xmlPath);

    validityIcon_->setFixedSize(24, 24);

    errorView_->setModel(errorModel_);
    errorView_->hideColumn(2);
    errorView_->horizontalHeader()->hide();
    errorView_->setShowGrid(false);

    setupLayout();

    connect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()), Qt::UniqueConnection);
    connect(descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChange()), Qt::UniqueConnection);
    connect(headerEditor_, SIGNAL(textChanged()), this, SLOT(onHeaderChange()), Qt::UniqueConnection);
    connect(authorEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onAuthorChange()), Qt::UniqueConnection);
    connect(licenseEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onLicenseChange()), Qt::UniqueConnection);

    GeneralEditor::refresh();
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
        QVector<TagData> tags = component()->getTags();
		attributeEditor_->setAttributes(prodHier, firmness, tags);
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

    auto errors = QStringList();
    validator_->findErrorsIn(errors, component());
    
    errorModel_->clear();

    bool hasErrors = errors.empty() == false;
    errorView_->setVisible(hasErrors);

    if (hasErrors)
    {
        errorModel_->addErrors(errors, QString());
        errorView_->resizeColumnsToContents();

        validityIcon_->setPixmap(QPixmap(":icons/common/graphics/exclamation--frame.png").scaled(24, 24));
        validityStatus_->setText(tr("%1 error(s) found:").arg(QString::number(errors.count())));
    }
    else
    {
        validityIcon_->setPixmap(QPixmap(":icons/common/graphics/checkMark.png"));
        validityStatus_->setText(tr("Complete"));
    }

    changeExtensionsEditorItem();
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
        component()->setTags(attributeEditor_->getTags());
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
    QGridLayout* layout = new QGridLayout(this);
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

    QGroupBox* errorBox = new QGroupBox("Validation", this);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->addWidget(validityIcon_);
    labelLayout->addWidget(validityStatus_);
    labelLayout->addStretch();

    QVBoxLayout* errorLayout = new QVBoxLayout(errorBox);
    errorLayout->addLayout(labelLayout);
    errorLayout->addWidget(errorView_);

    layout->addWidget(new SummaryLabel(tr("Component summary"), this), 0, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(vlnvDisplayer_, 1, 0, 1, 2);
    layout->addWidget(attributeEditor_, 2, 0, 1, 1);
    layout->addWidget(authorBox, 2, 1, 1, 1);    
    layout->addWidget(descriptionBox, 3, 0, 1, 1);
    layout->addWidget(commentBox, 3, 1, 1, 1);
    layout->addWidget(errorBox, 4, 0, 1, 2);

}

//-----------------------------------------------------------------------------
// Function: generaleditor::changeExtensionsEditorItem()
//-----------------------------------------------------------------------------
void GeneralEditor::changeExtensionsEditorItem()
{
    QSharedPointer<Component> editedComponent = component();

    QString extensionID = QLatin1String("Component: ") + editedComponent->getVlnv().toString();
    emit changeVendorExtensions(extensionID, editedComponent);
}
