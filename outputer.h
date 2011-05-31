/*
 * outputer.h
 *
 *  Created on: 29.05.2011
 *      Author: newmen
 */

#ifndef OUTPUTER_H_
#define OUTPUTER_H_

#include <fstream>
#include <string>

#include "automata.h"
#include "flags_config.h"

namespace DiamondCA {

class Outputer {
public:
	Outputer(const FlagsConfig& config, const std::string& prefix);
	virtual ~Outputer() { }

	void setAutomata(const Automata* ca) { _ca = ca; }

	void outputPercent(float percent) { _percent_file << percent << std::endl; }
	void outputStep();

private:
	Outputer() { }

	void outInfoHead(std::ostream& os) { os << _ca->infoHead(); }
	void outInfoBody(std::ostream& os) { os << _ca->infoBody(); }
	void outArea(std::ostream& os) { os << _ca->typesArea(); }
	void outSpecs(std::ostream& os) { os << _ca->specsArea(); }

private:
	FlagsConfig _config;

	const Automata* _ca;

	std::ofstream _percent_file;
	std::ofstream _info_file;
	std::ofstream _area_file;
	std::ofstream _specs_file;
};

}

#endif /* OUTPUTER_H_ */
