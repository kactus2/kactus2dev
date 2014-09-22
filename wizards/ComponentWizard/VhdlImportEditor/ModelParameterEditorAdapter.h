//-----------------------------------------------------------------------------
// File: ModelParameterEditorAdapter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.09.2014
//
// Description:
// Adapter for ModelParameterEditor to implement ModelParameterVisualizer. 
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETEREDITORADAPTER_H
#define MODELPARAMETEREDITORADAPTER_H

#include <Plugins/PluginSystem/ModelParameterVisualizer.h>

class ModelParameter;
class ModelParameterEditor;

//-----------------------------------------------------------------------------
//! Adapter for ModelParameterEditor to implement ModelParameterVisualizer. 
//-----------------------------------------------------------------------------
class ModelParameterEditorAdapter : public ModelParameterVisualizer
{

public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] editor   The editor used for visualization.
     */
    ModelParameterEditorAdapter(ModelParameterEditor* editor);

    //! The destructor.
    ~ModelParameterEditorAdapter();

    /*!
     *  Called when a model parameter should be added to the editor.
     *
     *      @param [in] modelParameterToAdd   The  model parameter to add.
     */
    virtual void addModelParameter(QSharedPointer<ModelParameter> modelParameterToAdd);

    /*!
     *  Called when a  model parameter should be removed from the editor.
     *
     *      @param [in] modelParameterToRemove   The  model parameter to remove.
     */

    virtual void removeModelParameter(QSharedPointer<ModelParameter> modelParameterToRemove);

    /*!
     *  Called to receive future notifications of model parameter changes in the editor.
     *
     *      @param [in] listener   The the listener to receive notifications in.
     *
     *      @remark listener must have a slot with the following signature:
     *              onModelParameterChanged(QSharedPointer<ModelParameter> changedParameter);
     */
    virtual void registerChangeListener(QObject const* listener);

private:
    
    //! The editor used for visualization.
    ModelParameterEditor* editor_;
};

#endif // MODELPARAMETEREDITORADAPTER_H
