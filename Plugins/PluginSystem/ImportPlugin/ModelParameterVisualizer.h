//-----------------------------------------------------------------------------
// File: ModelParameterVisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Interface for model parameter visualizers in component import.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMTERVISUALIZER_H
#define MODELPARAMTERVISUALIZER_H

#include <QSharedPointer>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Interface for model parameter visualizers in component import.
//-----------------------------------------------------------------------------
class ModelParameterVisualizer
{
public:

	//! The destructor.
    virtual ~ModelParameterVisualizer() {};

    /*!
     *  Called when a model parameter should be added to the visualization.
     *
     *      @param [in] modelParameterToAdd   The  model parameter to add.
     */
    virtual void addModelParameter(QSharedPointer<ModelParameter> modelParameterToAdd) = 0;

    /*!
     *  Called when a  model parameter should be removed from the visualization.
     *
     *      @param [in] modelParameterToRemove   The  model parameter to remove.
     */
    virtual void removeModelParameter(QSharedPointer<ModelParameter> modelParameterToRemove) = 0;

    /*!
     *  Called to receive future notifications of model parameter changes in the visualization.
     *
     *      @param [in] listener   The the listener to receive notifications in.
     */
    virtual void registerChangeListener(QObject const* listener) = 0;
};

#endif // MODELPARAMTERVISUALIZER_H
