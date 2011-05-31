/*
 * parse_error.h
 *
 *  Created on: 30.05.2011
 *      Author: newmen
 */

#ifndef PARSE_ERROR_H_
#define PARSE_ERROR_H_

#include <string>

namespace DiamondCA {

class ParseError {
public:
	ParseError(const std::string& message) : _message(message) { }
	virtual ~ParseError() { }

	virtual std::string getMessage() const { return _message; }

protected:
	ParseError() { }

private:
	std::string _message;
};

}

#endif /* PARSE_ERROR_H_ */
