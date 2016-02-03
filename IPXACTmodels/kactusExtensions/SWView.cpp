//-----------------------------------------------------------------------------
// File: SWView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view class.
//-----------------------------------------------------------------------------

#include "SWView.h"

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(QDomNode& viewNode):
NameGroup(),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWFileBuilder> >()),
bspCommand_()
{
    QDomElement swViewElement = viewNode.toElement();

    setName(swViewElement.firstChildElement("ipxact:name").firstChild().nodeValue());

    setDisplayName(swViewElement.firstChildElement("ipxact:displayName").firstChild().nodeValue());

    setDescription(swViewElement.firstChildElement("ipxact:description").firstChild().nodeValue());

    QDomElement hierarchyElement = swViewElement.firstChildElement("kactus2:hierarchyRef");
    if (!hierarchyElement.isNull())
    {
        hierarchyRef_ = VLNV::createVLNV(hierarchyElement, VLNV::DESIGN);
    }

    QDomNodeList fileSetRefNodes = swViewElement.elementsByTagName("kactus2:fileSetRef");
    int fileSetRefCount = fileSetRefNodes.count();
    for (int i = 0; i < fileSetRefCount; i++)
    {
        QDomNode filesetNode = fileSetRefNodes.at(i);
        filesetRefs_.append(filesetNode.firstChild().nodeValue());
    }

    QDomNodeList buildCommandNodes = swViewElement.elementsByTagName("kactus2:SWBuildCommand");
    int buildCommandCount = buildCommandNodes.count();
    for (int i = 0; i < buildCommandCount; i++)
    {
        QDomNode buildCommandNode = buildCommandNodes.at(i);
        swBuildCommands_->append(QSharedPointer<SWFileBuilder>(new SWFileBuilder(buildCommandNode)));
    }

    QDomElement bspCommandElement = swViewElement.firstChildElement("kactus2:BSPBuildCommand");
    if (!bspCommandElement.isNull())
    {
        bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(bspCommandElement));
    }
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(QString const& name):
NameGroup(name),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWFileBuilder> > ()),
bspCommand_()
{

}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView():
NameGroup(),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWFileBuilder> > ()),
bspCommand_()
{

}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const SWView &other):
NameGroup(other),
hierarchyRef_(other.hierarchyRef_),
filesetRefs_(other.filesetRefs_),
swBuildCommands_(new QList<QSharedPointer<SWFileBuilder> > ()),
bspCommand_()
{
    copySwBuildCommands(other);
    copyBSPBuildCommand(other);
}

//-----------------------------------------------------------------------------
// Function: SWView::operator=()
//-----------------------------------------------------------------------------
SWView & SWView::operator=(const SWView &other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        hierarchyRef_ = other.hierarchyRef_;
        filesetRefs_ = other.filesetRefs_;
        
        swBuildCommands_->clear();
        copySwBuildCommands(other);
        
        bspCommand_.clear();
        copyBSPBuildCommand(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SWView::~SWView()
//-----------------------------------------------------------------------------
SWView::~SWView()
{

}

//-----------------------------------------------------------------------------
// Function: SWView::clone()
//-----------------------------------------------------------------------------
SWView* SWView::clone() const
{
    return new SWView(*this);
}

//-----------------------------------------------------------------------------
// Function: SWView::type()
//-----------------------------------------------------------------------------
QString SWView::type() const
{
    return QString("kactus2:swView");
}

//-----------------------------------------------------------------------------
// Function: SWView::write()
//-----------------------------------------------------------------------------
void SWView::write(QXmlStreamWriter& writer) const
{
	writer.writeStartElement("kactus2:swView");

    writer.writeTextElement("ipxact:name", name());

    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }

    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    // write ipxact:hierarchyRef if one exists
	if (hierarchyRef_.isValid())
    {
        writer.writeEmptyElement("kactus2:hierarchyRef");
        writer.writeAttribute("vendor", hierarchyRef_.getVendor());
        writer.writeAttribute("library", hierarchyRef_.getLibrary());
        writer.writeAttribute("name", hierarchyRef_.getName());
        writer.writeAttribute("version", hierarchyRef_.getVersion());
	}

	// write the file set references
	foreach (QString fileSetName, filesetRefs_)
    {
        writer.writeTextElement("kactus2:fileSetRef", fileSetName);
	}

	// write all SW build commands
	foreach (QSharedPointer<SWFileBuilder> buildCommand, *swBuildCommands_)
    {
        buildCommand->write(writer);
	}

	// BSP only exists on HW components
    if (bspCommand_)
    {
        bspCommand_->write(writer);
	}

	writer.writeEndElement(); // ipxact:swView
}

//-----------------------------------------------------------------------------
// Function: SWView::findErrorsIn()
//-----------------------------------------------------------------------------
bool SWView::findErrorsIn(QStringList const& fileSetNames, QStringList const& cpuNames, QVector<QString>& errorList,
    QString const& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("SW view %1").arg(name()));

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for SW view in %1").arg(parentIdentifier));
        valid = false;
    }

    if (!hierarchyRef_.isEmpty() && !hierarchyRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
    }

	// make sure the referenced file sets are found
	foreach (QString fileSetRef, filesetRefs_)
    {
		if (!fileSetNames.contains(fileSetRef))
        {
            errorList.append(QObject::tr("SW View %1 contained reference to file set %2 which is not found "
                "within %3").arg(name()).arg(fileSetRef).arg(parentIdentifier));
            valid = false;
		}
	}

	foreach (QSharedPointer<SWFileBuilder> swCom, *swBuildCommands_)
    {
		if (swCom && !swCom->isValid(errorList, thisIdentifier))
        {
			valid = false;
		}
	}

	if (bspCommand_ && !bspCommand_->isValid(cpuNames, errorList, thisIdentifier))
    {
		valid = false;
	}

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid(QStringList const& fileSetNames, QStringList const& cpuNames) const
{
    if (name().isEmpty())
    {
        return false;
    }

    if (!hierarchyRef_.isEmpty() && !hierarchyRef_.isValid())
    {
        return false;
    }

	// make sure the referenced file sets are found
	foreach (QString fileSetRef, filesetRefs_)
    {
		if (!fileSetNames.contains(fileSetRef))
        {
			return false;
		}
	}

	foreach (QSharedPointer<SWFileBuilder> swCom, *swBuildCommands_)
    {
		if (swCom && !swCom->isValid())
        {
			return false;
		}
	}

	if (bspCommand_ && !bspCommand_->isValid(cpuNames))
    {
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWView::getHierarchyRef()
//-----------------------------------------------------------------------------
VLNV SWView::getHierarchyRef() const
{
    return hierarchyRef_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setHierarchyRef()
//-----------------------------------------------------------------------------
void SWView::setHierarchyRef(VLNV const& hierarchyRef)
{
    hierarchyRef_ = hierarchyRef;
}

//-----------------------------------------------------------------------------
// Function: SWView::getFileSetRefs()
//-----------------------------------------------------------------------------
QStringList SWView::getFileSetRefs() const
{
	return filesetRefs_;
}

//-----------------------------------------------------------------------------
// Function: SWView::getFileSetRefs()
//-----------------------------------------------------------------------------
QStringList SWView::getFileSetRefs()
{
    return filesetRefs_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setFileSetRefs()
//-----------------------------------------------------------------------------
void SWView::setFileSetRefs(QStringList const& fileSetRefs)
{
    filesetRefs_.clear();
	filesetRefs_ = fileSetRefs;
}

//-----------------------------------------------------------------------------
// Function: SWView::getSWBuildCommands()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > SWView::getSWBuildCommands() const
{
	return swBuildCommands_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setSWBuildCommands()
//-----------------------------------------------------------------------------
void SWView::setSWBuildCommands(QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > newSWBuildCommands)
{
    swBuildCommands_ = newSWBuildCommands;
}

//-----------------------------------------------------------------------------
// Function: SWView::getBSPBuildCommand()
//-----------------------------------------------------------------------------
QSharedPointer<BSPBuildCommand> SWView::getBSPBuildCommand()
{
    if (!bspCommand_)
    {
		bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand());
	}

	return bspCommand_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setBSPBuildCommand()
//-----------------------------------------------------------------------------
void SWView::setBSPBuildCommand(QSharedPointer<BSPBuildCommand> newCommand)
{
    bspCommand_ = newCommand;
}

//-----------------------------------------------------------------------------
// Function: SWView::addFileSetRef()
//-----------------------------------------------------------------------------
void SWView::addFileSetRef(QString const& fileSetName)
{
	if (filesetRefs_.contains(fileSetName))
    {
		return;
	}
	filesetRefs_.append(fileSetName);
}

//-----------------------------------------------------------------------------
// Function: SWView::copySwBuildCommands()
//-----------------------------------------------------------------------------
void SWView::copySwBuildCommands(const SWView& other)
{
    foreach (QSharedPointer<SWFileBuilder> com, *other.swBuildCommands_)
    {
        if (com)
        {
            QSharedPointer<SWFileBuilder> copy(new SWFileBuilder(*com.data()));
            swBuildCommands_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWView::copyBSPBuildCommand()
//-----------------------------------------------------------------------------
void SWView::copyBSPBuildCommand(const SWView& other)
{
    if (other.bspCommand_)
    {
        bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(*other.bspCommand_.data()));
    }
}
