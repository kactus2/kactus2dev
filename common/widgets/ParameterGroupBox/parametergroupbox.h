//-----------------------------------------------------------------------------
// File: parametergroupbox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.2.2012
//
// Description:
// Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERGROUPBOX_H
#define PARAMETERGROUPBOX_H

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Document.h>

#include <editors/ComponentEditor/parameters/ParameterColumns.h>

#include <QGroupBox>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;
class Choice;
class ParameterFinder;
class ParametersModel;
class ParameterDelegate;
class ParameterValidator;
class ExpressionFormatter;
class ParametersView;
class ParametersInterface;
class AbstractParameterInterface;

//-----------------------------------------------------------------------------
//! Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------
class ParameterGroupBox : public QGroupBox 
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in]  parameters             The list containing the parameters.
	 *    @param [in]  choices                The available choices for the parameters.
	 *    @param [in]  parameterFinder        The parameter finder used to search the components parameters.
	 *    @param [in]  expressionFormatter    The expression formatter.
	 *    @param [in]  docRevision            The IP-XACT standard revision to use.
	 *    @param [in]  lockableParameters     Flag indicating if parameters should be lockable in the editor or not.
	 *    @param [in]  parent                 The owner of this editor.
	 */
	ParameterGroupBox(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QSharedPointer<QList<QSharedPointer<Choice> > > choices, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter, Document::Revision docRevision, bool lockableParameters = false, QWidget *parent = nullptr);

    //! No copying. No assignment.
    ParameterGroupBox(const ParameterGroupBox& other) = delete;
    ParameterGroupBox& operator=(const ParameterGroupBox& other) = delete;

	/*!
     *  The destructor.
     */
	virtual ~ParameterGroupBox();

	/*!
     *  Restore the changes made in the editor back to ones in parameter models.
     */
	virtual void refresh();

    /*!
     *  Setup a list of new parameters.
     *
     *    @param [in] newParameters   New parameter list.
     */
    void setNewParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters, 
        QSharedPointer<QList<QSharedPointer<Choice> > > newChoices, Document::Revision docRevision);

    /*!
     *	Lock editing of a given column of a given parameter. Should be called only when lockable model is used.
     *  
     *    @param [in] parameterName     Parameter whose cell to lock.
     *    @param [in] column            The column of the cell to lock.
     */
    void lockParameterColumn(QString const& parameterName, ParameterColumns::columns column);

    /*!
     *	Get the parameters interface being used.
     *  
     *    @return The parameters interface.
     */
    ParametersInterface* getInterface() const;

    /*!
     *	Add a parameter to the editor and its model.
     *  
     *    @param [in] parameterName     The name of the added parameter.
     */
    void addParameter(QString const& parameterName);

signals:

	//! Emitted when contents of the editor change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the parameter with the id.
     *
     *    @param [in] id              The id of the parameter.
     *    @param [in] parameterName   Name of the selected parameter.
     */
    void openReferenceTree(QString const& id, QString const& parameterName) const;

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *    @param [in] parameterList       The selected parameters.
     *    @param [in] parameterInterface  Interface for accessing parameters.
     */
    void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        AbstractParameterInterface* parameterInterface);

protected:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The view that displays the parameters.
    ParametersView* view_;

    //! The proxy that is used to do the sorting of parameters.
    QSortFilterProxyModel* proxy_;

	//! The model that holds the data to be displayed to the user
	ParametersModel* model_;

    //! The parameter delegate.
    ParameterDelegate* delegate_;

    //! The parameter finder for locating the contained parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Interface for accessing parameters.
    ParametersInterface* parameterInterface_;

    //! The parameter validator.
    QSharedPointer<ParameterValidator> parameterValidator_;
};

#endif // PARAMETERGROUPBOX_H
