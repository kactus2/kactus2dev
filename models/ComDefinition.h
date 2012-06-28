//-----------------------------------------------------------------------------
// File: ComDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 29.3.2012
//
// Description:
// Class which encapsulates the handling of custom communication definition
// object.
//-----------------------------------------------------------------------------

#ifndef COMDEFINITION_H
#define COMDEFINITION_H

#include "librarycomponent.h"
#include "ComProperty.h"

#include <common/Global.h>

#include <QStringList>
#include <QList>
#include <QDomDocument>

class VLNV;

//-----------------------------------------------------------------------------
//! Communication definition class.
//-----------------------------------------------------------------------------
class KACTUS2_API ComDefinition : public LibraryComponent
{
public:
    /*!
     *  Constructor which creates an empty communication definition.
     *
     *      @param [in] vlnv The VLNV of the communication definition.
     */
    ComDefinition(VLNV const& vlnv = VLNV());

    /*!
     *  Copy constructor.
     */
    ComDefinition(ComDefinition const& rhs);

    /*!
     *  Constructor which reads the communication definition from an XML document.
     *
     *      @param [in] doc The source XML document.
     *
     *      @throw Parse_error when a mandatory element is missing.
     */
    ComDefinition(QDomDocument& doc);

    /*!
     *  Destructor.
     */
    virtual ~ComDefinition();

    /*!
     *  Makes a copy of the document.
     *
     *      @return The created copy of the COM definition.
     */
    virtual QSharedPointer<LibraryComponent> clone() const;

    /*!
     *  Writes the communication definition to an XML file.
     *
     *      @param [in] file The file handle.
     */
    virtual void write(QFile& file);

    /*!
     *  Returns true if the communication definition is valid.
     *
     *      @param [out] errorList  The list of errors found in the contents.
     */
    virtual bool isValid(QStringList& errorList) const;

    /*!
     *  Returns true if the communication definition is valid.
     */
    virtual bool isValid() const;

    /*!
     *  Returns the dependent files (none).
     */
    virtual QStringList const getDependentFiles() const;

    /*!
     *  Returns the dependent VLNVs (none).
     */
    virtual QList<VLNV> const getDependentVLNVs() const;

    /*!
     *  Adds a new transfer type to the communication definition.
     *
     *      @param [in] type The name of the transfer type to add.
     */
    void addTransferType(QString const& type);

    /*!
     *  Removes a transfer type from the communication definition.
     *
     *      @param [in] type The name of the transfer type to remove.
     */
    void removeTransferType(QString const& type);

    /*!
     *  Sets the supported transfer types.
     *
     *      @param [in] types A list of transfer type names.
     */
    void setTransferTypes(QStringList const& types);

    /*!
     *  Adds a property to the definition.
     *
     *      @param [in] prop The property to add.
     */
    void addProperty(QSharedPointer<ComProperty> prop);

    /*!
     *  Removes a property from the definition.
     *
     *      @param [in] name The name of the property to remove.
     */
    void removeProperty(QString const& name);

    /*!
     *  Sets the properties.
     *
     *      @param [in] properties A list of properties to set.
     */
    void setProperties(QList< QSharedPointer<ComProperty> > const& properties);

    /*!
     *  Returns the supported transfer types.
     */
    QStringList const& getTransferTypes() const;

    /*!
     *  Returns the list of properties.
     */
    QList< QSharedPointer<ComProperty> > const& getProperties() const;

private:
    /*!
     *  Parses all found transfer types from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseTransferTypes(QDomNode& node);

    /*!
     *  Parses all found properties from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseProperties(QDomNode& node);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of transfer types.
    QStringList transferTypes_;

    //! The list of properties.
    QList< QSharedPointer<ComProperty> > properties_;
};

//-----------------------------------------------------------------------------

#endif // COMDEFINITION_H
