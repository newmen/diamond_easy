/*
 * configurator.cpp
 *
 *  Created on: 30.05.2011
 *      Author: newmen
 */

#include <boost/regex.hpp>
#include <sstream>

#include "configurator.h"
#include "parse_error.h"
#include "parse_params_error.h"

namespace DiamondCA {

Configurator::Configurator() : _need_help(false), _config_file_name(CONFIG_FILE),
		_initial_spec(INITIAL_SPEC), _steps(STEPS), _any_step(ANY_STEP), _prefix("")
{
	_automata_config["dimers-form-drop"] = true;
	_automata_config["hydrogen-migration"] = true;
	_automata_config["activate-surface"] = true;
	_automata_config["deactivate-surface"] = true;
	_automata_config["methyl-adsorption"] = true;
	_automata_config["bridge-migration"] = true;
	_automata_config["bridge-migration-down"] = true;

	_outputer_config["only-info"] = false;
	_outputer_config["only-specs"] = false;
	_outputer_config["without-area"] = false;
	_outputer_config["without-info"] = false;
	_outputer_config["with-specs"] = false;
}

void Configurator::parseParams(int argc, char* argv[]) {
	_program_name = argv[0];

	boost::regex rx_help("-h|--help");
	boost::regex rx_config_file_name("(-c|--config)=([\\w\\.]+)");
	boost::regex rx_size_x("(-x|--size-x)=(\\d+)");
	boost::regex rx_size_y("(-y|--size-y)=(\\d+)");
	boost::regex rx_size_z("(-z|--size-z)=(\\d+)");
	boost::regex rx_spec("(-is|--spec)=([*H]{2})");
	boost::regex rx_steps("(-st|--steps)=(\\d+)");
	boost::regex rx_any_step("(-as|--any-step)=(\\d+)");
	boost::regex rx_wo_dfd("-wo-dfd|--without-dimers-form-drop");
	boost::regex rx_wo_hm("-wo-hm|--without-hydrogen-migration");
	boost::regex rx_wo_as("-wo-as|--without-activate-surface");
	boost::regex rx_wo_ds("-wo-ds|--without-deactivate-surface");
	boost::regex rx_wo_ma("-wo-ma|--without-methyl-adsorption");
	boost::regex rx_wo_bm("-wo-bm|--without-bridge-migration");
	boost::regex rx_wo_bmd("-wo-bmd|--without-bridge-migration-down");
	boost::regex rx_oi("-oi|--only-info");
	boost::regex rx_os("-os|--only-specs");
	boost::regex rx_wo_a("-wo-a|--without-area");
	boost::regex rx_wo_i("-wo-i|--without-info");
	boost::regex rx_w_s("-w-s|--with-specs");
	boost::regex rx_prefix("^([^-][\\S]*)$");

	for (int i = 1; i < argc; ++i) {
		std::string current_param = argv[i];
		boost::smatch matches;

		if (boost::regex_match(current_param, matches, rx_help)) _need_help = true;
		else if (boost::regex_match(current_param, matches, rx_config_file_name)) _config_file_name = matches[2];
		else if (boost::regex_match(current_param, matches, rx_size_x)) _sizes.x = atoi(matches[2].str().c_str());
		else if (boost::regex_match(current_param, matches, rx_size_y)) _sizes.y = atoi(matches[2].str().c_str());
		else if (boost::regex_match(current_param, matches, rx_size_z)) _sizes.z = atoi(matches[2].str().c_str());
		else if (boost::regex_match(current_param, matches, rx_spec)) _initial_spec = matches[2];
		else if (boost::regex_match(current_param, matches, rx_steps)) _steps = atoi(matches[2].str().c_str());
		else if (boost::regex_match(current_param, matches, rx_any_step)) _any_step = atoi(matches[2].str().c_str());
		else if (boost::regex_match(current_param, matches, rx_wo_dfd)) _automata_config["dimers-form-drop"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_hm)) _automata_config["hydrogen-migration"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_as)) _automata_config["activate-surface"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_ds)) _automata_config["deactivate-surface"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_ma)) _automata_config["methyl-adsorption"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_bm)) _automata_config["bridge-migration"] = false;
		else if (boost::regex_match(current_param, matches, rx_wo_bmd)) _automata_config["bridge-migration-down"] = false;
		else if (boost::regex_match(current_param, matches, rx_oi)) _outputer_config["only-info"] = true;
		else if (boost::regex_match(current_param, matches, rx_os)) _outputer_config["only-specs"] = true;
		else if (boost::regex_match(current_param, matches, rx_wo_a)) _outputer_config["without-area"] = true;
		else if (boost::regex_match(current_param, matches, rx_wo_i)) _outputer_config["without-info"] = true;
		else if (boost::regex_match(current_param, matches, rx_w_s)) _outputer_config["with-specs"] = true;
		else if (i == argc - 1 && boost::regex_match(current_param, matches, rx_prefix)) _prefix = matches[1];
		else throw ParseParamsError("Undefined parameter", current_param);
	}

	if (_outputer_config["only-info"] && _outputer_config["only-specs"]) {
		throw ParseError("Cannot use -oi (--only-info) with -os (--only-specs)");
	}
}

std::string Configurator::help() const {
	std::stringstream result;
	result << "Расчёт процесса роста кристалла алмаза CVD методом клеточного автомата\n"
			<< "(c) 2009-2012 РХТУ им. Д.И. Менделеева, каф. ИКТ, Г.Ю. Аверчук\n\n"

			<< "Запуск программы:\n"
			<< "  " << _program_name << " [параметры] [префикс_выходных_файлов]\n\n"

			<< "Параметры:\n"
			<< "  -h, --help - эта справка\n\n"

			<< "  -c=конфигурационный_файл, --config=конфигурационный_файл - задаёт конфигурационный файл "
			<< "(по умолчанию "<< _config_file_name << ")\n\n"

			<< "Размеры автомата, заменяют указанные в конфигурационном файле\n"
			<< "  -x=число, --size-x=число - размер автомата по X\n"
			<< "  -y=число, --size-x=число - размер автомата по Y\n"
			<< "  -z=число, --size-x=число - размер автомата по Z\n\n"

			<< "  -is=содержание, --spec=содержание - инициализация нижнего слоя углеродов активными связями и водородом, "
			<< "общее количество активных связей и водорода должно быть равно двум (по умолчанию "
			<< _initial_spec << ")\n\n"

			<< "  -st=число, --steps=число - число шагов по времени (по умолчанию " << _steps << ")\n"
			<< "  -as=число, --any-step=число - вывод результатов, когда шаг кратен этому значению"
			<< "(по умолчанию " << _any_step << ")\n\n"

			<< "  -wo-dfd, --without-dimers-form-drop - не использовать образование/рызрыв димеров\n"
			<< "  -wo-hm, --without-hydrogen-migration - не использовать миграцию водорода по димеру\n"
			<< "  -wo-as, --without-activate-surface - не активировать поверхность водородом газовой фазы\n"
			<< "  -wo-ds, --without-deactivate-surface - не деактивировать поверхность водородом газовой фазы\n"
			<< "  -wo-ma, --without-methyl-adsorption - не осаждать метил радикал\n"
			<< "  -wo-bm, --without-bridge-migration - отменить миграцию мостовой группы\n"
			<< "  -wo-bmd, --without-bridge-migration-down - отменить миграцию мостовой группы вниз "
			<< "(миграция вниз не работает без \"обычной\" миграции)\n\n"

			<< "  -oi, --only-info - выводить информацию в стандартный поток вывода и не сохранять выходные файлы\n"
			<< "  -os, --only-specs - выводить содержащиеся виды в стандартный поток вывода и не сохранять выходные файлы\n\n"

			<< "  -wo-a, --without-area - не сохранять файл для визуализации\n"
			<< "  -wo-i, --without-info - не сохранять инфо\n"
			<< "  -w-s, --with-specs - сохранять содержащиеся виды в текстовом виде\n";

	return result.str();
}

}
