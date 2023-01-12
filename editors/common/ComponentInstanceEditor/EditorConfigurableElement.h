//-----------------------------------------------------------------------------
// File: EditorConfigurableElement.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.08.2017
//
// Description:
// Container class for configurable elements in the configurable element value tables.
//-----------------------------------------------------------------------------

#ifndef EDITORCONFIGURABLEELEMENT_H
#define EDITORCONFIGURABLEELEMENT_H

#include <QSharedPointer>

class Parameter;

//-----------------------------------------------------------------------------
//! Container class for configurable elements in the configurable element value tables.
//-----------------------------------------------------------------------------
class EditorConfigurableElement
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] referenceParameter      Parameter referenced by the configurable element.
	 *      @param [in] evaluatedDefaultValue   Evaluated default value of the referenced parameter.
	 *      @param [in] configuratedValue       The configurable value.
	 */
    EditorConfigurableElement(QSharedPointer<Parameter> referencedParameter, QString const& evaluatedDefaultValue,
        QString const& configuratedValue);
	
	/*!
	 *  The destructor.
	 */
    ~EditorConfigurableElement() = default;

    //! No copying. No assignment.
    EditorConfigurableElement(const EditorConfigurableElement& other) = delete;
    EditorConfigurableElement& operator=(const EditorConfigurableElement& other) = delete;

    /*!
     *  Get the referenced parameter.
     *
     *      @return The referenced parameter.
     */
    QSharedPointer<Parameter> getReferencedParameter() const;

    /*!
     *  Set a new parameter reference.
     *
     *      @param [in] parameter   The new parameter.
     */
    void setParameterReference(QSharedPointer<Parameter> parameter);

    /*!
     *  Get the evaluated parameter default value.
     *
     *      @return The evaluated parameter default value.
     */
    QString const& getEvaluatedDefaultValue() const;

    /*!
     *  Set a new evaluated default value.
     *
     *      @param [in] newEvaluatedDefaultValue    The new evaluated default value.
     */
    void setEvaluatedDefaultValue(QString const& newEvaluatedDefaultValue);

    /*!
     *  Get the configurable element value.
     *
     *      @return The configurable element value.
     */
    QString const& getConfiguratedValue() const;

    /*!
     *  Set a new value for configurable value.
     *
     *      @param [in] newConfiguratedValue    The new configurable element value.
     */
    void setConfiguratedValue(QString const& newConfiguratedValue);

    /*!
     *  Get the configurable element type.
     *
     *      @return The configurable element type.
     */
    QString const& getType() const;

    /*!
     *  Set a new type for configurable value.
     *
     *      @param [in] newType    The new configurable element type.
     */
    void setType(QString const& newType);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The referenced parameter.
    QSharedPointer<Parameter> referencedParameter_;

    //! Evaluated default value.
    QString evaluatedDefaultValue_;

    //! The configurable value.
    QString configuratedValue_;

    //! The type of the configurable value.
    QString type_;
};

#endif // EDITORCONFIGURABLEELEMENT_H
