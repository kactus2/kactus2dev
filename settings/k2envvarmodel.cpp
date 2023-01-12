//-----------------------------------------------------------------------------
// File: k2envvarmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.06.2013
//
// Description:
// Manages the K2 environment variables displayed in the settings dialog.
//-----------------------------------------------------------------------------

#include "k2envvarmodel.h"
#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/PluginManager.h>

#include <common/KactusColors.h>

#include <QColor>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::K2EnvVarModel()
//-----------------------------------------------------------------------------
K2EnvVarModel::K2EnvVarModel(QSettings& settings, QObject *parent): QAbstractTableModel(parent), table_()
{
    loadData(settings);
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::apply()
//-----------------------------------------------------------------------------
void K2EnvVarModel::apply(QSettings& settings)
{
    // first clear all previously defined variables to remove duplicates
    settings.remove(QStringLiteral("K2Variables"));

    QStringList variableNames;

    foreach (K2EnvVarModel::EnvSetting const& envVar, table_)
    {
        // unnamed variables are not stored
        if (envVar.name_.isEmpty() == false)
        {
            variableNames.append(envVar.name_);

            // save the settings for the variable name
            settings.setValue(QStringLiteral("K2Variables/") + envVar.name_ + QStringLiteral("/value"),
                envVar.value_);
            settings.setValue(QStringLiteral("K2Variables/") + envVar.name_ + QStringLiteral("/suffix"),
                envVar.fileSuffix_);
            settings.setValue(QStringLiteral("K2Variables/") + envVar.name_ + QStringLiteral("/description"),
                envVar.description_);
        }
    }

    settings.setValue(QStringLiteral("K2Variables/variableNames"), variableNames);
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::rowCount()
//-----------------------------------------------------------------------------
int K2EnvVarModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::columnCount()
//-----------------------------------------------------------------------------
int K2EnvVarModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return K2EnvVarModel::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags K2EnvVarModel::flags( const QModelIndex& index ) const
{
    if (index.isValid() == false || index.row() < 0 || index.row() >= table_.size())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if ((index.column() == K2EnvVarModel::NAME_COLUMN || index.column() == K2EnvVarModel::DESCRIPTION_COLUMN) &&
        table_.at(index.row()).fromPlugin_ == false)
    {
        flags |= Qt::ItemIsEditable;
    }
    else if (index.column() ==K2EnvVarModel::VALUE_COLUMN)
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::headerData()
//-----------------------------------------------------------------------------
QVariant K2EnvVarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (section == K2EnvVarModel::NAME_COLUMN)
        {
            return tr("Name");
        }
        else if (section == K2EnvVarModel::VALUE_COLUMN)
        {
            return tr("Value");
        }
        else if (section == K2EnvVarModel::FILE_SUFFIX_COLUMN)
        {
            return tr("File Suffix");
        }
        else if (section == K2EnvVarModel::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::data()
//-----------------------------------------------------------------------------
QVariant K2EnvVarModel::data(QModelIndex const& index, int role) const
{
    if (index.isValid() == false || index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (index.column() == K2EnvVarModel::NAME_COLUMN)
        {
            return table_.at(index.row()).name_;
        }
        else if (index.column() == K2EnvVarModel::VALUE_COLUMN)
        {
            return table_.at(index.row()).value_;
        }
        else if (index.column() == K2EnvVarModel::FILE_SUFFIX_COLUMN)
        {
            return table_.at(index.row()).fileSuffix_;
        }
        else if (index.column() == K2EnvVarModel::DESCRIPTION_COLUMN)
        {
            return table_.at(index.row()).description_;
        }
        else
        {
            return QVariant();
        }
    }
    else if (Qt::BackgroundRole == role)
    {
        if (index.column() == K2EnvVarModel::NAME_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::setData()
//-----------------------------------------------------------------------------
bool K2EnvVarModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
    if (index.isValid() == false || index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }

    if (Qt::EditRole == role)
    {
        if (index.column() == K2EnvVarModel::NAME_COLUMN)
        {
            // plugin variable's name can't be modified
            if (table_[index.row()].fromPlugin_)
            {
                return false;
            }

            QString newName = value.toString();

            // if a variable with the name already exists
            K2EnvVarModel::EnvSetting testVar(newName, QString(), QString(), QString(), false);
            if (table_.contains(testVar))
            {
                return false;
            }

            table_[index.row()].name_ = newName;
        }

        else if (index.column() == K2EnvVarModel::VALUE_COLUMN)
        {
            table_[index.row()].value_ = value.toString();

        }

        else if (index.column() == K2EnvVarModel::FILE_SUFFIX_COLUMN)
        {
            // plugin variable's file suffix can't be modified
            if (table_[index.row()].fromPlugin_)
            {
                return false;
            }

            table_[index.row()].fileSuffix_ = value.toString();
        }

        else if (index.column() == K2EnvVarModel::DESCRIPTION_COLUMN)
        {
            // plugin variable's description can't be modified
            if (table_[index.row()].fromPlugin_) {
                return false;
            }

            table_[index.row()].description_ = value.toString();

        }

        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::onAddItem()
//-----------------------------------------------------------------------------
void K2EnvVarModel::onAddItem(QModelIndex const& index)
{
    int row = table_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    table_.insert(row, K2EnvVarModel::EnvSetting());
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::onRemoveItem()
//-----------------------------------------------------------------------------
void K2EnvVarModel::onRemoveItem(QModelIndex const& index)
{
    // don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return;
    }

    // remove the specified item
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::containsEmptyVariables()
//-----------------------------------------------------------------------------
bool K2EnvVarModel::containsEmptyVariables() const
{
    foreach (K2EnvVarModel::EnvSetting const& envVar, table_)
    {
        if (envVar.name_.isEmpty())
        {
            return true;
        }
    }

    // no empty variables defined
    return false;
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::loadData()
//-----------------------------------------------------------------------------
void K2EnvVarModel::loadData(QSettings& settings)
{
    beginResetModel();

    // find the variables required by the active plugins
    QList<IPlugin::ExternalProgramRequirement> pluginRequirements;
    foreach (IPlugin* plugin, PluginManager::getInstance().getActivePlugins())
    {
        pluginRequirements.append(plugin->getProgramRequirements());
    }

    // add the plugin variables to the list
    QStringList usedNames;
    foreach (IPlugin::ExternalProgramRequirement const& req, pluginRequirements)
    {
        // do not add variables with no name
        // if same variable is used by several plugins then add it only once
        if (req.name_.isEmpty() == false && !usedNames.contains(req.name_))
        {
            usedNames.append(req.name_);

            // add the variable
            K2EnvVarModel::EnvSetting envVar(req.name_, QString(), req.filters_, req.description_, true);
            table_.append(envVar);
        }
    }

    // list of the variable names
    QStringList variableNames = settings.value(QStringLiteral("K2Variables/variableNames")).toStringList();

    // find the settings for each variable
    foreach(QString const& name, variableNames)
    {
        // create the variable from the settings
        QString value = settings.value(
            QStringLiteral("K2Variables/") + name + QStringLiteral("/value")).toString();
        QString suffix = settings.value(
            QStringLiteral("K2Variables/") + name + QStringLiteral("/suffix")).toString();
        QString desc = settings.value(
            QStringLiteral("K2Variables/") + name + QStringLiteral("/description")).toString();

        K2EnvVarModel::EnvSetting envVar(name, value, suffix, desc, false);

        // if the variable was defined by the plugins
        if (table_.contains(envVar))
        {
            // other settings are defined by the plugins but value is set by settings
            int index = table_.indexOf(envVar);
            table_[index].value_ = envVar.value_;
        }
        // if the variable is new and not defined by the plugins
        else
        {
            table_.append(envVar);
        }
    }

    // Sort the variables before showing them
    std::sort(table_.begin(), table_.end());

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::EnvSetting::EnvSetting()
//-----------------------------------------------------------------------------
K2EnvVarModel::EnvSetting::EnvSetting(QString const& name,
    QString const& value,
    QString const& fileSuffix, 
    QString const& description,
    bool fromPlugin ):
name_(name),
    value_(value),
    fileSuffix_(fileSuffix),
    description_(description),
    fromPlugin_(fromPlugin)
{

}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::EnvSetting::EnvSetting()
//-----------------------------------------------------------------------------
K2EnvVarModel::EnvSetting::EnvSetting():
name_(),
value_(),
fileSuffix_(),
description_(),
fromPlugin_(false)
{
}


//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::EnvSetting::operator<()
//-----------------------------------------------------------------------------
bool K2EnvVarModel::EnvSetting::operator<( const EnvSetting& other ) const
{
    return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::EnvSetting::operator==()
//-----------------------------------------------------------------------------
bool K2EnvVarModel::EnvSetting::operator==( const EnvSetting& other ) const
{
    return name_.compare(other.name_, Qt::CaseInsensitive) == 0;
}

//-----------------------------------------------------------------------------
// Function: K2EnvVarModel::EnvSetting::operator!=()
//-----------------------------------------------------------------------------
bool K2EnvVarModel::EnvSetting::operator!=( const EnvSetting& other ) const
{
    return name_.compare(other.name_, Qt::CaseInsensitive) != 0;
}
