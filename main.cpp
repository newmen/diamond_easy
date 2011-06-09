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

	Outputer outputer(configurator);
	outputer.outputConfigInfo(handbook);

	Automata ca(handbook, configurator.automataConfig(), outputer);
	ca.stickToCells(configurator.initialSpec(), RangeOfCells(1, 1));
	ca.stickToCells("*", RangeOfCells(1, 1), RangeOfCells(1, 2), RangeOfCells(1, 2));
	ca.stickToCells("*", RangeOfCells(2, 2), RangeOfCells(1, 2), RangeOfCells(1, 1));
	ca.stickToCells("*H", RangeOfCells(3, 3), RangeOfCells(2, 2), RangeOfCells(1, 1));
	ca.stickToCells("*", RangeOfCells(1, 1), RangeOfCells(4, 5), RangeOfCells(4, 5));
	ca.stickToCells("*", RangeOfCells(2, 2), RangeOfCells(4, 5), RangeOfCells(4, 4));
	ca.stickToCells("*H", RangeOfCells(3, 3), RangeOfCells(5, 5), RangeOfCells(4, 4));
	ca.stickToCells("*", RangeOfCells(1, 1), RangeOfCells(7, 8), RangeOfCells(7, 8));
	ca.stickToCells("*", RangeOfCells(2, 2), RangeOfCells(7, 8), RangeOfCells(7, 7));
	ca.stickToCells("*H", RangeOfCells(3, 3), RangeOfCells(8, 8), RangeOfCells(7, 7));
	ca.run(configurator.steps(), configurator.anyStep());

	outputer.outputCalcTime();

	return 0;
}

