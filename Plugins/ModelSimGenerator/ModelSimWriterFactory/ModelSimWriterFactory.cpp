//-----------------------------------------------------------------------------
// File: ModelSimWriterFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Creates writers for generating do files.
//-----------------------------------------------------------------------------

#include "ModelSimWriterFactory.h"
#include "ModelSimWriter.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::ModelSimWriterFactory()
//-----------------------------------------------------------------------------
ModelSimWriterFactory::ModelSimWriterFactory(LibraryInterface* library,
    MessageMediator* messages, GenerationSettings* settings,
    QString const& kactusVersion, QString const& generatorVersion) :
    library_(library),
    messages_(messages),
    settings_(settings),    
    generatorVersion_(generatorVersion),
    kactusVersion_(kactusVersion)
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::prepareComponent()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationOutput> ModelSimWriterFactory::prepareComponent(QString const& /*outputPath*/,
    QSharedPointer<MetaComponent> /*component*/)
{
    return QSharedPointer<ModelSimDocument>();
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::prepareDesign()
//-----------------------------------------------------------------------------
QList<QSharedPointer<GenerationOutput> > ModelSimWriterFactory::prepareDesign(QList<QSharedPointer<MetaDesign> >& designs)
{
    QSharedPointer<ModelSimDocument> document = 
        QSharedPointer<ModelSimDocument>(new ModelSimDocument(QSharedPointer<ModelSimWriter>(new ModelSimWriter)));

    QList<QSharedPointer<MetaInstance> > components;

    foreach (QSharedPointer<MetaDesign> mDesign, designs)
    {
        components.append(mDesign->getTopInstance());
        components.append(mDesign->getInstances()->values());
    }

    QSharedPointer<ModelSimWriter> writer(new ModelSimWriter);

    document->writer_ = writer;
    document->fileName_ = designs.first()->getTopInstance()->getModuleName() + ".do";
    document->vlnv_ = designs.first()->getTopInstance()->getComponent()->getVlnv().toString();

    foreach (QSharedPointer<MetaComponent> mComponent, components)
    {
        QString basePath = library_->getPath(mComponent->getComponent()->getVlnv());

        foreach (QSharedPointer<FileSet> fileSet, *mComponent->getFileSets())
        {
            foreach (QSharedPointer<File> file, *fileSet->getFiles())
            {
                // Fetch the absolute path to the file
                QString absolutePath = General::getAbsolutePath(basePath, file->name());

                writer->addPath(absolutePath);
            }
        }
    }

    QList<QSharedPointer<GenerationOutput> > retval;
    retval.append(document);

    return retval;
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getLanguage()
//-----------------------------------------------------------------------------
QString ModelSimWriterFactory::getLanguage() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getSaveToFileset()
//-----------------------------------------------------------------------------
bool ModelSimWriterFactory::getSaveToFileset() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getGroupIdentifier()
//-----------------------------------------------------------------------------
QString ModelSimWriterFactory::getGroupIdentifier() const
{
    return QStringLiteral("simulation");
}
