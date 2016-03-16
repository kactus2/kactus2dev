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
SystemView::SystemView(QDomNode& viewNode) : NameGroup(), hierarchyRef_(), fileSetRefs_()
{
    QDomElement systemViewElement = viewNode.toElement();

    setName(systemViewElement.firstChildElement("ipxact:name").firstChild().nodeValue());

    setDisplayName(systemViewElement.firstChildElement("ipxact:displayName").firstChild().nodeValue());

    setDescription(systemViewElement.firstChildElement("ipxact:description").firstChild().nodeValue());
    
    QDomElement hierarchyRefElement = systemViewElement.firstChildElement("ipxact:hierarchyRef");
    if (!hierarchyRefElement.isNull())
    {
        hierarchyRef_ = VLNV::createVLNV(hierarchyRefElement, VLNV::DESIGN);
    }

    hwViewRef_ = systemViewElement.firstChildElement("kactus2:hwViewRef").childNodes().at(0).nodeValue();

    QDomNodeList fileSetRefNodes = systemViewElement.elementsByTagName("kactus2:fileSetRef");
    int fileSetRefCount = fileSetRefNodes.count();
    for (int i = 0; i < fileSetRefCount; i++)
    {
        QDomNode filesetNode = fileSetRefNodes.at(i);
        fileSetRefs_.append(filesetNode.childNodes().at(0).nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(QString const& name): NameGroup(name), hierarchyRef_(), hwViewRef_(), fileSetRefs_()
{

}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(): NameGroup(),  hierarchyRef_(), hwViewRef_(), fileSetRefs_()
{

}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(const SystemView &other) :
NameGroup(other),
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
        NameGroup::operator=(other);
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
// Function: SystemView::clone()
//-----------------------------------------------------------------------------
SystemView* SystemView::clone() const
{
    return new SystemView(*this);
}

//-----------------------------------------------------------------------------
// Function: SystemView::type()
//-----------------------------------------------------------------------------
QString SystemView::type() const
{
    return QString("kactus2:systemView");
}

//-----------------------------------------------------------------------------
// Function: SystemView::write()
//-----------------------------------------------------------------------------
void SystemView::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:systemView");

    writer.writeTextElement("ipxact:name", name());

    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    // write hierarchyRef if one exists
    if (!hierarchyRef_.isEmpty())
    {
        writer.writeEmptyElement("kactus2:hierarchyRef");
        writer.writeAttribute("vendor", hierarchyRef_.getVendor());
        writer.writeAttribute("library", hierarchyRef_.getLibrary());
        writer.writeAttribute("name", hierarchyRef_.getName());
        writer.writeAttribute("version", hierarchyRef_.getVersion());
    }

    // Write HW view reference.
    writer.writeTextElement("kactus2:hwViewRef", hwViewRef_);

	 foreach (QString const& fileSetName, fileSetRefs_)
     {
		 writer.writeTextElement("kactus2:fileSetRef", fileSetName);
	 }

    writer.writeEndElement(); // kactus2:systemView
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(QStringList const& fileSetNames, QStringList const& HWViewNames,
    QVector<QString>& errorList, QString const& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("system view %1").arg(name()));

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for system view in %1").arg(parentIdentifier));
        valid = false;
    }

    if (!hierarchyRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
    }

	 // make sure the referenced file sets are found
	 foreach (QString const& fileSetRef, fileSetRefs_)
     {
		 if (!fileSetNames.contains(fileSetRef))
         {
			 errorList.append(QObject::tr("System View %1 contained reference to file set %2 which is not found "
                 "within %3").arg(name()).arg(fileSetRef).arg(parentIdentifier));
			 valid = false;
		 }
	 }

	 // if HW view is specified but not found
	 if (!hwViewRef_.isEmpty() && !HWViewNames.contains(hwViewRef_))
     {
		 errorList.append(QObject::tr("System view %1 contained reference to HW view %2 which is not found "
             "withing %3.").arg(name()).arg(hwViewRef_).arg(parentIdentifier));
		 valid = false;
	 }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(QStringList const& fileSetNames, QStringList const& HWViewNames) const
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
void SystemView::setHierarchyRef(VLNV const& hierarchyRef)
{
    hierarchyRef_ = hierarchyRef;
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
// Function: SystemView::getHWViewRef()
//-----------------------------------------------------------------------------
QString SystemView::getHWViewRef() const
{
    return hwViewRef_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getFileSetRefs()
//-----------------------------------------------------------------------------
QStringList SystemView::getFileSetRefs() const
{
	return fileSetRefs_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::setFileSetRefs()
//-----------------------------------------------------------------------------
void SystemView::setFileSetRefs(QStringList const& fileSetRefs)
{
	fileSetRefs_ = fileSetRefs;
}
