//-----------------------------------------------------------------------------
// File: ModelSimDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Storage for ModelSim writers.
//-----------------------------------------------------------------------------

#ifndef MODELSIMDOCUMENT_H
#define MODELSIMDOCUMENT_H

#include "../modelsimgenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/OutputControl.h>

#include "ModelSimWriter.h"

//-----------------------------------------------------------------------------
// ModelSim document.
//-----------------------------------------------------------------------------
struct ModelSimDocument : public GenerationOutput
{
    QSharedPointer<ModelSimWriter> writer_;

    //! The constructor.
    explicit ModelSimDocument(QSharedPointer<ModelSimWriter> writer);

    //! The destructor.
    virtual ~ModelSimDocument() = default;

    /*!
     *  Writes the content. 
     *
     *    @param [in] outputDirectory         The possible output directory.
     */
	void write(QString const& outputDirectory);
    
    /*!
     *  Finds position for body text highlight in document, if any exists.
     *
	 *    @param [out] begin                  The position where the highlight begins, if successful.
     *    @param [out] end                    The position where the highlight ends, if successful.
     */
    void getBodyHighlight(int& begin, int& end) const;
    
private:
};

#endif // MODELSIMDOCUMENT_H
