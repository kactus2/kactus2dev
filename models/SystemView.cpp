//-----------------------------------------------------------------------------
// File: SystemView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// System view class.
//-----------------------------------------------------------------------------

#include "SystemView.h"

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(QDomNode& viewNode)
    : nameGroup_(viewNode), 
      hierarchyRef_(),
		fileSetRefs_()
{
    for (int i = 0; i < viewNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = viewNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:hierarchyRef")) {
            hierarchyRef_ = General::createVLNV(tempNode, VLNV::DESIGN);
        }
        else if (tempNode.nodeName() == "kactus2:hwViewRef")
        {
            hwViewRef_ = tempNode.childNodes().at(0).nodeValue();
        }
		  else if (tempNode.nodeName() == "kactus2:fileSetRef") {
			  fileSetRefs_.append(tempNode.childNodes().at(0).nodeValue());
		  }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(const QString name)
    : nameGroup_(name),
      hierarchyRef_(),
      hwViewRef_(),
		fileSetRefs_()
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView()
    : nameGroup_(),
      hierarchyRef_(),
      hwViewRef_(),
		fileSetRefs_()
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(const SystemView &other)
    : nameGroup_(other.nameGroup_),
      hierarchyRef_(other.hierarchyRef_),
      hwViewRef_(other.hwViewRef_),
		fileSetRefs_(other.fileSetRefs_)
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::operator=()
//-----------------------------------------------------------------------------
SystemView& SystemView::operator=(const SystemView &other)
{
    if (this != &other)
    {
        nameGroup_ = other.nameGroup_;
        hierarchyRef_ = other.hierarchyRef_;
        hwViewRef_ = other.hwViewRef_;
		  fileSetRefs_ = other.fileSetRefs_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SystemView::~SystemView()
//-----------------------------------------------------------------------------
SystemView::~SystemView()
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::write()
//-----------------------------------------------------------------------------
void SystemView::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("kactus2:systemView");

    writer.writeTextElement("spirit:name", nameGroup_.name_);

    // if display name is defined
    if (!nameGroup_.displayName_.isEmpty())
        writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);

    // if description is defined
    if (!nameGroup_.description_.isEmpty())
        writer.writeTextElement("spirit:description", nameGroup_.description_);

    // write spirit:hierarchyRef if one exists
    writer.writeEmptyElement("spirit:hierarchyRef");
    General::writeVLNVAttributes(writer, &hierarchyRef_);

    // Write HW view reference.
    writer.writeTextElement("kactus2:hwViewRef", hwViewRef_);

	 foreach (QString fileSetName, fileSetRefs_) {
		 writer.writeTextElement("kactus2:fileSetRef", fileSetName);
	 }

    writer.writeEndElement(); // kactus2:systemView
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(const QStringList& fileSetNames, QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("system view %1").arg(nameGroup_.name_));

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for system view in %1").arg(
            parentIdentifier));
        valid = false;
    }

	 // make sure the referenced file sets are found
	 foreach (QString fileSetRef, fileSetRefs_) {
		 if (!fileSetNames.contains(fileSetRef)) {
			 errorList.append(QObject::tr("System View %1 contained reference to file"
				 " set %2 which is not found within %3").arg(
				 nameGroup_.name_).arg(fileSetRef).arg(parentIdentifier));
			 valid = false;
		 }
	 }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(const QStringList& fileSetNames) const
{
    if (nameGroup_.name_.isEmpty())
    {
        return false;
    }

	 // make sure the referenced file sets are found
	 foreach (QString fileSetRef, fileSetRefs_) {
		 if (!fileSetNames.contains(fileSetRef)) {
			 return false;
		 }
	 }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setHierarchyRef()
//-----------------------------------------------------------------------------
void SystemView::setHierarchyRef(const VLNV& hierarchyRef)
{
    hierarchyRef_ = hierarchyRef;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setName()
//-----------------------------------------------------------------------------
void SystemView::setName(const QString &name)
{
    nameGroup_.name_ = name;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setDisplayName()
//-----------------------------------------------------------------------------
void SystemView::setDisplayName( const QString& displayName )
{
    nameGroup_.displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setDescription()
//-----------------------------------------------------------------------------
void SystemView::setDescription( const QString& description )
{
    nameGroup_.description_ = description;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setHWViewRef()
//-----------------------------------------------------------------------------
void SystemView::setHWViewRef(QString const& viewName)
{
    hwViewRef_ = viewName;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getHierarchyRef()
//-----------------------------------------------------------------------------
VLNV SystemView::getHierarchyRef() const
{
    return hierarchyRef_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getName()
//-----------------------------------------------------------------------------
QString const& SystemView::getName() const
{
    return nameGroup_.name_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getDisplayName()
//-----------------------------------------------------------------------------
QString const& SystemView::getDisplayName() const
{
    return nameGroup_.displayName_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getDescription()
//-----------------------------------------------------------------------------
QString const& SystemView::getDescription() const {
    return nameGroup_.description_;
}


//-----------------------------------------------------------------------------
// Function: SystemView::getHWViewRef()
//-----------------------------------------------------------------------------
QString const& SystemView::getHWViewRef() const
{
    return hwViewRef_;
}

General::NameGroup& SystemView::getNameGroup() {
    return nameGroup_;
}

const General::NameGroup& SystemView::getNameGroup() const {
    return nameGroup_;
}

QStringList SystemView::getFileSetRefs() const {
	return fileSetRefs_;
}

void SystemView::setFileSetRefs( const QStringList& fileSetRefs ) {
	fileSetRefs_ = fileSetRefs;
}
