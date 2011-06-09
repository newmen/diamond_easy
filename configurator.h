/*
 * configurator.h
 *
 *  Created on: 30.05.2011
 *      Author: newmen
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_

#include <string>

#include "int3.h"
#include "flags_config.h"

#define CONFIG_FILE "handbook.cnf"
#define INITIAL_SPEC "*H"
#define STEPS 10000
#define ANY_STEP 1000

namespace DiamondCA {

class Configurator {
public:
	Configurator();
	virtual ~Configurator() { }

	void parseParams(int argc, char* argv[]);

	const char* programName() const { return _program_name.c_str(); }
	bool needHelp() const { return _need_help; }
	const char* configFileName() const { return _config_file_name.c_str(); }
	int3 sizes() const { return _sizes; }
	const char* initialSpec() const { return _initial_spec.c_str(); }
	unsigned int steps() const { return _steps; }
	unsigned int anyStep() const { return _any_step; }
	FlagsConfig automataConfig() const { return _automata_config; }
	FlagsConfig outputerConfig() const { return _outputer_config; }
	std::string prefix() const { return _prefix; }

	std::string help() const;

private:
	std::string _program_name;
	bool _need_help;
	std::string _config_file_name;
	int3 _sizes;
	std::string _initial_spec;
	unsigned int _steps, _any_step;
	FlagsConfig _automata_config;
	FlagsConfig _outputer_config;
	std::string _prefix;
};

}

#endif /* CONFIGURATOR_H_ */
