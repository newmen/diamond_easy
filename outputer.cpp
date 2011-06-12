/*
 * outputer.cpp
 *
 *  Created on: 29.05.2011
 *      Author: newmen
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "cell.h"
#include "outputer.h"

namespace DiamondCA {

Outputer::Outputer(const Configurator& cg) : _cg(&cg) {
	_config = _cg->outputerConfig();
	_start_time = time(0);

	if (!(_config.count("only-info") > 0 && _config.find("only-info")->second)
			&& !(_config.count("only-specs") > 0 && _config.find("only-specs")->second))
	{
		std::string prefix = _cg->prefix();
		std::stringstream full_prefix;
		if (prefix != "") full_prefix << prefix << '-';
		full_prefix << "out-";

		std::stringstream percent_file_name;
		percent_file_name << full_prefix.str() << "percent-" << _start_time << ".txt";
		_percent_file.open(percent_file_name.str().c_str());

		if (!(_config.count("without-info") == 0 || _config.find("without-info")->second)) {
			std::stringstream info_file_name;
			info_file_name << full_prefix.str() << "info-" << _start_time << ".txt";
			_info_file.open(info_file_name.str().c_str());
		}

		if (!(_config.count("without-area") == 0 || _config.find("without-area")->second)) {
			std::stringstream area_file_name;
			area_file_name << full_prefix.str() << "area-" << _start_time << ".wxyz";
			_area_file.open(area_file_name.str().c_str());
		}

		if (_config.count("with-specs") > 0 && _config.find("with-specs")->second) {
			std::stringstream specs_file_name;
			specs_file_name << full_prefix.str() << "specs-" << _start_time << ".txt";
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

void Outputer::outputConfigInfo(const Handbook& hb) const {
	std::ostream &oci = std::cout;

	const double rad = M_PI / 180;
	const double cc_bond_length = 0.15e-9; // метров
	const double ccc_angle = 109.28; // градусов
	const double level_width = cc_bond_length * cos(rad * ccc_angle * 0.5);

	const Cell info_cell(_cg->initialSpec(), 0, 0, 0);

	oci << "Используется конфигурационный файл: " << _cg->configFileName() << "\n"
			<< "Префикс выходных файлов: " << _cg->prefix() << "\n"
			<< "Постфикс выходных файлов: " << _start_time << "\n"
			<< "Размеры автомата (в клетках): X = " << hb.sizes().x << ", Y = " << hb.sizes().y << ", Z = " << hb.sizes().z << "\n"
//			<< "Расстояние между слоями атомов: " << level_width << " м.\n"
//			<< "Размер по X: " << hb.sizes().x * level_width * 1e9 << " нм.\n"
//			<< "Размер по Y: " << hb.sizes().y * level_width * 1e9 << " нм.\n"
			<< "Площадь подложки: " << (hb.sizes().x * level_width) * (hb.sizes().y * level_width) * 1e18 << " кв.нм.\n"
			<< "Нижний начальный слой инициализирован: " << info_cell.spec() << "\n"
//			<< "Всего шагов по времени: " << _cg->steps() << ", сохранение происходит каждый " << _cg->anyStep() << " шаг\n"
//			<< "Всего рассчитывается " << formatTime(_cg->steps() * hb.dt()) << "процесса, шаг по времени " << hb.dt() << " сек.\n"
//			<< "Результаты сохраняются раз в " << _cg->anyStep() * hb.dt() << " сек. процесса\n"
			<< "Всего рассчитывается " << formatTime(_cg->fullTime()) << "процесса, шаг по времени " << hb.dt() << " сек.\n"
			<< "Результаты сохраняются раз в " << _cg->anyTime() << " сек. процесса\n"
			<< "Температура: " << hb.temperature() << " K\n"
			<< "Скорость отрыва водорода: " << hb.kMolecule("abs_H") << " 1/сек\n"
			<< "Скорость осаждения водорода: " << hb.kMolecule("add_H") << " 1/сек\n"
			<< "Скорость миграции водорода: " << hb.kMolecule("migrate_H") << " 1/сек\n"
			<< "Скорость отделения метил-радикала: " << hb.kMolecule("add_CH3") << " 1/сек\n"
			<< "Процент разрываемых димеров: " << hb.percentOfNotDimers() * 100 << "%\n"
			<< "\n"
			<< "Образование/разрыв димеров " << (_cg->automataConfig()["dimers-form-drop"] ? "включёно" : "отключёно") << "\n"
			<< "Миграция водорода " << (_cg->automataConfig()["hydrogen-migration"] ? "включёна" : "отключёна") << "\n"
			<< "Активация поверхности " << (_cg->automataConfig()["activate-surface"] ? "включёна" : "отключёна") << "\n"
			<< "Деактивация поверхности " << (_cg->automataConfig()["deactivate-surface"] ? "включёна" : "отключёна") << "\n"
			<< "Адсорбция метила " << (_cg->automataConfig()["methyl-adsorption"] ? "включёна" : "отключёна") << "\n"
			<< "Миграция мостовой группы " << (_cg->automataConfig()["bridge-migration"] ? "включёна" : "отключёна") << "\n"
			<< "Миграция мостовой группы вверх-вниз " << (_cg->automataConfig()["bridge-migration-up-down"] ? "включёна" : "отключёна") << "\n"
			<< "\n";

	oci << "Файл для визуализации ";
	if (_config.count("without-area") == 0 || _config.find("without-area")->second) oci << "не ";
	oci << "сохраняется\n";

	oci << "Инфо ";
	if (_config.count("without-info") == 0 || _config.find("without-info")->second) oci << "не ";
	oci << "сохраняется\n";

	oci << "Содержащиеся виды ";
	if (!(_config.count("with-specs") > 0 && _config.find("with-specs")->second)) oci << "не ";
	oci << "сохраняются в текстовом виде\n";
}

void Outputer::outputCalcTime() const {
	std::ostream &oct = std::cout;
	oct << "\nРассчётное время: " << formatTime(time(0) - _start_time) << std::endl;
}


#define IN_DAY 86400
#define IN_HOUR 3600
#define IN_MINUTE 60
std::string Outputer::formatTime(float secs) {
	int days = (int)(secs / IN_DAY);
	secs -= days * IN_DAY;
	int hours = (int)(secs / IN_HOUR);
	secs -= hours * IN_HOUR;
	int minutes = (int)(secs / IN_MINUTE);
	secs -= minutes * IN_MINUTE;

	std::stringstream result;
	if (days > 0) result << humanName(days, "день", "дня", "дней");
	if (hours > 0) result << humanName(hours, "час", "часа", "часов");
	if (minutes > 0) result << humanName(minutes, "минута", "минуты", "минут");
	if (secs > 0 || result.str().length() == 0) result << humanName(secs, "секунда", "секунды", "секунд");
	return result.str();
}

std::string Outputer::humanName(float value, const char* one, const char* few, const char* many) {
	std::stringstream translator;
	translator << value;
	int length = translator.str().length();

	int last_two = 0;
	if (length > 1) last_two = atoi(translator.str().substr(length - 2).c_str());

	int last_one = atoi(translator.str().substr(length - 1).c_str());

	translator << ' ';
	if (last_two > 10 && last_two < 15) translator << many;
	else {
		if (last_one == 1) translator << one;
		else if (last_one > 1 && last_one < 5) translator << few;
		else translator << many;
	}

	translator << ' ';
	return translator.str();
}

}
