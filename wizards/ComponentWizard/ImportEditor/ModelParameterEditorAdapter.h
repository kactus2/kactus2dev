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

#include <Plugins/PluginSystem/ImportPlugin/ModelParameterVisualizer.h>

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
