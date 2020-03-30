//-----------------------------------------------------------------------------
// File: ModuleParameterEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Editor for module parameters.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETEREDITOR_H
#define MODULEPARAMETEREDITOR_H

#include <QGroupBox>
#include <QSortFilterProxyModel>

class Choice;
class ModuleParameter;
class ModuleParameterModel;
class ParameterFinder;
class ExpressionFormatter;
class Parameter;
class ParametersInterface;
class ParametersView;

//-----------------------------------------------------------------------------
//! Editor for module parameters.
//-----------------------------------------------------------------------------
class ModuleParameterEditor : public QGroupBox
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameters              The selected module parameters.
     *      @param [in] componentChoices        The component choices.
     *      @param [in] parameterFinder         The selected parameter finder.
     *      @param [in] expressionFormatter     The selected expression formatter.
     *      @param [in] parent                  The owner of this editor.
     */
    ModuleParameterEditor(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > parameters,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent);

	//! The destructor.
	~ModuleParameterEditor();

    //! Refresh the contents of the editor.
    void refresh();

    //! Enables the editing of module parameters.
    void enableEditing();

    //! Disables the editing of module parameters.
    void disableEditing();

    //! Sets the module parameters to edit.
    void setModuleParameters(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters);

signals:
    //! Emitted when contents of the editor change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(QString const& msg) const;

	//! Prints a notification to user.
	void noticeMessage(QString const& msg) const;

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the parameter with the ID.
     *
     *      @param [in] id              The ID of the parameter.
     *      @param [in] parameterName   Name of the selected parameter.
     */
    void openReferenceTree(QString const& id, QString const& parameterName) const;

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *      @param [in] parameterList       The selected parameters.
     *      @param [in] parameterInterface  Interface for accessing parameters.
     */
    void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        QSharedPointer<ParametersInterface> parameterInterface);

private:

	// Disable copying.
	ModuleParameterEditor(ModuleParameterEditor const& rhs);
	ModuleParameterEditor& operator=(ModuleParameterEditor const& rhs);

    //! The used sort filter proxy.
    QSortFilterProxyModel* proxy_;

    //! Used for editing model parameters.
    ModuleParameterModel* model_;

    //! Interface for accessing parameters.
    QSharedPointer<ParametersInterface> parameterInterface_;

    //! The parameters view.
    ParametersView* view_;
};

#endif // MODULEPARAMETEREDITOR_H