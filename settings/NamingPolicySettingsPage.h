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

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include <QPlainTextEdit>

class ListEditor;
class SnippetTextEdit;

//-----------------------------------------------------------------------------
//! NamingPolicySettingsPage class.
//-----------------------------------------------------------------------------
class NamingPolicySettingsPage : public PropertyPageView
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
    ~NamingPolicySettingsPage();

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange();

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

    //! The settings store.
    QSettings& settings_;

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
    ListEditor* valuesList_;

    //! Label for naming format edit.
    QLabel* formatLabel_;

    //! Text box for naming format.
    SnippetTextEdit* formatEdit_;
};

//-----------------------------------------------------------------------------

#endif // NAMINGPOLICYSETTINGSPAGE_H
