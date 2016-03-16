//-----------------------------------------------------------------------------
// File: ComponentInstantiation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Describes the ipxact:componentInstantiation element.
//-----------------------------------------------------------------------------

#include "ComponentInstantiation.h"

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::ComponentInstantiation(QString const& name /* = QString() */,
    QString const& displayName /* = QString() */, QString const& description /* = QString() */) :
NameGroup(name, displayName, description),
Extendable(),
virtuality_(false),
language_(),
languageStrictness_(false),
libraryName_(),
packageName_(),
moduleName_(),
architectureName_(),
configurationName_(),
moduleParameters_(new QList<QSharedPointer<ModuleParameter> > ()),
defaultFileBuilders_(new QList<QSharedPointer<FileBuilder> > ()),
fileSetReferences_(new QStringList()),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::ComponentInstantiation(const ComponentInstantiation& other) :
NameGroup(other),
Extendable(other),
virtuality_(other.virtuality_),
language_(other.language_),
languageStrictness_(other.languageStrictness_),
libraryName_(other.libraryName_),
packageName_(other.packageName_),
moduleName_(other.moduleName_),
architectureName_(other.architectureName_),
configurationName_(other.configurationName_),
moduleParameters_(new QList<QSharedPointer<ModuleParameter> > ()),
defaultFileBuilders_(new QList<QSharedPointer<FileBuilder> > ()),
fileSetReferences_(new QStringList),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyModuleParameters(other);
    copyDefaultFileBuilders(other);
    copyFileSetReferences(other);
    copyParameters(other);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::operator=()
//-----------------------------------------------------------------------------
ComponentInstantiation& ComponentInstantiation::operator=(const ComponentInstantiation& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        
        virtuality_ = other.virtuality_;
        language_ = other.language_;
        languageStrictness_ = other.languageStrictness_;
        libraryName_ = other.libraryName_;
        packageName_ = other.packageName_;
        moduleName_ = other.moduleName_;
        architectureName_ = other.architectureName_;
        configurationName_ = other.configurationName_;

        moduleParameters_->clear();
        copyModuleParameters(other);
        defaultFileBuilders_->clear();
        copyDefaultFileBuilders(other);
        fileSetReferences_->clear();
        copyFileSetReferences(other);
        parameters_->clear();
        copyParameters(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::~ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::~ComponentInstantiation()
{
    moduleParameters_.clear();
    defaultFileBuilders_.clear();
    fileSetReferences_.clear();
    parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::isVirtual()
//-----------------------------------------------------------------------------
bool ComponentInstantiation::isVirtual() const
{
    return virtuality_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setVirtual()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setVirtual(bool virtualStatus)
{
    virtuality_ = virtualStatus;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getLanguage()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getLanguage() const
{
    return language_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setLanguage()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setLanguage(QString const& newLanguage)
{
    language_ = newLanguage;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::isLanguageStrict()
//-----------------------------------------------------------------------------
bool ComponentInstantiation::isLanguageStrict() const
{
    return languageStrictness_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setLanguageStrictness()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setLanguageStrictness(bool strictness)
{
    languageStrictness_ = strictness;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getLibraryName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getLibraryName() const
{
    return libraryName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setLibraryName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setLibraryName(QString const& newLibraryName)
{
    libraryName_ = newLibraryName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getPackageName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getPackageName() const
{
    return packageName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setPackageName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setPackageName(QString const& newPackageName)
{
    packageName_ = newPackageName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getModuleName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getModuleName() const
{
    return moduleName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setModuleName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setModuleName(QString const& newModuleName)
{
    moduleName_ = newModuleName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getArchitectureName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getArchitectureName() const
{
    return architectureName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setArchitectureName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setArchitectureName(QString const& newArchitectureName)
{
    architectureName_ = newArchitectureName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getConfigurationName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getConfigurationName() const
{
    return configurationName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setConfigurationName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setConfigurationName(QString const& newConfigurationName)
{
    configurationName_ = newConfigurationName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getModuleParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModuleParameter> > > ComponentInstantiation::getModuleParameters() const
{
    return moduleParameters_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setModuleParameters()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setModuleParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > newModuleParameters)
{
    moduleParameters_->clear();
    moduleParameters_ = newModuleParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getDefaultFileBuilders()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FileBuilder> > > ComponentInstantiation::getDefaultFileBuilders() const
{
    return defaultFileBuilders_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setDefaultFileBuilders()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setDefaultFileBuilders(
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > newDefaultFileBuilders)
{
    defaultFileBuilders_->clear();
    defaultFileBuilders_ = newDefaultFileBuilders;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getFileSetReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> ComponentInstantiation::getFileSetReferences() const
{
    return fileSetReferences_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setFileSetReferences()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setFileSetReferences(QSharedPointer<QStringList> newFileSetReferences)
{
    fileSetReferences_ = newFileSetReferences;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ComponentInstantiation::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setParameters()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_->clear();
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::copyModuleParameters()
//-----------------------------------------------------------------------------
void ComponentInstantiation::copyModuleParameters(const ComponentInstantiation& other) const
{
    foreach (QSharedPointer<ModuleParameter> moduleParameter, *other.moduleParameters_)
    {
        if (moduleParameter)
        {
            QSharedPointer<ModuleParameter> copy =
                QSharedPointer<ModuleParameter>(new ModuleParameter(*moduleParameter.data()));
            moduleParameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::copyDefaultFileBuilders()
//-----------------------------------------------------------------------------
void ComponentInstantiation::copyDefaultFileBuilders(const ComponentInstantiation& other) const
{
    foreach (QSharedPointer<FileBuilder> fileBuilder, *other.defaultFileBuilders_)
    {
        if (fileBuilder)
        {
            QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(new FileBuilder(*fileBuilder.data()));
            defaultFileBuilders_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::copyFileSetReferences()
//-----------------------------------------------------------------------------
void ComponentInstantiation::copyFileSetReferences(const ComponentInstantiation& other) const
{
    foreach (QString reference, *other.fileSetReferences_)
    {
        fileSetReferences_->append(reference);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::copyParameters()
//-----------------------------------------------------------------------------
void ComponentInstantiation::copyParameters(const ComponentInstantiation& other) const
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}