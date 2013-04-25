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
nameGroup_(viewNode), 
	hierarchyRef_(),
	filesetRefs_(),
	swBuildCommands_(),
	bspCommand_()
{
    for (int i = 0; i < viewNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = viewNode.childNodes().at(i);

		  if (tempNode.isComment()) {
			  continue;
		  }

        else if (tempNode.nodeName() == QString("spirit:hierarchyRef")) {
            hierarchyRef_ = General::createVLNV(tempNode, VLNV::DESIGN);
        }
		  else if (tempNode.nodeName() == QString("kactus2:fileSetRef")) {
			  QString ref = tempNode.childNodes().at(0).nodeValue();
			  ref = General::removeWhiteSpace(ref);
			  filesetRefs_.append(ref);
		  }
		  else if (tempNode.nodeName() == QString("kactus2:SWBuildCommand")) {
			  QSharedPointer<SWBuildCommand> com(new SWBuildCommand(tempNode));
			  swBuildCommands_.append(com);
		  }
		  else if (tempNode.nodeName() == QString("kactus2:BSPBuildCommand")) {
			  bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(tempNode));
		  }
	 }
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const QString& name):
nameGroup_(name),
	hierarchyRef_(),
	filesetRefs_(),
	swBuildCommands_(),
	bspCommand_()
{
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView():
nameGroup_(),
	hierarchyRef_(),
	filesetRefs_(),
	swBuildCommands_(),
	bspCommand_()
{
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const SWView &other):
nameGroup_(other.nameGroup_),
	hierarchyRef_(other.hierarchyRef_),
	filesetRefs_(other.filesetRefs_),
swBuildCommands_(),
bspCommand_() {

	foreach (QSharedPointer<SWBuildCommand> com, other.swBuildCommands_) {
		if (com) {
			QSharedPointer<SWBuildCommand> copy(new SWBuildCommand(*com.data()));
			swBuildCommands_.append(copy);
		}
	}

	if (other.bspCommand_) {
		bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(*other.bspCommand_.data()));
	}
}

//-----------------------------------------------------------------------------
// Function: SWView::operator=()
//-----------------------------------------------------------------------------
SWView & SWView::operator=(const SWView &other)
{
    if (this != &other)
    {
        nameGroup_ = other.nameGroup_;
        hierarchyRef_ = other.hierarchyRef_;
		  filesetRefs_ = other.filesetRefs_;

		  foreach (QSharedPointer<SWBuildCommand> com, other.swBuildCommands_) {
			  if (com) {
				  QSharedPointer<SWBuildCommand> copy(new SWBuildCommand(*com.data()));
				  swBuildCommands_.append(copy);
			  }
		  }

		  bspCommand_.clear();
		  if (other.bspCommand_) {
			  bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand(*other.bspCommand_.data()));
		  }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SWView::~SWView()
//-----------------------------------------------------------------------------
SWView::~SWView() {
}

//-----------------------------------------------------------------------------
// Function: SWView::write()
//-----------------------------------------------------------------------------
void SWView::write( QXmlStreamWriter& writer, bool withinHWComp ) {
	writer.writeStartElement("kactus2:swView");

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	// if display name is defined
	if (!nameGroup_.displayName_.isEmpty())
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);

	// if description is defined
	if (!nameGroup_.description_.isEmpty())
		writer.writeTextElement("spirit:description", nameGroup_.description_);

	// write spirit:hierarchyRef if one exists
	if (hierarchyRef_.isValid()) {
		writer.writeEmptyElement("spirit:hierarchyRef");
		General::writeVLNVAttributes(writer, &hierarchyRef_);
	}

	// write the file set references
	foreach (QString fileSetName, filesetRefs_) {
		writer.writeTextElement("kactus2:fileSetRef", fileSetName);
	}

	// write all SW build commands
	foreach (QSharedPointer<SWBuildCommand> com, swBuildCommands_) {
		com->write(writer, withinHWComp);
	}

	// BSP only exists on HW components
	if (withinHWComp && bspCommand_) {
		bspCommand_->write(writer);
	}

	writer.writeEndElement(); // spirit:swView
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid(const QStringList& fileSetNames, 
	const QStringList& cpuNames,
	QStringList& errorList, 
	const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("SW view %1").arg(nameGroup_.name_));

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for SW view in %1").arg(
            parentIdentifier));
        valid = false;
    }

	// make sure the referenced file sets are found
	foreach (QString fileSetRef, filesetRefs_) {
		if (!fileSetNames.contains(fileSetRef)) {
			errorList.append(QObject::tr("SW View %1 contained reference to file"
				" set %2 which is not found within %3").arg(
				nameGroup_.name_).arg(fileSetRef).arg(parentIdentifier));
			valid = false;
		}
	}

	foreach (QSharedPointer<SWBuildCommand> swCom, swBuildCommands_) {
		if (swCom && !swCom->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	if (bspCommand_ && !bspCommand_->isValid(cpuNames, errorList, thisIdentifier)) {
		valid = false;
	}

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid(const QStringList& fileSetNames, const QStringList& cpuNames) const
{
    if (nameGroup_.name_.isEmpty()) {
        return false;
    }

	// make sure the referenced file sets are found
	foreach (QString fileSetRef, filesetRefs_) {
		if (!fileSetNames.contains(fileSetRef)) {
			return false;
		}
	}

	foreach (QSharedPointer<SWBuildCommand> swCom, swBuildCommands_) {
		if (swCom && !swCom->isValid()) {
			return false;
		}
	}

	if (bspCommand_ && !bspCommand_->isValid(cpuNames)) {
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWView::getName()
//-----------------------------------------------------------------------------
QString SWView::getName() const
{
    return nameGroup_.name_;
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
// Function: SWView::setName()
//-----------------------------------------------------------------------------
void SWView::setName(const QString &name)
{
    nameGroup_.name_ = name;
}

//-----------------------------------------------------------------------------
// Function: SWView::getDisplayName()
//-----------------------------------------------------------------------------
QString SWView::getDisplayName() const
{
    return nameGroup_.displayName_;
}

//-----------------------------------------------------------------------------
// Function: SWView::getDescription()
//-----------------------------------------------------------------------------
QString SWView::getDescription() const {
    return nameGroup_.description_;
}

//-----------------------------------------------------------------------------
// Function: SWView::setDisplayName()
//-----------------------------------------------------------------------------
void SWView::setDisplayName( const QString& displayName )
{
    nameGroup_.displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: SWView::setDescription()
//-----------------------------------------------------------------------------
void SWView::setDescription( const QString& description )
{
    nameGroup_.description_ = description;
}

General::NameGroup& SWView::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& SWView::getNameGroup() const {
	return nameGroup_;
}

const QStringList& SWView::getFileSetRefs() const {
	return filesetRefs_;
}

QStringList SWView::getFileSetRefs() {
	return filesetRefs_;
}

void SWView::setFileSetRefs( const QStringList& fileSetRefs ) {
	filesetRefs_.clear();
	filesetRefs_ = fileSetRefs;
}

QList<QSharedPointer<SWBuildCommand> >& SWView::getSWBuildCommands() {
	return swBuildCommands_;
}

const QList<QSharedPointer<SWBuildCommand> >& SWView::getSWBuildCommands() const {
	return swBuildCommands_;
}

QSharedPointer<BSPBuildCommand> SWView::getBSPBuildCommand() {
	if (!bspCommand_) {
		bspCommand_ = QSharedPointer<BSPBuildCommand>(new BSPBuildCommand());
	}

	return bspCommand_;
}

const QSharedPointer<BSPBuildCommand> SWView::getBSPBuildCommand() const {
	return bspCommand_;
}

void SWView::addFileSetRef( const QString& fileSetName ) {
	// if the reference already exists
	if (filesetRefs_.contains(fileSetName)) {
		return;
	}
	filesetRefs_.append(fileSetName);
}
