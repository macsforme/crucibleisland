// DrawTypes.h
// Dominicus

#ifndef DRAWTYPES_H
#define DRAWTYPES_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "math/VectorMath.h"
#include "platform/OpenGLHeaders.h"

typedef std::map<std::string, void*> DrawStackArgList;
typedef std::pair< std::string, DrawStackArgList > DrawStackEntry;
typedef std::vector<DrawStackEntry> DrawStack;

class BaseDrawNode {
protected:
	std::map<std::string, GLuint> vertexBuffers;

public:
	virtual void execute(DrawStackArgList arguments) = 0;
};

class BaseUIElement : public BaseDrawNode {
public:
	virtual DrawStackArgList instantiateArgList() = 0;
	virtual void deleteArgList(DrawStackArgList argList) = 0;

	// this is for metrics; all drawers should return it whether or not they take a "size" argument which may be the same value
	virtual Vector2 getSize(DrawStackArgList argList) = 0;
};

#endif // DRAWTYPES_H
