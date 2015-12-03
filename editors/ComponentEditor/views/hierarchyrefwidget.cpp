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

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/Choice.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QStringList>
#include <QFormLayout>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::HierarchyRefWidget()
//-----------------------------------------------------------------------------
HierarchyRefWidget::HierarchyRefWidget(QSharedPointer<View> view,
    QSharedPointer<DesignInstantiation> designInstantiation,
    QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation,
    LibraryInterface* libHandler, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget* parent):
QWidget(parent),
library_(libHandler),
view_(view),
designInstantiation_(designInstantiation),
designConfigurationInstantiation_(designConfigurationInstantiation),
designConfigurationEditor_(new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parent, this)),
designEditor_(new VLNVEditor(VLNV::DESIGN, libHandler, parent, this))
{
	designConfigurationEditor_->setTitle(tr("Design configuration"));
	designConfigurationEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    designConfigurationEditor_->setMandatory(false);

    designEditor_->setTitle(tr("Design"));
    designEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    designEditor_->setMandatory(false);
    designEditor_->setEnabled(false);

	connect(designConfigurationEditor_, SIGNAL(vlnvEdited()), this, SLOT(onVLNVChanged()), Qt::UniqueConnection);

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
	return designConfigurationEditor_->isValid() || designEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> HierarchyRefWidget::getDesignInstantiation()
{
    return designInstantiation_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::getDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> HierarchyRefWidget::getDesignConfigurationInstantiation()
{
    return designConfigurationInstantiation_;
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::refresh()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::refresh()
{
    designConfigurationEditor_->setVLNV(*designConfigurationInstantiation_->getDesignConfigurationReference());
    designEditor_->setVLNV(*designInstantiation_->getDesignReference());
    //updateDesignReference();

    updateErrorSignAndTooltip();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::onVLNVChanged()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::onVLNVChanged()
{
    VLNV referenceValue = designConfigurationEditor_->getVLNV();
    QSharedPointer<ConfigurableVLNVReference> designConfigurationReference
        (new ConfigurableVLNVReference(referenceValue));
    designConfigurationInstantiation_->setDesignConfigurationReference(designConfigurationReference);

    updateDesignReference();
    //updateErrorSignAndTooltip();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::clear()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::clear()
{	
	// clear the hierarchy ref
	designConfigurationEditor_->setVLNV(VLNV());
    designEditor_->setVLNV(VLNV());

    designInstantiation_->getDesignReference()->clear();
    designConfigurationInstantiation_->getDesignConfigurationReference()->clear();
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
    VLNV configurationVLNV = *designConfigurationInstantiation_->getDesignConfigurationReference();
    if (library_->contains(configurationVLNV) && 
        library_->getDocumentType(configurationVLNV) == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<DesignConfiguration const> designConfiguration = 
            library_->getModelReadOnly(configurationVLNV).dynamicCast<DesignConfiguration const>();
        

        QSharedPointer<ConfigurableVLNVReference> designReference
            (new ConfigurableVLNVReference(designConfiguration->getDesignRef()));
        designInstantiation_->setDesignReference(designReference);

        designEditor_->setVLNV(designConfiguration->getDesignRef());
    }
    else
    {
        designEditor_->setVLNV(VLNV());
    }

    /*
    if (library_->contains(*designInstantiation_->getDesignReference()))
    {
        designEditor_->setVLNV(*designInstantiation_->getDesignReference());
    }
    else
    {
        designEditor_->setVLNV(VLNV());
    }*/
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::updateErrorSignAndTooltip()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::updateErrorSignAndTooltip()
{
    /*if (!designInstantiation_ || !designConfigurationInstantiation_)
    {
        hideErrorSignAndTooltip();
    }
    else
    {
        showErrorSignAndTooltip();
    }*/
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::showErrorSignAndTooltip()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::showErrorSignAndTooltip()
{
    /*QPalette errorPalette = designConfigurationEditor_->palette();
    errorPalette.setColor(QPalette::Foreground, Qt::red);
    designConfigurationEditor_->setPalette(errorPalette);

    //! Enable showing check indicator that is error sign.
    designConfigurationEditor_->setCheckable(true);

    if (designConfigurationInstantiation_)
    {
        QSharedPointer<VLNV> designConfigurationReference =
            designConfigurationInstantiation_->getDesignConfigurationReference();
        if (!library_->contains(*designConfigurationReference.data()))
        {
            QString tooltipMessage =
                tr("<p>VLNV %1 not found in the library.</p>").arg(designConfigurationReference->toString());
            designConfigurationEditor_->setToolTip(tooltipMessage);
        }
    }

    if (designInstantiation_)
    {
        QSharedPointer<VLNV> designReference = designInstantiation_->getDesignReference();
        if (!library_->contains(*designReference.data()))
        {
            QString tooltipMessage =
                tr("<p>VLNV %1 not found in the library.</p>").arg(designReference->toString());
            designEditor_->setToolTip(tooltipMessage);
        }
    }*/
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::hideErrorSignAndTooltip()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::hideErrorSignAndTooltip()
{
   /* QPalette normalPalette = designConfigurationEditor_->palette();
    normalPalette.setColor(QPalette::Foreground, Qt::black);
    designConfigurationEditor_->setPalette(normalPalette);

    //! Disable showing check indicator that is error sign.
    designConfigurationEditor_->setCheckable(false);

    designConfigurationEditor_->setToolTip("");
    designEditor_->setToolTip("");*/
}

//-----------------------------------------------------------------------------
// Function: HierarchyRefWidget::setupLayout()
//-----------------------------------------------------------------------------
void HierarchyRefWidget::setupLayout()
{
    QGroupBox* hierarchyReferenceGroup = new QGroupBox(tr("Hierarchy reference"), this);
    QHBoxLayout* referenceLayout = new QHBoxLayout(hierarchyReferenceGroup);
    referenceLayout->addWidget(designConfigurationEditor_);
    referenceLayout->addWidget(designEditor_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(hierarchyReferenceGroup);
    topLayout->addStretch();
    topLayout->setContentsMargins(0, 0, 0, 0);
}
