/*
 * cell.cpp
 *
 *  Created on: 28.05.2011
 *      Author: newmen
 */

#include <sstream>

#include "cell.h"

namespace DiamondCA {

Cell::Cell(const char* mix, int z, int y, int x) : _coords(z, y, x) {
	compose(mix);
}

void Cell::compose(const char* mix) {
	_active = Cell::parse_mix(mix, '*');
	_hydro = Cell::parse_mix(mix, 'H');
}

int Cell::parse_mix(const char* mix, char spec) {
	int sn = 0;
	for (int i = 0; mix[i]; ++i) {
		if (mix[i] == spec) ++sn;
	}

	return sn;
}

int Cell::type() const {
	int t;
	if (_active == 0 && _hydro == 0) t = 1;
	else if (_active == 1 && _hydro == 0) t = 2;
	else if (_active == 0 && _hydro == 1) t = 3;
	else if (_active == 2 && _hydro == 0) t = 4;
	else if (_active == 1 && _hydro == 1) t = 5;
	else if (_active == 0 && _hydro == 2) t = 6;
	else t = 7;

	return t;
}

std::string Cell::spec() const {
	int ckey = cache_key();
	if (Cell::cache_of_specs.count(ckey) > 0) return Cell::cache_of_specs[ckey];

	std::stringstream ss;
	for (int i = 0; i < _active; ++i) ss << '*';
	ss << 'C';
	if (_hydro > 0) {
		ss << 'H';
		if (_hydro > 1) ss << _hydro;
	}

	Cell::cache_of_specs[ckey] = ss.str();
	return Cell::cache_of_specs[ckey];
}

std::map< int, std::string > Cell::cache_of_specs = std::map< int, std::string >();

}
