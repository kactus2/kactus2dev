//-----------------------------------------------------------------------------
// File: ApiFunctionParameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// API function parameter class for API definitions.
//-----------------------------------------------------------------------------

#ifndef APIFUNCTIONPARAMETER_H
#define APIFUNCTIONPARAMETER_H

#include "generaldeclarations.h"

#include "ipxactmodels_global.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! API function parameter class for API definitions.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiFunctionParameter
{
public:
    /*!
     *  Default constructor.
     */
    ApiFunctionParameter();

    /*!
     *  Copy constructor.
     */
    ApiFunctionParameter(ApiFunctionParameter const& rhs);

    /*!
     *  Constructor which read the function parameter from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiFunctionParameter(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiFunctionParameter();

    /*!
     *  Writes the function parameter to an XML stream.
     *
     *      @param [in] writer The XML stream writer.
     */
    void write(QXmlStreamWriter& writer);

    /*!
     *  Returns true if the function parameter is valid.
     *
     *      @param [out] errorList  The list of errors found in the contents.
     *      @param [in]  parentId   The identifier of the containing API function.
     */
    bool isValid(QStringList& errorList, QString const& parentId) const;

    /*!
     *  Returns true if the function parameter is valid.
     */
    bool isValid() const;

    /*!
     *  Sets the name of the parameter.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the type of the parameter.
     *
     *      @param [in] type The type name to set.
     */
    void setType(QString const& type);

    /*!
     *  Sets the allowed COM transfer type.
     *
     *      @param [in] comTransferType The COM transfer type to set.
     */
    void setComTransferType(QString const& comTransferType);

    /*!
     *  Sets the allowed COM direction.
     *
     *      @param [in] comDirection The COM direction to set.
     */
    void setComDirection(General::Direction comDirection);

    /*!
     *  Sets the COM content source.
     *
     *      @param [in] contentSource The name of the content source.
     *                                "name" = name of a COM interface, otherwise any name of a COM property
     */
    void setContentSource(QString const& contentSource);

    /*!
     *  Sets the dependent parameter index.
     *
     *      @param [in] index The dependent parameter index. -1 if not set.
     */
    void setDependentParameterIndex(int index);

    /*!
     *  Sets the parameter description.
     *
     *      @param [in] desc The description.
     */
    void setDescription(QString const& desc);

    /*!
     *  Returns the name of the parameter.
     */
    QString const& getName() const;

    /*!
     *  Returns the type of the parameter.
     */
    QString const& getType() const;

    /*!
     *  Returns the allowed COM transfer type.
     */
    QString const& getComTransferType() const;

    /*!
     *  Returns the allowed COM direction.
     */
    General::Direction getComDirection() const;

    /*!
     *  Returns the name of the COM content source.
     */
    QString const& getContentSource() const;

    /*!
     *  Returns the index of the dependent parameter, or -1 if not used.
     */
    int getDependentParameterIndex() const;

    /*!
     *  Returns the parameter description.
     */
    QString const& getDescription() const;

    /*!
     *  Assignment operator.
     */
    ApiFunctionParameter& operator=(ApiFunctionParameter const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the parameter.
    QString name_;

    //! The type of the parameter.
    QString type_;

    //! COM transfer type.
    QString comTransferType_;

    //! COM direction.
    General::Direction comDirection_;

    //! The name of the source for content assist.
    QString contentSource_;

    //! Dependent parameter index.
    int dependentParamIndex_;

    //! The parameter description.
    QString desc_;
};

//-----------------------------------------------------------------------------

#endif // APIFUNCTIONPARAMETER_H
