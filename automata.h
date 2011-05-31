/*
 * automata.h
 *
 *  Created on: 28.05.2011
 *      Author: newmen
 */

#ifndef AUTOMATA_H_
#define AUTOMATA_H_

#include <map>
#include <set>
#include <string>

#include "int3.h"
#include "flags_config.h"
#include "cell.h"
#include "handbook.h"

namespace DiamondCA {

typedef std::pair<int, int> RangeOfCells;
typedef std::set<Cell*> SetOfCells;
typedef std::map<Cell*, Cell*> CellToCell;

class Outputer;

class Automata {
public:
	Automata(const Handbook& handbook, const FlagsConfig& config, Outputer& outputer);
	virtual ~Automata();

	void stickToCells(const char* mix, const RangeOfCells& z_range);
	void stickToCells(const char* mix, const RangeOfCells& z_range, const RangeOfCells& y_range);
	void stickToCells(const char* mix, const RangeOfCells& z_range, const RangeOfCells& y_range,
			const RangeOfCells& x_range);

	std::string typesArea() const;
	std::string specsArea() const;

	std::string infoHead() const;
	std::string infoBody() const;

	void run(unsigned int steps, unsigned int out_any_step = 1);

private:
	Automata() { }

	void exploreArea();

	void migratingHydrogen();
	void activatingSurface();
	void deactivatingSurface();
	void addingBridges();
	void migratingBridges();
	void formingDimers();
	void droppingDimers();

	static SetOfCells* differentCells(const SetOfCells& s1, const SetOfCells& s2);

	void activate(Cell* cell);
	void deactivate(Cell* cell);
	bool isAvailableForMigrating(Cell* cells[2]) const;
	bool isDimer(Cell* cells[2]) const;
	void formDimerPart(Cell* cell);
	void deleteDimer(Cell* cell1, Cell* cell2);
	static void topNeighbourCoords(const int3& coords1, const int3& coords2, int3& top_neighbour_coords);
	void directNeighboursCoords(const int3& current_coords, int3 direct_neighbours_coords[2]) const;
	void flatNeighboursCoords(const int3& current_coords, int3 flat_neighbours_coords[2][2]) const;
	void bottomNeighboursCells(const int3& current_coords, Cell* bottom_neighbours_cells[2]) const;
	void bottomNeighboursCoords(const int3& current_coords, int3 bottom_neighbours_coords[2]) const;
	void torusCoordinate(char coord, int current, int& less, int& more) const;

private:
	FlagsConfig _config;
	Outputer* _outputer;

	int3 _sizes;
	Cell**** _cells;

	float _dt;
	double _k_abs_H_dt;
	double _k_add_H_dt;
	double _k_add_CH3_dt;
	double _k_migrate_H_dt;
	double _percent_of_not_dimers;

	CellToCell _dimer_bonds;

	SetOfCells _dimers;
	SetOfCells _actives;
	SetOfCells _hydrides;

	float _time;
	int _max_z;
	int _carbons_num;
	int _hydrogen_atoms_num;
	int _active_bonds_num;
	int _active_bridges_num;
	int _active_dimers_num;
	int _abstracted_hydrogen_atoms_num;
	int _adsorbed_hydrogen_atoms_num;
	int _adsorbed_methyl_radicals_num;
	int _migrated_hydrogen_atoms_num;
	int _migrated_bridges_num;
};

}

#endif /* AUTOMATA_H_ */
