/*
 * automata.cpp
 *
 *  Created on: 28.05.2011
 *      Author: newmen
 */

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <vector>
//#include <iostream>

#include "automata.h"
#include "outputer.h"

namespace DiamondCA {

typedef std::vector<Cell*> VariantCells;
typedef std::vector<int3> VariantCoords;

Automata::Automata(const Handbook& handbook, const FlagsConfig& config, Outputer& outputer) :
		_config(config), _outputer(&outputer),
		_hydrogen_atoms_num(0), _active_bonds_num(0),
//		_active_bridges_num(0),
		_bridges_num(0),
		_active_dimers_num(0),
		_abstracted_hydrogen_atoms_num(0), _adsorbed_hydrogen_atoms_num(0), _adsorbed_methyl_radicals_num(0),
		_migrated_hydrogen_atoms_num(0), _migrated_bridges_num(0)
{
	_outputer->setAutomata(this);

	_sizes = handbook.sizes();

	_cells = new Cell***[_sizes.z];
	for (int iz = 0; iz < _sizes.z; ++iz) {
		_cells[iz] = new Cell**[_sizes.y];
		for (int iy = 0; iy < _sizes.y; ++iy) {
			_cells[iz][iy] = new Cell*[_sizes.x];
			for (int ix = 0; ix < _sizes.x; ++ix) {
				_cells[iz][iy][ix] = 0;
			}
		}
	}

	stickToCells("", RangeOfCells(0, 0));

	_dt = handbook.dt();
	_k_abs_H_dt = handbook.kMolecule("abs_H") * _dt;
	_k_add_H_dt = handbook.kMolecule("add_H") * _dt;
	_k_add_CH3_dt = handbook.kMolecule("add_CH3") * _dt;
	_k_migrate_H_dt = handbook.kMolecule("migrate_H") * _dt;
	_percent_of_not_dimers = handbook.percentOfNotDimers();
}

Automata::~Automata() {
	for (int iz = 0; iz < _sizes.z; ++iz) {
		for (int iy = 0; iy < _sizes.y; ++iy) {
			for (int ix = 0; ix < _sizes.x; ++ix) {
				if (_cells[iz][iy][ix]) delete _cells[iz][iy][ix];
			}
			delete[] _cells[iz][iy];
		}
		delete[] _cells[iz];
	}
	delete[] _cells;
}

void Automata::stickToCells(const char* mix, const RangeOfCells& z_range) {
	RangeOfCells y_range(0, _sizes.y - 1);
	stickToCells(mix, z_range, y_range);
}

void Automata::stickToCells(const char* mix, const RangeOfCells& z_range, const RangeOfCells& y_range) {
	RangeOfCells x_range(0, _sizes.x - 1);
	stickToCells(mix, z_range, y_range, x_range);
}

void Automata::stickToCells(const char* mix, const RangeOfCells& z_range, const RangeOfCells& y_range,
		const RangeOfCells& x_range)
{
	for (int iz = z_range.first; iz <= z_range.second; ++iz) {
		for (int iy = y_range.first; iy <= y_range.second; ++iy) {
			for (int ix = x_range.first; ix <= x_range.second; ++ix) {
				if (_cells[iz][iy][ix]) _cells[iz][iy][ix]->compose(mix);
				else _cells[iz][iy][ix] = new Cell(mix, iz, iy, ix);
			}
		}
	}
}

std::string Automata::typesArea() const {
	std::stringstream area;
	for (int iz = 0; iz < _sizes.z; ++iz) {
		for (int iy = 0; iy < _sizes.y; ++iy) {
			for (int ix = 0; ix < _sizes.x; ++ix) {
				if (!_cells[iz][iy][ix]) continue;
				area << _cells[iz][iy][ix]->type() << ' ' << ix << ' ' << iy << ' ' << iz << '\n';
			}
		}
	}
	area << "0 0 0 0\n";

	return area.str();
}

std::string Automata::specsArea() const {
	std::stringstream* lines = new std::stringstream[_sizes.y];
	std::string spec;
	for (int iz = 0; iz < _sizes.z; ++iz) {
		for (int iy = 0; iy < _sizes.y; ++iy) {
			if (iz > 0) lines[iy] << "  | ";

			for (int ix = 0; ix < _sizes.x; ++ix) {
				if (_cells[iz][iy][ix]) spec = _cells[iz][iy][ix]->spec();
				else spec = ".";

				lines[iy].width(4);
				lines[iy] << spec;
			}
		}
	}

	std::stringstream area;
	for (int iy = 0; iy < _sizes.y; ++iy) {
		area << lines[iy].str() << '\n';
	}
	area << '\n';

	delete[] lines;

	return area.str();
}

std::string Automata::infoHead() const {
	std::stringstream info;
	info << "Time (sec)"
			<< "\tMax Z"
			<< "\tTotal carbons"
			<< "\tTotal hydrogen atoms"
			<< "\tTotal dimers"
			<< "\tTotal active dimers"
			<< "\tTotal active bonds"
//			<< "\tTotal active bridges"
			<< "\tTotal bridges"
			<< "\tAbstracted hydrogen atoms"
			<< "\tAdsorbed hydrogen atoms"
			<< "\tAdsorbed methyl radicals"
			<< "\tMigrated hydrogen atoms"
			<< "\tMigrated bridges"
			<< '\n';
	return info.str();
}

std::string Automata::infoBody() const {
	std::stringstream info;
	info << _time
			<< '\t' << _max_z
			<< '\t' << _carbons_num
			<< '\t' << _hydrogen_atoms_num
			<< '\t' << _dimer_bonds.size()
			<< '\t' << _active_dimers_num
			<< '\t' << _active_bonds_num
//			<< '\t' << _active_bridges_num
			<< '\t' << _bridges_num
			<< '\t' << _abstracted_hydrogen_atoms_num
			<< '\t' << _adsorbed_hydrogen_atoms_num
			<< '\t' << _adsorbed_methyl_radicals_num
			<< '\t' << _migrated_hydrogen_atoms_num
			<< '\t' << _migrated_bridges_num
			<< '\n';
	return info.str();
}

void Automata::exploreArea() {
	_max_z = 1;
	_carbons_num = 0;

	for (int iz = 0; iz < _sizes.z; ++iz) {
		for (int iy = 0; iy < _sizes.y; ++iy) {
			for (int ix = 0; ix < _sizes.x; ++ix) {
				if (!_cells[iz][iy][ix]) continue;

				++_carbons_num;
				if (iz > _max_z) _max_z = iz;
				if (_cells[iz][iy][ix]->active() > 0) _actives.insert(_cells[iz][iy][ix]);
				if (_cells[iz][iy][ix]->hydro() > 0) _hydrides.insert(_cells[iz][iy][ix]);
			}
		}
	}
}

void Automata::run(unsigned int steps, unsigned int out_any_step) {
	exploreArea();

	typedef void (Automata::*StepFunc)();
	typedef std::vector<StepFunc> StepFuncs;

	StepFuncs step_funcs;
	if (_config["hydrogen-migration"]) step_funcs.push_back(&Automata::migratingHydrogen);
	if (_config["activate-surface"]) step_funcs.push_back(&Automata::activatingSurface);
	if (_config["deactivate-surface"]) step_funcs.push_back(&Automata::deactivatingSurface);
	if (_config["methyl-adsorption"]) step_funcs.push_back(&Automata::addingBridges);
	if (_config["bridge-migration"]) step_funcs.push_back(&Automata::migratingBridges);
	if (_config["dimers-form-drop"]) {
		step_funcs.push_back(&Automata::formingDimers);
		step_funcs.push_back(&Automata::droppingDimers);
	}

	unsigned int percent_step = (unsigned int)(steps * 0.001);
	if (percent_step == 0) percent_step = 1;

	srand(time(0));

	unsigned int step = 0;
	for ( ; step <= steps; ++step) {
		if (step % percent_step == 0) _outputer->outputPercent((float)(100 * step) / steps);
		if (step % out_any_step == 0) {
			_time = step * _dt;
			_outputer->outputStep();
		}

		for (StepFuncs::const_iterator it = step_funcs.begin(); it != step_funcs.end(); ++it) {
			(this->*(*it))();
		}
	}
}

void Automata::formingDimers() {
	SetOfCells* actives_not_dimers = differentCells(_actives, _dimers);
	for (SetOfCells::iterator it = actives_not_dimers->begin(); it != actives_not_dimers->end(); ++it) {
		Cell* current_cell = *it;

//		if (actives_not_dimers->count(current_cell) == 0) continue;
		if (_dimers.count(current_cell) > 0) continue;

		VariantCells may_be_dimer;

		int3 direct_n_coords[2];
		directNeighboursCoords(current_cell->coords(), direct_n_coords);
		for (int i = 0; i < 2; ++i) {
			Cell* direct_n_cell = _cells[direct_n_coords[i].z][direct_n_coords[i].y][direct_n_coords[i].x];
			if (!direct_n_cell || direct_n_cell->active() == 0 || _dimers.count(direct_n_cell) > 0) continue;

			int3 top_n_coords;
			topNeighbourCoords(current_cell->coords(), direct_n_cell->coords(), top_n_coords);
			Cell* top_n_cell = _cells[top_n_coords.z][top_n_coords.y][top_n_coords.x];
			if (top_n_cell) continue;

			may_be_dimer.push_back(direct_n_cell);
		}

		if (may_be_dimer.empty()) continue;

		unsigned int random_index = rand() % may_be_dimer.size();
		Cell* random_direct_n_cell = may_be_dimer[random_index];

		_dimer_bonds[current_cell] = random_direct_n_cell;

		formDimerPart(current_cell);
		formDimerPart(random_direct_n_cell);

//		actives_not_dimers->erase(si);
//		actives_not_dimers->erase(random_direct_n_cell);
	}

	delete actives_not_dimers;
}

void Automata::droppingDimers() {
	VariantCells dimer_cells1(_dimer_bonds.size());
	VariantCells dimer_cells2(_dimer_bonds.size());

	int i = 0;
	for (CellToCell::const_iterator it = _dimer_bonds.begin(); it != _dimer_bonds.end(); ++it) {
		dimer_cells1[i] = it->first;
		dimer_cells2[i] = it->second;
		++i;
	}

	int dropped_dimers_num = (int)(_dimer_bonds.size() * _percent_of_not_dimers + 0.5);
	for (i = 0; i < dropped_dimers_num; ++i) {
		unsigned int random_index = rand() % dimer_cells1.size();

		VariantCells::iterator dcit1 = dimer_cells1.begin() + random_index;
		VariantCells::iterator dcit2 = dimer_cells2.begin() + random_index;

		activate(*dcit1);
		activate(*dcit2);

		deleteDimer(*dcit1, *dcit2);

		dimer_cells1.erase(dcit1);
		dimer_cells2.erase(dcit2);
	}
}

void Automata::migratingHydrogen() {
	VariantCells dimer_cells1, dimer_cells2;
	for (CellToCell::const_iterator it = _dimer_bonds.begin(); it != _dimer_bonds.end(); ++it) {
		if (it->first->active() > 0 && it->second->hydro() > 0) {
			dimer_cells1.push_back(it->first);
			dimer_cells2.push_back(it->second);
		} else if (it->second->active() > 0 && it->first->hydro() > 0) {
			dimer_cells1.push_back(it->second);
			dimer_cells2.push_back(it->first);
		}
	}

	_migrated_hydrogen_atoms_num = (int)(dimer_cells1.size() * _k_migrate_H_dt + 0.5);
	for (int i = 0; i < _migrated_hydrogen_atoms_num; ++i) {
		unsigned int random_index = rand() % dimer_cells1.size();

		VariantCells::iterator dcit1 = dimer_cells1.begin() + random_index;
		VariantCells::iterator dcit2 = dimer_cells2.begin() + random_index;

		(*dcit1)->addHydrogen();
		_actives.erase(*dcit1);
		_hydrides.insert(*dcit1);

		(*dcit2)->removeHydrogen();
		_actives.insert(*dcit2);
		_hydrides.erase(*dcit2);

		dimer_cells1.erase(dcit1);
		dimer_cells2.erase(dcit2);
	}
}

void Automata::activatingSurface() {
	VariantCells cells_with_hydro(_hydrides.size());
	int i = 0;
	_hydrogen_atoms_num = 0;
	for (SetOfCells::const_iterator it = _hydrides.begin(); it != _hydrides.end(); ++it) {
		cells_with_hydro[i++] = *it;
		_hydrogen_atoms_num += (*it)->hydro();
	}

	_abstracted_hydrogen_atoms_num = (int)(_hydrogen_atoms_num * _k_abs_H_dt + 0.5);
	for (i = 0; i < _abstracted_hydrogen_atoms_num; ++i) {
		unsigned int random_index = rand() % cells_with_hydro.size();
		VariantCells::iterator hcit = cells_with_hydro.begin() + random_index;

		(*hcit)->removeHydrogen();
		_actives.insert(*hcit);

		if ((*hcit)->hydro() > 0) continue;

		_hydrides.erase(*hcit);
		cells_with_hydro.erase(hcit);
	}
}

void Automata::deactivatingSurface() {
	VariantCells active_cells(_actives.size());
	int i = 0;
	_active_bonds_num = 0;
	for (SetOfCells::const_iterator it = _actives.begin(); it != _actives.end(); ++it) {
		active_cells[i++] = *it;
		_active_bonds_num += (*it)->active();
	}

	_adsorbed_hydrogen_atoms_num = (int)(_active_bonds_num * _k_add_H_dt + 0.5);
	for (i = 0; i < _adsorbed_hydrogen_atoms_num; ++i) {
		unsigned int random_index = rand() % active_cells.size();
		VariantCells::iterator acit = active_cells.begin() + random_index;

		(*acit)->addHydrogen();
		_hydrides.insert(*acit);

		if ((*acit)->active() > 0) continue;

		_actives.erase(*acit);
		active_cells.erase(acit);
	}
}

void Automata::addingBridges() {
	VariantCells ad_cells1, ad_cells2;
	for (CellToCell::iterator it = _dimer_bonds.begin(); it != _dimer_bonds.end(); ++it) {
		if (it->first->active() > 0 && it->second->active() > 0) {
			if (rand() % 2 == 0) {
				ad_cells1.push_back(it->first);
				ad_cells2.push_back(it->second);
			} else {
				ad_cells1.push_back(it->second);
				ad_cells2.push_back(it->first);
			}
		} else if (it->first->active() > 0) {
			ad_cells1.push_back(it->first);
			ad_cells2.push_back(it->second);
		} else if (it->second->active() > 0) {
			ad_cells1.push_back(it->second);
			ad_cells2.push_back(it->first);
		}
	}

	_active_dimers_num = ad_cells1.size();
	_adsorbed_methyl_radicals_num = (int)(_active_dimers_num * _k_add_CH3_dt + 0.5);
	for (int i = 0; i < _adsorbed_methyl_radicals_num; ++i) {
		unsigned int random_index = rand() % ad_cells1.size();

		VariantCells::iterator adcit1 = ad_cells1.begin() + random_index;
		VariantCells::iterator adcit2 = ad_cells2.begin() + random_index;

		deleteDimer(*adcit1, *adcit2);

		int3 top_n_coords;
		topNeighbourCoords((*adcit1)->coords(), (*adcit2)->coords(), top_n_coords);
		_cells[top_n_coords.z][top_n_coords.y][top_n_coords.x] =
				new Cell("HH", top_n_coords.z, top_n_coords.y, top_n_coords.x);

		_hydrides.insert(_cells[top_n_coords.z][top_n_coords.y][top_n_coords.x]);

		++_carbons_num;
		if (top_n_coords.z > _max_z) _max_z = top_n_coords.z;

		(*adcit1)->addHydrogen();
		_hydrides.insert(*adcit1);
		_actives.erase(*adcit1);

		ad_cells1.erase(adcit1);
		ad_cells2.erase(adcit2);
	}
}

void Automata::migratingBridges() {
//	SetOfCells* actives_not_dimer = differentCells(_actives, _dimers);
	SetOfCells* surface_cells = unionCells(_actives, _hydrides);
	SetOfCells* bridge_cells = differentCells(*surface_cells, _dimers);
	delete surface_cells;

//	_active_bridges_num = 0;
	_bridges_num = 0;
	_migrated_bridges_num = 0;
//	for (SetOfCells::iterator it = actives_not_dimer->begin(); it != actives_not_dimer->end(); ++it) {
	for (SetOfCells::iterator it = bridge_cells->begin(); it != bridge_cells->end(); ++it) {
		Cell* current_cell = *it;
		if (!(current_cell->active() + current_cell->hydro() > 1)) continue;
//		++_active_bridges_num;
		++_bridges_num;

		int3 current_coords = current_cell->coords();

		Cell* bottom_n_cells[2];
		bottomNeighboursCells(current_coords, bottom_n_cells);

		VariantCoords empty_cells_coords;

		int i;
		int3 flat_n_coords[2][2];
		flatNeighboursCoords(current_coords, flat_n_coords);

		// если есть 2 соседа в плоскости - не мигрирует
		int nn = 0;
		for (i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				if (!getCell(flat_n_coords[i][j])) continue;
				++nn;
			}
		}
		if (nn > 1) continue;

		int3* direct_n_coords = flat_n_coords[0];
		int3* across_n_coords = flat_n_coords[1];

		for (i = 0; i < 2; ++i) {
			Cell* direct_n_cell = getCell(direct_n_coords[i]);
			if (!direct_n_cell) {
				int3 direct_bottom_n_coords[2];
				bottomNeighboursCoords(direct_n_coords[i], direct_bottom_n_coords);
				Cell* direct_bottom_n_cells[2];
				for (int idc = 0; idc < 2; ++idc) direct_bottom_n_cells[idc] = getCell(direct_bottom_n_coords[idc]);

				if (isAvailableForMigrating(direct_bottom_n_cells)) {
					empty_cells_coords.push_back(direct_n_coords[i]);
				} else if (_config["bridge-migration-up-down"]) {
					// миграция вниз
					for (int ibc = 0; ibc < 2; ++ibc) {
						if (direct_bottom_n_cells[ibc] || bottom_n_cells[ibc]->active() != 1) continue;
						Cell* bottom_bottom_n_cells[2];
						bottomNeighboursCells(direct_bottom_n_coords[ibc], bottom_bottom_n_cells);
						if (!isAvailableForMigrating(bottom_bottom_n_cells)) continue;
						if (!isCanDirectMigrating(current_cell, direct_bottom_n_coords[ibc])) continue;
						empty_cells_coords.push_back(direct_bottom_n_coords[ibc]);
					}
				}
			} else if (direct_n_cell->hydro() == 0 && direct_n_cell->active() == 1 &&
					_config["bridge-migration-up-down"])
			{
				// миграция вверх
//				Cell* other_direct_n_cell = getCell(direct_n_coords[1-i]);
//				if (other_direct_n_cell || (getCell(across_n_coords[0]) && getCell(across_n_coords[1]))) continue;

				int3 direct_direct_n_coords[2];
				directNeighboursCoords(direct_n_coords[i], direct_direct_n_coords);
				Cell* direct_direct_n_cells[2];
				int iddc;
				for (iddc = 0; iddc < 2; ++iddc) direct_direct_n_cells[iddc] = getCell(direct_direct_n_coords[iddc]);
				for (iddc = 0; iddc < 2; ++iddc) {
					if (current_cell == direct_direct_n_cells[1-iddc] &&
							isAvailableForMigrating(direct_n_cell, direct_direct_n_cells[iddc]))
					{
						int3 top_direct_n_coords;
						topNeighbourCoords(direct_n_coords[i], direct_direct_n_coords[iddc], top_direct_n_coords);
						if (!getCell(top_direct_n_coords)) empty_cells_coords.push_back(top_direct_n_coords);
					}
				}
			}
		}

		for (i = 0; i < 2; ++i) {
			Cell* across_n_cell = getCell(across_n_coords[i]);
			if (!across_n_cell) {
				int3 across_bottom_n_coords[2];
				bottomNeighboursCoords(across_n_coords[i], across_bottom_n_coords);
				Cell* across_bottom_n_cells[2];
				for (int iac = 0; iac < 2; ++iac) across_bottom_n_cells[iac] = getCell(across_bottom_n_coords[iac]);

				if (isAvailableForMigrating(across_bottom_n_cells)) {
					if (!isCanDirectMigrating(current_cell, across_n_coords[i])) continue;
					empty_cells_coords.push_back(across_n_coords[i]);
				} else if (_config["bridge-migration-up-down"]) {
					// миграция вниз
					for (int iabc = 0; iabc < 2; ++iabc) {
						if (across_bottom_n_cells[iabc] || across_bottom_n_cells[1-iabc]->hydro() != 0) continue;

						Cell* bottom_bottom_n_cells[2];
						bottomNeighboursCells(across_bottom_n_coords[iabc], bottom_bottom_n_cells);
						if (isAvailableForMigrating(bottom_bottom_n_cells)) {
							empty_cells_coords.push_back(across_bottom_n_coords[iabc]);
						}
					}
				}
			} else if (across_n_cell->hydro() == 0 && across_n_cell->active() == 1 &&
					_config["bridge-migration-up-down"])
			{
				// миграция вверх
//				Cell* other_across_n_cell = getCell(across_n_coords[1-i]);
//				if (other_across_n_cell || (getCell(direct_n_coords[0]) && getCell(direct_n_coords[1]))) continue;

				int3 direct_across_n_coords[2];
				directNeighboursCoords(across_n_coords[i], direct_across_n_coords);
				Cell* direct_across_n_cells[2];
				int idac;
				for (idac = 0; idac < 2; ++idac) direct_across_n_cells[idac] = getCell(direct_across_n_coords[idac]);
				for (idac = 0; idac < 2; ++idac) {
					if (!isAvailableForMigrating(across_n_cell, direct_across_n_cells[idac])) continue;
					int3 top_direct_across_n_coords;
					topNeighbourCoords(across_n_coords[i], direct_across_n_coords[idac], top_direct_across_n_coords);
					if (getCell(top_direct_across_n_coords)) continue;
					if (!isCanDirectMigrating(current_cell, top_direct_across_n_coords)) continue;
					empty_cells_coords.push_back(top_direct_across_n_coords);
				}
			}
		}

		if (empty_cells_coords.empty()) continue;

		// либо мигрирует, либо остаётся на месте
		unsigned int random_index = rand() % (empty_cells_coords.size() + 1);
		if (random_index == empty_cells_coords.size()) continue;

		++_migrated_bridges_num;

		VariantCoords::const_iterator rcit = empty_cells_coords.begin() + random_index;
		Cell* neighbour_bottom_n_cells[2];
		bottomNeighboursCells(*rcit, neighbour_bottom_n_cells);

		if (isDimer(neighbour_bottom_n_cells)) {
			deleteDimer(neighbour_bottom_n_cells[0], neighbour_bottom_n_cells[1]);
		} else {
			deactivate(neighbour_bottom_n_cells[0]);
			deactivate(neighbour_bottom_n_cells[1]);
		}

		activate(bottom_n_cells[0]);
		activate(bottom_n_cells[1]);

		current_cell->setCoords(*rcit);
		_cells[rcit->z][rcit->y][rcit->x] = current_cell;
		_cells[current_coords.z][current_coords.y][current_coords.x] = 0;
	}

//	delete actives_not_dimer;
	delete bridge_cells;
}

SetOfCells* Automata::unionCells(const SetOfCells& s1, const SetOfCells& s2) {
	SetOfCells* result = new SetOfCells;
	std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(*result, result->begin()));
	return result;
}

SetOfCells* Automata::differentCells(const SetOfCells& s1, const SetOfCells& s2) {
	SetOfCells* result = new SetOfCells;
	std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(*result, result->end()));
	return result;
}

bool Automata::isCanDirectMigrating(Cell* cell, const int3& to_coords) {
	if (cell->active() > 0) return true;

	int3 direct_n_coords[2];
	directNeighboursCoords(to_coords, direct_n_coords);
	Cell* direct_n_cells[2];
	for (int i = 0; i < 2; ++i) direct_n_cells[i] = getCell(direct_n_coords[i]);
	return (!direct_n_cells[0] || !direct_n_cells[1] ||
			direct_n_cells[0]->active() > 0 || direct_n_cells[1]->active() > 0);
}

void Automata::activate(Cell* cell) {
	cell->activate();
	if (cell->active() > 0) _actives.insert(cell);
}

void Automata::deactivate(Cell* cell) {
	cell->deactivate();
	if (cell->active() == 0) _actives.erase(cell);
}

void Automata::formDimerPart(Cell* cell) {
	_dimers.insert(cell);
	deactivate(cell);
}

void Automata::deleteDimer(Cell* cell1, Cell* cell2) {
	if (!_dimer_bonds.erase(cell1)) _dimer_bonds.erase(cell2);

	_dimers.erase(cell1);
	_dimers.erase(cell2);
}

void Automata::topNeighbourCoords(const int3& coords1, const int3& coords2, int3& top_neighbour_coords) {
	const int3* sc1 = (coords1 < coords2) ? &coords1 : &coords2;
	const int3* sc2 = (coords1 < coords2) ? &coords2 : &coords1;

	top_neighbour_coords.z = sc1->z + 1;
	if (sc1->z % 2 == 1) top_neighbour_coords.y = sc1->y;
	else top_neighbour_coords.x = sc1->x;

    if (sc1->z % 4 == 3) {
    	top_neighbour_coords.x = ((sc2->x - sc1->x) == 1) ? sc2->x : sc1->x;
    } else if (sc1->z % 4 == 2) {
    	top_neighbour_coords.y = ((sc2->y - sc1->y) == 1) ? sc2->y : sc1->y;
    } else if (sc1->z % 4 == 1) {
    	top_neighbour_coords.x = ((sc2->x - sc1->x) == 1) ? sc1->x : sc2->x;
    } else { // sc1->z % 4 == 0
    	top_neighbour_coords.y = ((sc2->y - sc1->y) == 1) ? sc1->y : sc2->y;
    }
}

void Automata::directNeighboursCoords(const int3& current_coords, int3 direct_neighbours_coords[2]) const {
	if (current_coords.z % 2 == 0) {
		int less_y, more_y;
		torusCoordinate('y', current_coords.y, less_y, more_y);

		direct_neighbours_coords[0].z = current_coords.z;
		direct_neighbours_coords[0].y = less_y;
		direct_neighbours_coords[0].x = current_coords.x;

		direct_neighbours_coords[1].z = current_coords.z;
		direct_neighbours_coords[1].y = more_y;
		direct_neighbours_coords[1].x = current_coords.x;
	} else {
		int less_x, more_x;
		torusCoordinate('x', current_coords.x, less_x, more_x);

		direct_neighbours_coords[0].z = current_coords.z;
		direct_neighbours_coords[0].y = current_coords.y;
		direct_neighbours_coords[0].x = less_x;

		direct_neighbours_coords[1].z = current_coords.z;
		direct_neighbours_coords[1].y = current_coords.y;
		direct_neighbours_coords[1].x = more_x;
	}

}

void Automata::flatNeighboursCoords(const int3& current_coords, int3 flat_neighbours_coords[2][2]) const {
	int less_x, more_x;
	torusCoordinate('x', current_coords.x, less_x, more_x);

	int less_y, more_y;
	torusCoordinate('y', current_coords.y, less_y, more_y);

	if (current_coords.z % 2 == 0) {
		flat_neighbours_coords[0][0].z = current_coords.z;
		flat_neighbours_coords[0][0].y = less_y;
		flat_neighbours_coords[0][0].x = current_coords.x;

		flat_neighbours_coords[0][1].z = current_coords.z;
		flat_neighbours_coords[0][1].y = more_y;
		flat_neighbours_coords[0][1].x = current_coords.x;

		flat_neighbours_coords[1][0].z = current_coords.z;
		flat_neighbours_coords[1][0].y = current_coords.y;
		flat_neighbours_coords[1][0].x = less_x;

		flat_neighbours_coords[1][1].z = current_coords.z;
		flat_neighbours_coords[1][1].y = current_coords.y;
		flat_neighbours_coords[1][1].x = more_x;
	} else {
		flat_neighbours_coords[0][0].z = current_coords.z;
		flat_neighbours_coords[0][0].y = current_coords.y;
		flat_neighbours_coords[0][0].x = less_x;

		flat_neighbours_coords[0][1].z = current_coords.z;
		flat_neighbours_coords[0][1].y = current_coords.y;
		flat_neighbours_coords[0][1].x = more_x;

		flat_neighbours_coords[1][0].z = current_coords.z;
		flat_neighbours_coords[1][0].y = less_y;
		flat_neighbours_coords[1][0].x = current_coords.x;

		flat_neighbours_coords[1][1].z = current_coords.z;
		flat_neighbours_coords[1][1].y = more_y;
		flat_neighbours_coords[1][1].x = current_coords.x;
	}
}

void Automata::bottomNeighboursCells(const int3& current_coords, Cell* bottom_neighbours_cells[2]) const {
	int3 bottom_n_coords[2];
	bottomNeighboursCoords(current_coords, bottom_n_coords);
	for (int i = 0; i < 2; ++i) bottom_neighbours_cells[i] = getCell(bottom_n_coords[i]);
}

void Automata::bottomNeighboursCoords(const int3& current_coords, int3 bottom_neighbours_coords[2]) const {
	int less_x, more_x;
	torusCoordinate('x', current_coords.x, less_x, more_x);

	int less_y, more_y;
	torusCoordinate('y', current_coords.y, less_y, more_y);

	bottom_neighbours_coords[0].z = current_coords.z - 1;
	bottom_neighbours_coords[1].z = current_coords.z - 1;
	if (current_coords.z % 4 == 3) {
		bottom_neighbours_coords[0].y = less_y;
		bottom_neighbours_coords[0].x = current_coords.x;

		bottom_neighbours_coords[1].y = current_coords.y;
		bottom_neighbours_coords[1].x = current_coords.x;
	} else if (current_coords.z % 4 == 2) {
		bottom_neighbours_coords[0].y = current_coords.y;
		bottom_neighbours_coords[0].x = current_coords.x;

		bottom_neighbours_coords[1].y = current_coords.y;
		bottom_neighbours_coords[1].x = more_x;
	} else if (current_coords.z % 4 == 1) {
		bottom_neighbours_coords[0].y = current_coords.y;
		bottom_neighbours_coords[0].x = current_coords.x;

		bottom_neighbours_coords[1].y = more_y;
		bottom_neighbours_coords[1].x = current_coords.x;
	} else { // current_coords.z % 4 == 0
		bottom_neighbours_coords[0].y = current_coords.y;
		bottom_neighbours_coords[0].x = less_x;

		bottom_neighbours_coords[1].y = current_coords.y;
		bottom_neighbours_coords[1].x = current_coords.x;
	}
}

void Automata::torusCoordinate(char coord, int current, int& less, int& more) const {
	int max;
	if (coord == 'x') max = _sizes.x - 1;
	else max = _sizes.y - 1;

	less = current - 1;
	if (less < 0) less = max;

	more = current + 1;
	if (more > max) more = 0;
}

}
