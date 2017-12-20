#pragma once

#include<vector>
using namespace std;

class Branch
{
public:
	unsigned int listSize;

	Branch(int curK);

	vector<int> columns;

	Branch();
};

