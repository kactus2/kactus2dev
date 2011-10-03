/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef TYPEBOX_H_
#define TYPEBOX_H_

#include <QGroupBox>
#include <QCheckBox>
#include <QObject>

/*! \brief The widget for user the select the document types to contain in search.
 *
 */
class TypeBox: public QGroupBox {

	Q_OBJECT

public:

	//! \brief The struct that contains the info on which types to include in search.
	struct TypeSearchSettings {

		//! \brief include abstractors
		bool abstractors_;

		//! \brief include abstraction definitions
		bool abstractionDefs_;

		//! \brief include bus definitions
		bool busDefs_;

		//! \brief include components
		bool components_;

		//! \brief include designs
		bool designs_;

		//! \brief include design configurations
		bool designConfs_;

		//! \brief include generator chains
		bool generatorChains_;

		/*! \brief The constructor
		 *
		 * \param abstractors setting for abstractors
		 * \param abstractionDefs setting for abstraction definitions
		 * \param busDefs setting for bus definitions
		 * \param components setting for components
		 * \param designs setting for designs
		 * \param designConfs setting for design configurations
		 * \param generatorChains setting for generator chains
		 *
		*/
		TypeSearchSettings(
				bool abstractors,
				bool abstractionDefs,
				bool busDefs,
				bool components,
				bool designs,
				bool designConfs,
				bool generatorChains);

		/*! \brief Default constructor
		 *
		 * Constructs struct with all fields set to TRUE.
		*/
		TypeSearchSettings();
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	TypeBox(QWidget* parent);

	//! \brief The destructor
	virtual ~TypeBox();

public slots:

	//! \brief Handler when the state of one of the check boxes changes state.
	void onCheckStateChanged();

signals:

	//! \brief Inform that the state of the filter should change
	void stateChanged(const TypeBox::TypeSearchSettings* settings);

private:

	//! \brief No copying
	TypeBox(const TypeBox& other);

	//! \brief No assignment
	TypeBox& operator=(const TypeBox& other);

	//! \brief CheckBox to select/deselect spirit:abstractors within search
	QCheckBox searchAbstractors_;

	//! \brief CheckBox to select/deselct abstraction definitions within search
	QCheckBox searchAbstractionDefs_;

	//! \brief CheckBox to select/deselect bus definitions within search
	QCheckBox searchBusDefs_;

	//! \brief CheckBox to select/deselect components within search
	QCheckBox searchComponents_;

	//! \brief CheckBox to select/deselect designs within search
	QCheckBox searchDesigns_;

	//! \brief CheckBox to select/deselect design configurations within search
	QCheckBox searchDesignConfs_;

	//! \brief CheckBox to select/deselect generator chains within search
	QCheckBox searchGeneratorChains_;

	//! \brief Contains the search settings for this group
	TypeSearchSettings settings_;
};

#endif /* TYPEBOX_H_ */
