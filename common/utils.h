/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: utils.h
 * 		
 * 		Description: This file contains declarations for general purpose 
 * 		functions that can be used in the whole software.
 */

#ifndef UTILS_H
#define UTILS_H

#include <QObject>

namespace Utils {

	/*! \brief Convert QCheckBoxes checkstate to bool.
	 *
	 * \param state The boxes checkstate
	 *
	 * \return bool True if the item was checked. If not checked or partially
	 * checked then false.
	*/
	bool checkState2Bool(const Qt::CheckState state);

	/*! \brief Convert boolean value to Qt::CheckState
	 *
	 * \param state The boolean value to convert.
	 *
	 * \return Qt::Checked if state if true, otherwise returns Qt::Unchecked.
	*/
	Qt::CheckState bool2CheckState(const bool state);

	/*! \brief Convert QCheckBox's signaled check state to bool
	 *
	 * \param state The box's check state.
	 *
	 * \return bool True if state was 2, otherwise false.
	*/
	bool checkBoxState2Bool(const int state);

	//! \brief Contains firmness search settings.
	struct ImplementationOptions {
	
		//! \brief If true then hardware components should be included in search.
		bool hw_;

		//! \brief If true then software components should be included in search.
		bool sw_;

		//! \brief If true then system components should be included in search.
		bool system_;

		/*! \brief The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
		ImplementationOptions();
	};

	//! \brief Contains the search settings for hierarchy.
	struct HierarchyOptions {

		//! \brief If true then global objects should be included in search.
		bool global_;

		//! \brief If true then product objects should be included in search.
		bool product_;

		//! \brief If true then board objects should be included in search.
		bool board_;

		//! \brief If true then chip objects should be included in search.
		bool chip_;

		//! \brief If true then soc objects should be included in search.
		bool soc_;

		//! \brief If true then ip objects should be included in search.
		bool ip_;

		/*! \brief The default constructor.
		 *
		 * Constructs struct with all options set to true.
		*/
		HierarchyOptions();
	};

	//! \brief Contains the Re-usability search filters.
	struct FirmnessOptions {

		//! \brief If true then templates should be included in search.
		bool templates_;

		//! \brief If true then mutables should be included in search.
		bool mutable_;

		//! \brief If true then parameterizables should be included in search.
		bool parameterizable_;

		//! \brief If true then fixeds should be included in search.
		bool fixed_;

		/*! \brief The default constructor
		 *
		 * Constructs struct with all options set to true
		*/
		FirmnessOptions();
	};

	//! \brief Contains the search options for document types.
	struct TypeOptions {

		//! \brief If true then components should be included in search.
		bool components_;

		//! \brief If true then bus definitions should be included in search.
		bool buses_;

		//! \brief If true then other IP-Xact types should be included in search.
		bool advanced_;

		/*! \brief The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
		TypeOptions();
	};

	//! \brief The struct to handle a port name and its left and right boundary.
	struct Mapping {

		QString portName_;

		int left_;

		int right_;

		Mapping();

		Mapping(const QString& name);

		Mapping(const QString& name, int left, int right);

		//! \brief Operator that returns true if the port names are identical.
		bool operator==(const Mapping& other) const;

		//! \brief Operator that returns true if the port names are not identical.
		bool operator!=(const Mapping& other) const;

		//! \brief Operator for sorting mappings by the name
		bool operator<(const Mapping& other) const;

		//! \brief Operator for sorting mappings by name
		bool operator>(const Mapping& other) const;
	};

}

#endif // UTILS_H
