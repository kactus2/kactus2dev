//-----------------------------------------------------------------------------
// File: ParameterItemEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// ItemEditor for component items containing parameters, containing signals for reference trees.
//-----------------------------------------------------------------------------

#ifndef PARAMETERITEMEDITOR_H
#define PARAMETERITEMEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

class Parameter;
class AbstractParameterInterface;

//-----------------------------------------------------------------------------
//! ItemEditor for component items containing parameters, containing signals for reference trees.
//-----------------------------------------------------------------------------
class ParameterItemEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component   Pointer to the component instance that is being edited.
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the owner of this widget.
	 */
	ParameterItemEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~ParameterItemEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh() = 0;

signals:

    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] id              Id of the selected parameter.
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

	//! No copying. No Assignment.
	ParameterItemEditor(const ParameterItemEditor& other);
	ParameterItemEditor& operator=(const ParameterItemEditor& other);
};

#endif // PARAMETERITEMEDITOR_H
