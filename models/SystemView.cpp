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
      hierarchyRef_()
{
    for (int i = 0; i < viewNode.childNodes().count(); ++i)
    {
        QDomNode tempNode = viewNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:hierarchyRef")) {
            hierarchyRef_ = General::createVLNV(tempNode, VLNV::DESIGN);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(const QString name)
    : nameGroup_(name),
      hierarchyRef_()
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView()
    : nameGroup_(),
      hierarchyRef_()
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::SystemView()
//-----------------------------------------------------------------------------
SystemView::SystemView(const SystemView &other)
    : nameGroup_(other.nameGroup_),
      hierarchyRef_(other.hierarchyRef_)
{
}

//-----------------------------------------------------------------------------
// Function: SystemView::operator=()
//-----------------------------------------------------------------------------
SystemView & SystemView::operator=(const SystemView &other)
{
    if (this != &other)
    {
        nameGroup_ = other.nameGroup_;
        hierarchyRef_ = other.hierarchyRef_;
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

    writer.writeEndElement(); // kactus2:systemView
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("system view %1").arg(nameGroup_.name_));

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for system view in %1").arg(
            parentIdentifier));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SystemView::isValid()
//-----------------------------------------------------------------------------
bool SystemView::isValid() const
{
    if (nameGroup_.name_.isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getName()
//-----------------------------------------------------------------------------
QString SystemView::getName() const
{
    return nameGroup_.name_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getHierarchyRef()
//-----------------------------------------------------------------------------
VLNV SystemView::getHierarchyRef() const
{
    return hierarchyRef_;
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
// Function: SystemView::getDisplayName()
//-----------------------------------------------------------------------------
QString SystemView::getDisplayName() const
{
    return nameGroup_.displayName_;
}

//-----------------------------------------------------------------------------
// Function: SystemView::getDescription()
//-----------------------------------------------------------------------------
QString SystemView::getDescription() const {
    return nameGroup_.description_;
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


General::NameGroup& SystemView::getNameGroup() {
    return nameGroup_;
}

const General::NameGroup& SystemView::getNameGroup() const {
    return nameGroup_;
}
