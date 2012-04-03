//-----------------------------------------------------------------------------
// File: ApiFunction.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 29.3.2012
//
// Description:
// Class for managing one API function and its information.
//-----------------------------------------------------------------------------

#ifndef APIFUNCTION_H
#define APIFUNCTION_H

#include <QString>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class ApiFunctionParameter;

//-----------------------------------------------------------------------------
//! API function class.
//-----------------------------------------------------------------------------
class ApiFunction
{
public:
    /*!
     *  Default constructor.
     */
    ApiFunction();

    /*!
     *  Copy constructor.
     */
    ApiFunction(ApiFunction const& rhs);

    /*!
     *  Constructor which reads the function from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiFunction(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiFunction();

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer The XML stream writer.
     */
    void write(QXmlStreamWriter& writer);

    /*!
     *  Sets the name of the function.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the return value type.
     *
     *      @param [in] type The name of the return value type.
     */
    void setReturnValueType(QString const& type);

    /*!
     *  Sets the function description.
     *
     *      @param [in] desc The description.
     */
    void setDescription(QString const& desc);

    /*!
     *  Adds a new parameter to the function.
     *
     *      @param [in] param  The parameter object.
     *      @param [in] index  The index of the parameter. If -1, the parameter is added as the last one.
     */
    void addParam(QSharedPointer<ApiFunctionParameter> param, int index = -1);

    /*!
     *  Removes a parameter from the function.
     *
     *      @param [in] index The index of the parameter.
     */
    void removeParam(int index);

    /*!
     *  Returns the name of the function.
     */
    QString const& getName() const;

    /*!
     *  Returns the return value type.
     */
    QString const& getReturnValueType() const;

    /*!
     *  Returns the function description.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the parameter at the given index.
     *
     *      @param [in] index The parameter index.
     */
    QSharedPointer<ApiFunctionParameter> getParam(int index);

    /*!
     *  Returns the number of parameters in the function.
     */
    int getParamCount() const;

    /*!
     *  Generates a tooltip text for the function prototype.
     *
     *      @param [in]  paramIndex  The index of the parameter to highlight.
     *      @param [out] text        The tool tip text.
     */
    void generateToolTipText(unsigned int paramIndex, QString& text) const;

    /*!
     *  Assignment operator.
     */
    ApiFunction& operator=(ApiFunction const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the function.
    QString name_;

    //! The return value type.
    QString returnType_;

    //! The function description.
    QString desc_;

    //! The function parameters.
    QList< QSharedPointer<ApiFunctionParameter> > params_;
};

//-----------------------------------------------------------------------------

#endif // APIFUNCTION_H
