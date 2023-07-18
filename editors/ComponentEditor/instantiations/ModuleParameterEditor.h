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

#include <IPXACTmodels/common/Document.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>

class Choice;
class ModuleParameterModel;
class ParameterFinder;
class ExpressionFormatter;
class Parameter;
class AbstractParameterInterface;
class ModuleParameterInterface;
class ParametersView;
class ComponentInstantiation;

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
     *      @param [in] instantiation           Component instantiation containing the selected module parameters.
     *      @param [in] componentChoices        The component choices.
     *      @param [in] parameterFinder         The selected parameter finder.
     *      @param [in] expressionFormatter     The selected expression formatter.
     *      @param [in] parameterInterface      Interface for accessing module parameters.
     *      @param [in] docRevision             The IP-XACT standard revision in use.
     *      @param [in] parent                  The owner of this editor.
     */
    ModuleParameterEditor(
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ModuleParameterInterface* parameterInterface,
        Document::Revision docRevision,
        QWidget* parent);

	/*!
     *  The destructor.
     */
	~ModuleParameterEditor();

    /*!
     *  Refresh the contents of the editor.
     */
    void refresh();

    /*!
     *  Enables the editing of module parameters.
     */
    void enableEditing();

    /*!
     *  Disables the editing of module parameters.
     */
    void disableEditing();

    /*!
     *  Sets the module parameters to edit.
     *
     *      @param [in] containingInstantiation     Component instantiation containing the module parameters.
     */
    void setModuleParameters(QSharedPointer<ComponentInstantiation> containingInstantiation);

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
        AbstractParameterInterface* parameterInterface);

private:

	// Disable copying.
	ModuleParameterEditor(ModuleParameterEditor const& rhs);
	ModuleParameterEditor& operator=(ModuleParameterEditor const& rhs);

    //! The used sort filter proxy.
    QSortFilterProxyModel* proxy_;

    //! Used for editing model parameters.
    ModuleParameterModel* model_;

    //! Interface for accessing module parameters.
    ModuleParameterInterface* moduleParameterInterface_;

    //! The parameters view.
    ParametersView* view_;

    //! Component instantiation containing the module parameters.
    QSharedPointer<ComponentInstantiation> instantiation_;
};

#endif // MODULEPARAMETEREDITOR_H