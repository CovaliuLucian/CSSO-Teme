#include "Tree.h"

Tree::Tree(): process(nullptr), parent(nullptr)
{
	children = std::list<Tree*>();
}

Tree::Tree(Process* process, Tree* parent): process(process), parent(parent)
{
	children = std::list<Tree*>();
}
