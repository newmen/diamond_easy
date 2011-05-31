/*
 * outputer.cpp
 *
 *  Created on: 29.05.2011
 *      Author: newmen
 */

#include <ctime>
#include <iostream>
#include <sstream>

#include "outputer.h"

namespace DiamondCA {

Outputer::Outputer(const FlagsConfig& config, const std::string& prefix) : _config(config) {
	if (!(_config.count("only-info") > 0 && _config.find("only-info")->second)
			&& !(_config.count("only-specs") > 0 && _config.find("only-specs")->second))
	{
		time_t current_time = time(0);

		std::stringstream full_prefix;
		if (prefix != "") full_prefix << prefix << '-';
		full_prefix << "out-";

		std::stringstream percent_file_name;
		percent_file_name << full_prefix.str() << "percent-" << current_time << ".txt";
		_percent_file.open(percent_file_name.str().c_str());

		if (!(_config.count("without-info") == 0 || _config.find("without-info")->second)) {
			std::stringstream info_file_name;
			info_file_name << full_prefix.str() << "info-" << current_time << ".txt";
			_info_file.open(info_file_name.str().c_str());
		}

		if (!(_config.count("without-area") == 0 || _config.find("without-area")->second)) {
			std::stringstream area_file_name;
			area_file_name << full_prefix.str() << "area-" << current_time << ".wxyz";
			_area_file.open(area_file_name.str().c_str());
		}

		if (_config.count("with-specs") > 0 && _config.find("with-specs")->second) {
			std::stringstream specs_file_name;
			specs_file_name << full_prefix.str() << "specs-" << current_time << ".txt";
			_specs_file.open(specs_file_name.str().c_str());
		}
	}

	if (_config.count("only-info") > 0 && _config.find("only-info")->second) {
		outInfoHead(std::cout);
	} else if (!(_config.count("without-info") == 0 || _config.find("without-info")->second)) {
		outInfoHead(_info_file);
	}
}

void Outputer::outputStep() {
	if (_config.count("only-info") > 0 && _config.find("only-info")->second) {
		outInfoBody(std::cout);
	} else if (_config.count("only-specs") > 0 && _config.find("only-specs")->second) {
		outSpecs(std::cout);
	} else {
		if (!(_config.count("without-info") == 0 || _config.find("without-info")->second)) {
			outInfoBody(_info_file);
		}

		if (!(_config.count("without-area") == 0 || _config.find("without-area")->second)) {
			outArea(_area_file);
		}

		if (_config.count("with-specs") > 0 && _config.find("with-specs")->second) {
			outSpecs(_specs_file);
		}
	}

}

}
