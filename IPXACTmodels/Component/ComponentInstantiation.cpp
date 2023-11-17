//-----------------------------------------------------------------------------
// File: ComponentInstantiation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Describes the ipxact:componentInstantiation element.
//-----------------------------------------------------------------------------

#include "ComponentInstantiation.h"

#include <IPXACTmodels/utilities/Copy.h>

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
configurationName_()
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
configurationName_(other.configurationName_)
{
    Copy::copyList(other.moduleParameters_, moduleParameters_);
    Copy::copyList(other.defaultFileBuilders_, defaultFileBuilders_);
    Copy::copyList(other.fileSetReferences_, fileSetReferences_);
    Copy::copyList(other.parameters_, parameters_);
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
        Copy::copyList(other.moduleParameters_, moduleParameters_);

        defaultFileBuilders_->clear();
        Copy::copyList(other.defaultFileBuilders_, defaultFileBuilders_);

        fileSetReferences_->clear();
        Copy::copyList(other.fileSetReferences_, fileSetReferences_);

        parameters_->clear();
        Copy::copyList(other.parameters_, parameters_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::~ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::~ComponentInstantiation()
{

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
QSharedPointer<QList<QSharedPointer<FileSetRef> > > ComponentInstantiation::getFileSetReferences() const
{
    return fileSetReferences_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getFileSetReferenceStrings()
//-----------------------------------------------------------------------------
QStringList ComponentInstantiation::getFileSetReferenceStrings() const
{
    QStringList referenceStrings;
    std::transform(fileSetReferences_->cbegin(), fileSetReferences_->cend(), std::back_inserter(referenceStrings),
        [](QSharedPointer<FileSetRef> fileSetRef)
        {
            return fileSetRef->getReference();
        });

    return referenceStrings;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setFileSetReferences()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setFileSetReferences(QSharedPointer<QList<QSharedPointer<FileSetRef> > > newFileSetReferences)
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
