////-----------------------------------------------------------------------------
// File: ApiDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 29.3.2012
//
// Description:
// Class which encapsulates the handling of custom API definition object.
//-----------------------------------------------------------------------------

#ifndef APIDEFINITION_H
#define APIDEFINITION_H

#include "librarycomponent.h"

#include <QStringList>
#include <QDomDocument>

class VLNV;
class ApiFunction;

//-----------------------------------------------------------------------------
//! Communication definition class.
//-----------------------------------------------------------------------------
class ApiDefinition : public LibraryComponent
{
public:
    /*!
     *  Constructor which creates an empty API definition.
     *
     *      @param [in] vlnv The VLNV of the API definition.
     */
    ApiDefinition(VLNV const& vlnv = VLNV());

    /*!
     *  Copy constructor.
     */
    ApiDefinition(ApiDefinition const& rhs);

    /*!
     *  Constructor which reads the API definition from an XML document.
     *
     *      @param [in] doc The source XML document.
     *
     *      @throw Parse_error when a mandatory element is missing.
     */
    ApiDefinition(QDomDocument& doc);

    /*!
     *  Destructor.
     */
    virtual ~ApiDefinition();

    /*!
     *  Makes a copy of the document.
     *
     *      @return The created copy of the API definition.
     */
    virtual QSharedPointer<LibraryComponent> clone() const;

    /*!
     *  Writes the API definition to an XML file.
     *
     *      @param [in] file The file handle.
     */
    virtual void write(QFile& file);

    /*!
     *  Returns true if the contents are valid.
     *
     *      @param [in/out] errorList Error list which is appended with errors if found while validating.
     */
    virtual bool isValid(QStringList& errorList) const;

    /*!
     *  Returns true if the contents are valid.
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
     *  Sets the API programming language.
     *
     *      @param [in] language The name of the programming language.
     */
    void setLanguage(QString const& language);

    /*!
     *  Sets the COM definition reference.
     *
     *      @param [in] vlnv The VLNV of the COM definition to reference.
     */
    void setComDefinitionRef(VLNV const& vlnv);

    /*!
     *  Adds a new data type to the API definition.
     *
     *      @param [in] type The name of the data type to add.
     */
    void addDataType(QString const& type);

    /*!
     *  Removes a data type from the API definition.
     *
     *      @param [in] type The name of the data type to remove.
     */
    void removeDataType(QString const& type);

    /*!
     *  Sets the API's data types.
     *
     *      @param [in] types A list of data type names.
     */
    void setDataTypes(QStringList const& types);

    /*!
     *  Adds a function to the API.
     *
     *      @param [in] func The function to add.
     */
    void addFunction(QSharedPointer<ApiFunction> func);

    /*!
     *  Removes the function at the given index from the API.
     */
    void removeFunction(int index);

    /*!
     *  Removes all functions.
     */
    void removeFunctions();

    /*!
     *  Returns the API programming language.
     */
    QString const& getLanguage() const;

    /*!
     *  Returns the COM definition reference.
     */
    VLNV const& getComDefinitionRef() const;

    /*!
     *  Returns the supported data types.
     */
    QStringList const& getDataTypes() const;

    /*!
     *  Returns the function at the given index.
     */
    QSharedPointer<ApiFunction> getFunction(int index);

    /*!
     *  Returns the function at the given index.
     */
    QSharedPointer<ApiFunction const> getFunction(int index) const;

    /*!
     *  Returns the number of functions in the API.
     */
    int getFunctionCount() const;
    
private:
    /*!
     *  Parses all found data types from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseDataTypes(QDomNode& node);

    /*!
     *  Parses all found API functions from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseFunctions(QDomNode& node);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the programming language for documentation purposes.
    QString language_;

    //! The COM definition reference.
    VLNV comDefRef_;

    //! The API data types.
    QStringList dataTypes_;

    //! The API functions.
    QList< QSharedPointer<ApiFunction> > functions_;
};

//-----------------------------------------------------------------------------

#endif // APIDEFINITION_H
