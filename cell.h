/*
 * cell.h
 *
 *  Created on: 28.05.2011
 *      Author: newmen
 */

#ifndef CELL_H_
#define CELL_H_

#include <map>
#include <string>

#include "int3.h"

namespace DiamondCA {

class Cell {
public:
	Cell(const char* mix, int z, int y, int x);
	virtual ~Cell() { }

	void compose(const char* mix);

	int active() const { return _active; }
	int hydro() const { return _hydro; }
	int3 coords() const { return _coords; }
	void setCoords(const int3& coords) { _coords = coords; }

	void activate() { ++_active; }
	void deactivate() { --_active; }
	void addHydrogen() {
		deactivate();
		++_hydro;
	}
	void removeHydrogen() {
		activate();
		--_hydro;
	}

	int type() const;
	std::string spec() const;

private:
	Cell() { }

	int cache_key() const { return 10 * _active + _hydro; }

	static int parse_mix(const char* mix, char spec);

private:
	int _active;
	int _hydro;
	int3 _coords;

	static std::map< int, std::string > cache_of_specs;
};

}

#endif /* CELL_H_ */
