/*
 * handbook.cpp
 *
 *  Created on: 27.05.2011
 *      Author: newmen
 */

#include <boost/regex.hpp>
#include <cmath>
#include <fstream>

#include "parse_config_error.h"
#include "handbook.h"

namespace DiamondCA {

void Handbook::parseConfig(const std::string& config_file_name) {
	std::fstream in(config_file_name.c_str());

	if (!in.is_open()) {
		throw ParseConfigError("Cannot open configuration file");
	}

	std::string line;

	boost::regex comment_regexp("^\\s*#.*$");
	boost::regex section_regexp("^\\[(.+)\\]\\s*");
	boost::regex variable_regexp("\\s*(\\w+)\\s*=\\s*([\\d\\.e-]+)\\s*");

	VarVal* current_section = 0;

	while (std::getline(in, line)) {
		boost::smatch matches;

		if (boost::regex_match(line, matches, comment_regexp)) continue;

		if (boost::regex_match(line, matches, section_regexp)) {
			std::string section_name = matches[1].str();
			current_section = &_params[section_name];
		} else if (boost::regex_match(line, matches, variable_regexp)) {
			if (current_section) {
				std::string variable = matches[1].str();
				double value = atof(matches[2].str().c_str());
				(*current_section)[variable] = value;
			} else {
				throw ParseConfigError("Variable is not in section", line);
			}
		}
	}

	VarVal sizes_section = _params.find("sizes")->second;
	for (VarVal::const_iterator it = sizes_section.begin(); it != sizes_section.end(); ++it) {
		int value = (int)(it->second);
		switch (it->first[0]) {
		case 'x':
			_sizes.x = value;
			break;
		case 'y':
			_sizes.y = value;
			break;
		case 'z':
			_sizes.z = value;
			break;
		}
	}

	VarVal temperature_section = _params.find("temperature")->second;
	_T = temperature_section.find("T")->second;
}

void Handbook::setSizes(const int3& sizes) {
	if (sizes.x > 0) _sizes.x = sizes.x;
	if (sizes.y > 0) _sizes.y = sizes.y;
	if (sizes.z > 0) _sizes.z = sizes.z;
}

double Handbook::dt() const {
	VarVal time_section = _params.find("time")->second;
	return time_section.find("dt")->second;
}

double Handbook::kMolecule(const std::string& key) const {
	double km = kMole(key);

	if (key == "abs_H" || key == "add_H" || key == "add_CH3") {
		VarVal concentrations_section = _params.find("concentrations")->second;
		double concentration = 0;
		if (key == "abs_H" || key == "add_H") {
			concentration = concentrations_section.find("H")->second;
		} else if (key == "add_CH3") {
			concentration = concentrations_section.find("CH3")->second;
		}

		km *= concentration;
	}

	return km;
}

double Handbook::kMole(const std::string& key) const {
	VarVal activation_energies_section = _params.find("activation_energies")->second;
	double Ea = activation_energies_section.find(key)->second;

	VarVal factors_section = _params.find("factors")->second;
	double A = factors_section.find(key)->second;

	return A * exp(-Ea / (R * _T));
}

double Handbook::percentOfNotDimers() const {
	return Handbook::percentOfLess(kMole("create_dimer"), kMole("drop_dimer"));
}

double Handbook::percentOfLess(double more, double less) {
	double n_more = 0;
	double n_less = 1e10;
	double start_sum = n_more + n_less;
	double delta = 1 / (pow(start_sum, 0.25) * more);

	double remember_more, remember_less;
	double new_number_of_more, new_number_of_less;
	while (true) {
		remember_more = n_more;
		remember_less = n_less;

		new_number_of_less = remember_less * more * delta;
		n_less -= new_number_of_less;
		n_more += new_number_of_less;

		new_number_of_more = remember_more * less * delta;
		n_less += new_number_of_more;
		n_more -= new_number_of_more;

		if (remember_less == n_less) break;
	}

	return n_less / start_sum;
}

}
