/*
 * handbook.h
 *
 *  Created on: 27.05.2011
 *      Author: newmen
 */

#ifndef HANDBOOK_H_
#define HANDBOOK_H_

#include <map>
#include <string>

#include "int3.h"

#define R 8.31

namespace DiamondCA {

class Handbook {
	typedef std::map<std::string, double> VarVal;

public:
	Handbook() { }
	virtual ~Handbook() { }

	void parseConfig(const std::string& config_file_name);

	int3 sizes() const { return _sizes; }
	void setSizes(const int3& sizes);
	double dt() const;

	double temperature() const { return _T; }
	double kMolecule(const std::string& key) const;

	double percentOfNotDimers() const;

private:
	double kMole(const std::string& key) const;

	static double percentOfLess(double more, double less);


private:
	std::map<std::string, VarVal> _params;
	int3 _sizes;

	double _T;
};

}

#endif /* HANDBOOK_H_ */
