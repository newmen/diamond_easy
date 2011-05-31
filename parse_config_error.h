/*
 * parse_config_error.h
 *
 *  Created on: 28.05.2011
 *      Author: newmen
 */

#ifndef PARSECONFIGERROR_H_
#define PARSECONFIGERROR_H_

#include <string>
#include <sstream>

#include "parse_error.h"

namespace DiamondCA {

class ParseConfigError : public ParseError {
public:
	ParseConfigError(const std::string& message) : ParseError(message), _line("") { }
	ParseConfigError(const std::string& message, const std::string& line) : ParseError(message), _line(line) { }

	std::string getMessage() const {
		std::stringstream message;
		message << ParseError::getMessage();
		if (_line != "") message << " (" << _line << ")" << std::endl;
		return message.str();
	}

private:
	ParseConfigError() { }

private:
	std::string _line;
};

}

#endif /* PARSECONFIGERROR_H_ */
