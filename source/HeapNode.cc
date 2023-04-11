#include <iostream>
#include "HeapNode.h"
#include <cmath>

using namespace std;
using namespace GraphModule;

int HeapNode::getParent() 
{
    return floor((key - 1) / 2);
}

int HeapNode::getLeft() 
{
    return 2 * key + 1;
}

int HeapNode::getRight() 
{
    return 2 * key + 2;
}

bool HeapNode::operator >(const HeapNode& other) 
{
    return priority > other.priority;
}

bool HeapNode::operator >(int x) 
{
    return priority > x;
}

bool HeapNode::operator <(const HeapNode& other) 
{
    return priority < other.priority;
}

bool HeapNode::operator <(int x) 
{
    return priority < x;
}

bool HeapNode::operator <=(const HeapNode& other) 
{
    return priority <= other.priority;
}

bool HeapNode::operator <=(int x) 
{
    return priority <= x;
}

bool HeapNode::operator >=(const HeapNode& other) 
{
    return priority >= other.priority;
}

bool HeapNode::operator >=(int x) 
{
    return priority >= x;
}

bool HeapNode::operator ==(const HeapNode& other) 
{
    return priority == other.priority;
}

bool HeapNode::operator !=(const HeapNode& other) 
{
    return priority != other.priority;
}
