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
            hierarchyRef_ = VLNV::createVLNV(tempNode, VLNV::DESIGN);
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

    writer.writeTextElement("spirit:name", nameGroup_.name());

    // if display name is defined
    if (!nameGroup_.displayName().isEmpty())
        writer.writeTextElement("spirit:displayName", nameGroup_.displayName());

    // if description is defined
    if (!nameGroup_.description().isEmpty())
        writer.writeTextElement("spirit:description", nameGroup_.description());

    // write spirit:hierarchyRef if one exists
    writer.writeEmptyElement("spirit:hierarchyRef");
    hierarchyRef_.writeAsAttributes(writer);

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
bool SystemView::isValid(const QStringList& fileSetNames, const QStringList& HWViewNames,
    QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("system view %1").arg(nameGroup_.name()));

    if (nameGroup_.name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for system view in %1").arg(parentIdentifier));
        valid = false;
    }

    if (!hierarchyRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
    }

	 // make sure the referenced file sets are found
	 foreach (QString fileSetRef, fileSetRefs_)
     {
		 if (!fileSetNames.contains(fileSetRef))
         {
			 errorList.append(QObject::tr("System View %1 contained reference to file set %2 which is not found "
                 "within %3").arg(nameGroup_.name()).arg(fileSetRef).arg(parentIdentifier));
			 valid = false;
		 }
	 }

	 // if HW view is specified but not found
	 if (!hwViewRef_.isEmpty() &&  !HWViewNames.contains(hwViewRef_))
     {
		 errorList.append(QObject::tr("System view %1 contained reference to HW view %2 which is not found "
             "withing %3.").arg(nameGroup_.name()).arg(hwViewRef_).arg(parentIdentifier));
		 valid = false;
	 }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(const QStringList& fileSetNames, const QStringList& HWViewNames) const
{
    if (nameGroup_.name().isEmpty())
    {
        return false;
    }

    if (!hierarchyRef_.isValid())
    {
        return false;
    }

	 // make sure the referenced file sets are found
	 foreach (QString fileSetRef, fileSetRefs_)
     {
		 if (!fileSetNames.contains(fileSetRef))
         {
			 return false;
		 }
	 }

	 // if HW view is specified but not found
	 if (!hwViewRef_.isEmpty() && !HWViewNames.contains(hwViewRef_))
     {
		 return false;
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
    nameGroup_.setName(name);
}

//-----------------------------------------------------------------------------
// Function: SystemView::setDisplayName()
//-----------------------------------------------------------------------------
void SystemView::setDisplayName( const QString& displayName )
{
    nameGroup_.setDisplayName(displayName);
}

//-----------------------------------------------------------------------------
// Function: SystemView::setDescription()
//-----------------------------------------------------------------------------
void SystemView::setDescription( const QString& description )
{
    nameGroup_.setDescription(description);
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
QString SystemView::getName() const
{    
    return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: SystemView::getDisplayName()
//-----------------------------------------------------------------------------
QString SystemView::getDisplayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: SystemView::getDescription()
//-----------------------------------------------------------------------------
QString SystemView::getDescription() const
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: SystemView::getHWViewRef()
//-----------------------------------------------------------------------------
QString const& SystemView::getHWViewRef() const
{
    return hwViewRef_;
}

NameGroup& SystemView::getNameGroup() {
    return nameGroup_;
}

QStringList SystemView::getFileSetRefs() const {
	return fileSetRefs_;
}

void SystemView::setFileSetRefs( const QStringList& fileSetRefs ) {
	fileSetRefs_ = fileSetRefs;
}
