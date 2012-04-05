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

#include <QStringList>
#include <QList>
#include <QDomDocument>

class VLNV;
class ComProperty;

//-----------------------------------------------------------------------------
//! Communication definition class.
//-----------------------------------------------------------------------------
class ComDefinition : public LibraryComponent
{
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
    bool isValid(QStringList& errorList) const;

    /*!
     *  Returns true if the communication definition is valid.
     */
    bool isValid() const;

    /*!
     *  Adds a new data type to the communication definition.
     *
     *      @param [in] type The name of the data type to add.
     */
    void addDataType(QString const& type);

    /*!
     *  Removes a data type from the communication definition.
     *
     *      @param [in] type The name of the data type to remove.
     */
    void removeDataType(QString const& type);

    /*!
     *  Sets the supported data types.
     *
     *      @param [in] types A list of data type names.
     */
    void setDataTypes(QStringList const& types);

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
     *  Returns the supported data types.
     */
    QStringList const& getDataTypes() const;

    /*!
     *  Returns the list of properties.
     */
    QList< QSharedPointer<ComProperty> > getProperties() const;

private:
    /*!
     *  Parses all found data types from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseDataTypes(QDomNode& node);

    /*!
     *  Parses all found properties from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseProperties(QDomNode& node);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of data types.
    QStringList dataTypes_;

    //! The list of properties.
    QList< QSharedPointer<ComProperty> > properties_;
};

//-----------------------------------------------------------------------------

#endif // COMDEFINITION_H
