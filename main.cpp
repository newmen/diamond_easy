#include <iostream>

#include "automata.h"
#include "configurator.h"
#include "handbook.h"
#include "outputer.h"
#include "parse_error.h"
#include "parse_config_error.h"
#include "parse_params_error.h"

using namespace DiamondCA;

int main(int argc, char* argv[]) {
	Configurator configurator;

	try {
		configurator.parseParams(argc, argv);
	} catch(const ParseParamsError& e) {
		std::cerr << e.getMessage() << '\n'
				<< "See " << configurator.programName() << " --help" << std::endl;
		return 1;
	} catch(const ParseError& e) {
		std::cerr << e.getMessage() << std::endl;
		return 1;
	}

	if (configurator.needHelp()) {
		std::cout << configurator.help() << std::endl;
		return 0;
	}

	Handbook handbook;
	try {
		handbook.parseConfig(configurator.configFileName());
	} catch(const ParseConfigError& e) {
		std::cerr << "Configuration file (" << configurator.configFileName() << ") contains error: "
				<< e.getMessage() << std::endl;
		return 1;
	}
	handbook.setSizes(configurator.sizes());

	Outputer outputer(configurator.outputerConfig(), configurator.prefix());

	Automata ca(handbook, configurator.automataConfig(), outputer);
	ca.stickToCells(configurator.initialSpec(), RangeOfCells(1, 1));
	ca.run(configurator.steps(), configurator.anyStep());

	return 0;
}

