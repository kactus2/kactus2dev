//-----------------------------------------------------------------------------
// File: InstantiationConfigurableElementEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2017
//
// Description:
// Editor for configurable elements of a single category.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONCONFIGURABLEELEMENTEDITOR_H
#define INSTANTIATIONCONFIGURABLEELEMENTEDITOR_H

#include <IPXACTmodels/common/Document.h>

#include <editors/common/ComponentInstanceEditor/configurableelementeditor.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>

#include <QAbstractItemModel>
#include <QGroupBox>
#include <QSharedPointer>
#include <QCheckBox>

class ConfigurableElementFinder;
class ExpressionFormatter;
class ExpressionParser;
class ParameterFinder;
class ConfigurableElementValue;
class ConfigurableElementsFilter;

//-----------------------------------------------------------------------------
//! Editor for configurable elements of a single category.
//-----------------------------------------------------------------------------
class InstantiationConfigurableElementEditor : public ConfigurableElementEditor
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] elementFinder       The finder for the configurable element values.
	 *      @param [in] parameterFinder     The finder for configurable elements and top parameters.
	 *      @param [in] elementFormatter    Formats referencing expressions in configurable elements.
	 *      @param [in] parameterFormatter  Formats referencing expressions in component instance.
	 *      @param [in] elementParser       Solves expressions in configurable elements.
	 *      @param [in] parameterParser     Solves expressions in default values (component instance).
	 *      @param [in] completionModel     The completion model for selecting parameter references.
	 *      @param [in] parent              The parent widget.
	 */
    InstantiationConfigurableElementEditor(
         QSharedPointer<ConfigurableElementFinder> elementFinder,
        ExpressionSet elementExpressions,
        ExpressionSet parameterExpressions,
//         QSharedPointer<ParameterFinder> parameterFinder,
//         QSharedPointer<ExpressionFormatter> elementFormatter,
//         QSharedPointer<ExpressionFormatter> parameterFormatter,
//         QSharedPointer<ExpressionParser> elementParser, QSharedPointer<ExpressionParser> parameterParser,
        QAbstractItemModel* completionModel, QWidget* parent);

	//! The destructor.
    virtual ~InstantiationConfigurableElementEditor() = default;

    /*!
     *  Setup the configurable element values for the model.
     *
     *      @param [in] containerName   Name of the item containing the parameters.
     *      @param [in] parameters      List of the selected parameters.
     *      @param [in] storedElements  List of the stored configurable element values.
     */
    void setParameters(QString const& containerName, QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        Document::Revision docRevision,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedElements);

private:

    //! No copying.	No assignment.
	InstantiationConfigurableElementEditor(const InstantiationConfigurableElementEditor& other);
	InstantiationConfigurableElementEditor& operator=(const InstantiationConfigurableElementEditor& other);


    //! The used configurable element finder.
    QSharedPointer<ConfigurableElementFinder> elementFinder_;

    Document::Revision docRevision_;

};

#endif // INSTANTIATIONCONFIGURABLEELEMENTEDITOR_H
