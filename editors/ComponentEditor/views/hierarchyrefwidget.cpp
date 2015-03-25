//-----------------------------------------------------------------------------
// File: hierarchyrefwidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2011
//
// Description:
// Editor to set the hierarchy reference for a hierarchical view.
//-----------------------------------------------------------------------------

#include "hierarchyrefwidget.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QStringList>
#include <QFormLayout>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::HierarchyRefWidget()
//-----------------------------------------------------------------------------
HierarchyRefWidget::HierarchyRefWidget(QSharedPointer<View> view, 
									   LibraryInterface* libHandler,
									   QWidget *parent): 
QWidget(parent),
    library_(libHandler),
    view_(view),
    designConfigurationEditor_(new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parent->parentWidget(), this)),
    designReferenceDisplay_(new VLNVEditor(VLNV::DESIGN, libHandler, parent->parentWidget(), this)),
    topLevelRef_(new QComboBox(this))
{
	designConfigurationEditor_->setTitle(tr("Design configuration"));
	designConfigurationEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    designReferenceDisplay_->setTitle(tr("Design"));
    designReferenceDisplay_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    designReferenceDisplay_->setMandatory(false);
    designReferenceDisplay_->setEnabled(false);

	connect(designConfigurationEditor_, SIGNAL(vlnvEdited()), this, SLOT(onVLNVChanged()), Qt::UniqueConnection);
	connect(topLevelRef_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onTopViewChanged(QString const&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::~HierarchyRefWidget()
//-----------------------------------------------------------------------------
HierarchyRefWidget::~HierarchyRefWidget()
{

}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::isValid()
//-----------------------------------------------------------------------------
bool HierarchyRefWidget::isValid() const
{
	return designConfigurationEditor_->isValid() || designReferenceDisplay_->isValid();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::refresh()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::refresh(QStringList const& availableViews)
{	
    VLNV viewReference = view_->getHierarchyRef();
    if (library_->getDocumentType(viewReference) == VLNV::DESIGNCONFIGURATION)
    {
        designConfigurationEditor_->setVLNV(viewReference);
        updateDesignReference();
    }
    else
    {
        designConfigurationEditor_->setVLNV(VLNV());
        designReferenceDisplay_->setVLNV(viewReference);
    }

	// before changing the index, the editor must be disconnected
	disconnect(topLevelRef_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onTopViewChanged(QString const&)));

	// remove the previous values from the combo box
	topLevelRef_->clear();

	// Set the available views from the component.
	QStringList viewList;	
	viewList.append(QString(""));   //<! If user does not want to reference any view.
	viewList.append(availableViews);
    viewList.removeAll(view_->getName()); //<! Cannot refer to this view.

	topLevelRef_->addItems(viewList);

	// Find the text in the combo box and select it.
	int index = topLevelRef_->findText(view_->getTopLevelView());
	topLevelRef_->setCurrentIndex(index);

	// after change reconnect the editor
	connect(topLevelRef_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onTopViewChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::onVLNVChanged()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::onVLNVChanged()
{
	view_->setHierarchyRef(designConfigurationEditor_->getVLNV());

    updateDesignReference();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::onTopViewChanged()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::onTopViewChanged(QString const& newViewName)
{
	view_->setTopLevelView(newViewName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::clear()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::clear()
{	
	// clear the hierarchy ref
	designConfigurationEditor_->setVLNV(VLNV());
    designReferenceDisplay_->setVLNV(VLNV());

	// before changing the index, the editor must be disconnected
	disconnect(topLevelRef_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onTopViewChanged(QString const&)));

	// clear the top level ref
	topLevelRef_->clear();

	// after change reconnect the editor
	connect(topLevelRef_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onTopViewChanged(QString const&)), Qt::UniqueConnection);

	view_->clearHierarchy();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::showEvent()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/hierview.html");
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::updateDesignReference()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::updateDesignReference()
{
    QSharedPointer<const DesignConfiguration> designConfiguration;

    if (library_->contains(view_->getHierarchyRef()))
    {
        QSharedPointer<const LibraryComponent> libraryModel = library_->getModelReadOnly(view_->getHierarchyRef());
        designConfiguration = libraryModel.dynamicCast<const DesignConfiguration>();
    }

    if (designConfiguration)
    {
        designReferenceDisplay_->setVLNV(designConfiguration->getDesignRef());
    }
    else
    {
        designReferenceDisplay_->setVLNV(VLNV());
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::setupLayout()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::setupLayout()
{
    QGroupBox* hierarchyReferenceGroup = new QGroupBox(tr("Hierarchy reference"), this);
    QHBoxLayout* referenceLayout = new QHBoxLayout(hierarchyReferenceGroup);
    referenceLayout->addWidget(designConfigurationEditor_);
    referenceLayout->addWidget(designReferenceDisplay_);

    QFormLayout* extensionLayout = new QFormLayout();
    extensionLayout->addRow(tr("VendorExtension: Reference to a top-level implementation view"), topLevelRef_);

    QGridLayout* halfPageLayout = new QGridLayout();
    halfPageLayout->addLayout(extensionLayout, 0, 0, 1, 1);
    halfPageLayout->setColumnStretch(0, 50);
    halfPageLayout->setColumnStretch(1, 50);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(hierarchyReferenceGroup);
    topLayout->addLayout(halfPageLayout);
    topLayout->addStretch();
    topLayout->setContentsMargins(0, 0, 0, 0);
}
