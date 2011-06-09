/*
 * outputer.h
 *
 *  Created on: 29.05.2011
 *      Author: newmen
 */

#ifndef OUTPUTER_H_
#define OUTPUTER_H_

#include <ctime>
#include <fstream>
#include <string>

#include "automata.h"
#include "configurator.h"
#include "flags_config.h"

namespace DiamondCA {

class Outputer {
public:
	Outputer(const Configurator& cg);
	virtual ~Outputer() { }

	void setAutomata(const Automata* ca) { _ca = ca; }

	void outputPercent(float percent) { _percent_file << percent << std::endl; }
	void outputStep();

	void outputConfigInfo(const Handbook& hb) const;
	void outputCalcTime() const;

private:
	Outputer() { }

	void outInfoHead(std::ostream& os) { os << _ca->infoHead(); }
	void outInfoBody(std::ostream& os) { os << _ca->infoBody(); }
	void outArea(std::ostream& os) { os << _ca->typesArea(); }
	void outSpecs(std::ostream& os) { os << _ca->specsArea(); }

	static std::string formatTime(float secs);
	static std::string humanName(float value, const char* one, const char* few, const char* many);

private:
	FlagsConfig _config;

	const Automata* _ca;
	const Configurator* _cg;

	std::ofstream _percent_file;
	std::ofstream _info_file;
	std::ofstream _area_file;
	std::ofstream _specs_file;

	time_t _start_time;
};

}

#endif /* OUTPUTER_H_ */
