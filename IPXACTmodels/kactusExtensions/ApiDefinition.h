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

#include <IPXACTmodels/common/Document.h>
#include "ApiFunction.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QStringList>
#include <QDomDocument>

class VLNV;

//-----------------------------------------------------------------------------
//! Communication definition class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiDefinition : public Document
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
     *  Destructor.
     */
    virtual ~ApiDefinition();

    /*!
     *  Makes a copy of the document.
     *
     *      @return The created copy of the API definition.
     */
    virtual QSharedPointer<Document> clone()  const;

    /*!
     *  Returns the dependent files (none).
     */
    virtual QStringList getDependentFiles() const;

    /*!
     *  Returns the dependent VLNVs (none).
     */
    virtual QList<VLNV> getDependentVLNVs() const;

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
    QSharedPointer<QStringList> getDataTypes() const;

    /*!
     *  Get the functions of the API definition.
     */
    QSharedPointer<QList<QSharedPointer<ApiFunction> > > getFunctions() const;

    /*!
     *  Returns the function at the given index.
     */
    QSharedPointer<ApiFunction> getFunction(int index);

    /*!
     *  Returns the function at the given index.
     */
    QSharedPointer<ApiFunction const> getFunction(int index) const;
    
private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the programming language for documentation purposes.
    QString language_;

    //! The COM definition reference.
    VLNV comDefRef_;

    //! The API data types.
    QSharedPointer< QStringList > dataTypes_;

    //! The API functions.
    QSharedPointer< QList< QSharedPointer<ApiFunction> > > functions_;
};

//-----------------------------------------------------------------------------

#endif // APIDEFINITION_H
