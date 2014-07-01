//-----------------------------------------------------------------------------
// File: NamingPolicySettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// settings property page for specifying naming policies.
//-----------------------------------------------------------------------------

#ifndef NAMINGPOLICYSETTINGSPAGE_H
#define NAMINGPOLICYSETTINGSPAGE_H

#include "SettingsPage.h"

#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include <QPlainTextEdit>
#include <QGroupBox>

class ListManager;
class SnippetTextEdit;

//-----------------------------------------------------------------------------
//! NamingPolicySettingsPage class.
//-----------------------------------------------------------------------------
class NamingPolicySettingsPage : public SettingsPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    NamingPolicySettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    virtual ~NamingPolicySettingsPage();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

private slots:
    /*!
     *  Updates the widget contents according to the new category selection.
     *
     *      @param [in] index The category index.
     */
    void onCategoryChanged(int index);

    /*!
     *  Saves the values of the currently selected category.
     */
    void saveCurrentValues();

private:
    // Disable copying.
    NamingPolicySettingsPage(NamingPolicySettingsPage const& rhs);
    NamingPolicySettingsPage& operator=(NamingPolicySettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    //! Policy type enumeration.
    //-----------------------------------------------------------------------------
    enum PolicyType
    {
        POLICY_ENUMERATION = 0,
        POLICY_FORMAT
    };

    //-----------------------------------------------------------------------------
    //! Policy category structure.
    //-----------------------------------------------------------------------------
    struct PolicyCategory
    {
        QString key;            //!< The name of the settings key.
        QString name;           //!< The name of the policy category.
        PolicyType type;        //!< The policy type for the category.
        QStringList values;     //!< The allowed value(s) for the policy.
        QStringList magicWords; //!< Available magic words. Only relevant if policy type is POLICY_FORMAT.

        /*!
         *  Constructor.
         */
        PolicyCategory(QString const& key, QString const& name, PolicyType type)
            : key(key),
              name(name),
              type(type),
              values()
        {
        }
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Policy categories data.
    QList<PolicyCategory> categories_;

    //! The index of the currently selected category.
    int curCategoryIndex_;

    //! Label for element selection.
    QLabel* categoryLabel_;

    //! Element selection combo box.
    QComboBox* categoryCombo_;

    //! Label for suggested names.
    QLabel* valuesLabel_;

    //! Suggested names list edit.
    ListManager* valuesList_;

    //! Label for naming format edit.
    QGroupBox* formatGroup_;

    //! Text box for naming format.
    SnippetTextEdit* formatEdit_;
};

//-----------------------------------------------------------------------------

#endif // NAMINGPOLICYSETTINGSPAGE_H
