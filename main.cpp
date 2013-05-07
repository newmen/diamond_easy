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
	} catch(const ParseParamsError* e) {
		std::cerr << e->getMessage() << '\n'
				<< "See " << configurator.programName() << " --help" << std::endl;
		return 1;
	} catch(const ParseError* e) {
		std::cerr << e->getMessage() << std::endl;
		return 1;
	}

	if (configurator.needHelp()) {
		std::cout << configurator.help() << std::endl;
		return 0;
	}

	Handbook handbook;
	try {
		handbook.parseConfig(configurator.configFileName());
	} catch(const ParseConfigError* e) {
		std::cerr << "Configuration file (" << configurator.configFileName() << ") contains error: "
				<< e->getMessage() << std::endl;
		return 1;
	}
	handbook.setSizes(configurator.sizes());

	Outputer outputer(configurator);
	outputer.outputConfigInfo(handbook);

	Automata ca(handbook, configurator.automataConfig(), outputer);
	ca.stickToCells(configurator.initialSpec(), Range(1, 1));
	ca.stickToCells("*", Range(1, 1), Range(1, 2), Range(1, 2));
	ca.stickToCells("*", Range(2, 2), Range(1, 2), Range(1, 1));
	ca.stickToCells("*H", Range(3, 3), Range(2, 2), Range(1, 1));
	ca.stickToCells("*", Range(1, 1), Range(4, 5), Range(4, 5));
	ca.stickToCells("*", Range(2, 2), Range(4, 5), Range(4, 4));
	ca.stickToCells("*H", Range(3, 3), Range(5, 5), Range(4, 4));
	ca.stickToCells("*", Range(1, 1), Range(7, 8), Range(7, 8));
	ca.stickToCells("*", Range(2, 2), Range(7, 8), Range(7, 7));
	ca.stickToCells("*H", Range(3, 3), Range(8, 8), Range(7, 7));
	ca.run(configurator.fullTime(), configurator.anyTime());

	outputer.outputCalcTime();

	return 0;
}

