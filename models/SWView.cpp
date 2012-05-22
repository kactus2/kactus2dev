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
SWView::SWView(QDomNode& viewNode)
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
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const QString name)
    : nameGroup_(name),
      hierarchyRef_()
{
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView()
    : nameGroup_(),
      hierarchyRef_()
{
}

//-----------------------------------------------------------------------------
// Function: SWView::SWView()
//-----------------------------------------------------------------------------
SWView::SWView(const SWView &other)
    : nameGroup_(other.nameGroup_),
      hierarchyRef_(other.hierarchyRef_)
{
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
// Function: SWView::write()
//-----------------------------------------------------------------------------
void SWView::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("kactus2:swView");

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

    writer.writeEndElement(); // spirit:swView
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("SW view %1").arg(nameGroup_.name_));

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for SW view in %1").arg(
            parentIdentifier));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: SWView::isValid()
//-----------------------------------------------------------------------------
bool SWView::isValid() const
{
    if (nameGroup_.name_.isEmpty())
    {
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
