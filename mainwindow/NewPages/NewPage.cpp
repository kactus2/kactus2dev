//-----------------------------------------------------------------------------
// File: NewPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.8.2013
//
// Description:
// Base class for all New pages.
//-----------------------------------------------------------------------------

#include "NewPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <IPXACTmodels/common/VLNV.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: NewPage()
//-----------------------------------------------------------------------------
NewPage::NewPage(LibraryInterface* libInterface, VLNV::IPXactType vlnvType, QString title, QString description,
    QWidget* parentDlg):
PropertyPageView(),
vlnvEditor_(new VLNVEditor(vlnvType, libInterface, parentDlg, this, true)),
libInterface_(libInterface),
librarySelector_(new LibrarySelectorWidget(this)),
titleLabel_(new QLabel(title,this)),
descLabel_(new QLabel(description,this))
{   
    QFont font = titleLabel_->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel_->setFont(font);

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

	connect(librarySelector_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~NewPage()
//-----------------------------------------------------------------------------
NewPage::~NewPage()
{

}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && librarySelector_->isValid());
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewPage::validate()
{
    Q_ASSERT(prevalidate());

    return isUnusedVLNV(vlnvEditor_->getVLNV());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    librarySelector_->reset();
    vlnvEditor_->setVLNV(VLNV());
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewPage::updateDirectory()
{
    QString vlnvDir;

    VLNV vlnv = vlnvEditor_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        vlnvDir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            vlnvDir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                vlnvDir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    vlnvDir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    librarySelector_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: NewPage::isUnusedVLNV()
//-----------------------------------------------------------------------------
bool NewPage::isUnusedVLNV(VLNV const& vlnv) const
{
    return !libInterface_->contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NewPage::selectedPath()
//-----------------------------------------------------------------------------
QString NewPage::selectedPath() const
{
    return librarySelector_->getPath();
}

//-----------------------------------------------------------------------------
// Function: NewPage::showErrorForReservedVLVN()
//-----------------------------------------------------------------------------
void NewPage::showErrorForReservedVLVN(VLNV const& vlnv)
{
    QString type = type2Show(vlnv.getType());
    QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
        tr("The %1 cannot be created because the VLNV %2" 
        " already exists in the library.").arg(type, vlnv.toString()), QMessageBox::Ok, this);
    msgBox.exec();
}

//-----------------------------------------------------------------------------
// Function: NewPage::type2Show()
//-----------------------------------------------------------------------------
QString NewPage::type2Show(VLNV::IPXactType const& type)
{
    if (type == VLNV::BUSDEFINITION)
    {
        return QObject::tr("Bus Definition");
    }
    else if (type == VLNV::COMPONENT)
    {
        return QObject::tr("Component");
    }
    else if (type == VLNV::DESIGN)
    {
        return QObject::tr("Design");
    }
    else if (type == VLNV::GENERATORCHAIN)
    {
        return QObject::tr("Generator Chain");
    }
    else if (type == VLNV::ABSTRACTOR)
    {
        return QObject::tr("Abstractor");
    }
    else if (type == VLNV::DESIGNCONFIGURATION)
    {
        return QObject::tr("Design Configuration");
    }
    else if (type == VLNV::ABSTRACTIONDEFINITION)
    {
        return QObject::tr("Abstraction Definition");
    }
    else if (type == VLNV::COMDEFINITION)
    {
        return QObject::tr("COM Definition");
    }
    else if (type == VLNV::APIDEFINITION)
    {
        return QObject::tr("API Definition");
    }
    else
    {
        return QObject::tr("Invalid");
    }
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void NewPage::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel_);
    layout->addWidget(descLabel_);
    layout->addSpacing(12);
    layout->addWidget(vlnvEditor_);
    layout->addSpacing(12);
    layout->addWidget(librarySelector_);
    layout->addStretch(1);
}
