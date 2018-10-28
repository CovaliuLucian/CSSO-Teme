#pragma once
#include "Process.h"
#include <list>

class Tree
{
public:
	Process* process;
	Tree* parent;
	std::list<Tree*> children;

	Tree();
	Tree(Process* process, Tree* parent);
};
