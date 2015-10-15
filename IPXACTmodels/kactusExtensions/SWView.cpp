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

#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(QDomNode& viewNode):
NameGroup(),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWBuildCommand> > ()),
bspCommand_(),
isWithinHWComponent_(false)
{
    for (int i = 0; i < viewNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = viewNode.childNodes().at(i);
        
        if (tempNode.isComment())
        {
            continue;
        }
        else if (tempNode.nodeName() == "ipxact:name")
        {
            setName(tempNode.firstChild().nodeValue());
        }
        else if (tempNode.nodeName() == "ipxact:displayName")
        {
            setDisplayName(tempNode.firstChild().nodeValue());
        }
        else if (tempNode.nodeName() == "ipxact:description")
        {
            setDescription(tempNode.firstChild().nodeValue());
        }
        else if (tempNode.nodeName() == QString("kactus2:hierarchyRef"))
        {
            hierarchyRef_ = VLNV::createVLNV(tempNode, VLNV::DESIGN);
        }
        else if (tempNode.nodeName() == QString("kactus2:fileSetRef"))
        {
            QString ref = tempNode.childNodes().at(0).nodeValue();
            ref = removeWhiteSpace(ref);
            filesetRefs_.append(ref);
        }
        else if (tempNode.nodeName() == QString("kactus2:SWBuildCommand"))
        {
            QSharedPointer<SWBuildCommand> com(new SWBuildCommand(tempNode));
            swBuildCommands_->append(com);
        }
        else if (tempNode.nodeName() == QString("kactus2:BSPBuildCommand"))
        {
            bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(tempNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const QString& name):
NameGroup(name),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWBuildCommand> > ()),
bspCommand_(),
isWithinHWComponent_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView():
NameGroup(),
hierarchyRef_(),
filesetRefs_(),
swBuildCommands_(new QList<QSharedPointer<SWBuildCommand> > ()),
bspCommand_(),
isWithinHWComponent_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const SWView &other):
NameGroup(other),
hierarchyRef_(other.hierarchyRef_),
filesetRefs_(other.filesetRefs_),
swBuildCommands_(new QList<QSharedPointer<SWBuildCommand> > ()),
bspCommand_(),
isWithinHWComponent_(other.isWithinHWComponent_)
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
        isWithinHWComponent_ = other.isWithinHWComponent_;
        
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

    // write spirit:hierarchyRef if one exists
	if (hierarchyRef_.isValid())
    {
        writer.writeEmptyElement("kactus2:hierarchyRef");
		hierarchyRef_.writeAsAttributes(writer);
	}

	// write the file set references
	foreach (QString fileSetName, filesetRefs_)
    {
        writer.writeTextElement("kactus2:fileSetRef", fileSetName);
	}

	// write all SW build commands
	foreach (QSharedPointer<SWBuildCommand> com, *swBuildCommands_)
    {
        com->write(writer, isWithinHWComponent_);
	}

	// BSP only exists on HW components
    if (isWithinHWComponent_ && bspCommand_)
    {
        bspCommand_->write(writer);
	}

	writer.writeEndElement(); // spirit:swView
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid(const QStringList& fileSetNames, const QStringList& cpuNames, QStringList& errorList,
    const QString& parentIdentifier) const
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

	foreach (QSharedPointer<SWBuildCommand> swCom, *swBuildCommands_)
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
bool SWView::isValid(const QStringList& fileSetNames, const QStringList& cpuNames) const
{
    if (name().isEmpty())
    {
        return false;
    }

    if (!hierarchyRef_.isValid())
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

	foreach (QSharedPointer<SWBuildCommand> swCom, *swBuildCommands_)
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
void SWView::setHierarchyRef(const VLNV& hierarchyRef)
{
    hierarchyRef_ = hierarchyRef;
}

//-----------------------------------------------------------------------------
// Function: SWView::getFileSetRefs()
//-----------------------------------------------------------------------------
const QStringList& SWView::getFileSetRefs() const
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
void SWView::setFileSetRefs( const QStringList& fileSetRefs )
{
    filesetRefs_.clear();
	filesetRefs_ = fileSetRefs;
}

//-----------------------------------------------------------------------------
// Function: SWView::getSWBuildCommands()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<SWBuildCommand> > > SWView::getSWBuildCommands() const
{
	return swBuildCommands_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setSWBuildCommands()
//-----------------------------------------------------------------------------
void SWView::setSWBuildCommands(QSharedPointer<QList<QSharedPointer<SWBuildCommand> > > newSWBuildCommands)
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
void SWView::addFileSetRef( const QString& fileSetName )
{
	if (filesetRefs_.contains(fileSetName))
    {
		return;
	}
	filesetRefs_.append(fileSetName);
}

//-----------------------------------------------------------------------------
// Function: SWView::setViewInHWComponentStatus()
//-----------------------------------------------------------------------------
void SWView::setViewInHWComponentStatus(bool newWithinHWStatus)
{
    isWithinHWComponent_ = newWithinHWStatus;
}

//-----------------------------------------------------------------------------
// Function: SWView::copySwBuildCommands()
//-----------------------------------------------------------------------------
void SWView::copySwBuildCommands(const SWView& other)
{
    foreach (QSharedPointer<SWBuildCommand> com, *other.swBuildCommands_)
    {
        if (com)
        {
            QSharedPointer<SWBuildCommand> copy(new SWBuildCommand(*com.data()));
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

//-----------------------------------------------------------------------------
// Function: SWView::removeWhiteSpace()
//-----------------------------------------------------------------------------
QString SWView::removeWhiteSpace(QString str)
{
    QTextStream stream(&str);
    QString resultStr;

    str = str.trimmed();

    while (!stream.atEnd())
    {
        QString temp;

        stream.skipWhiteSpace();
        stream >> temp;

        if (!stream.atEnd())
        {
            temp.append("_");
        }
        resultStr += temp;
    }

    return resultStr;
}