// Parth Shah
// Computing Systems Architecture 
// Dec 16, 2017
// Lab 2

#include<iostream>
#include<cstring>
#include<vector>
#include<bitset>
#include<fstream>
#include<sstream>
#include<map>
#include<math.h>
#include<algorithm>
#include"Branch.h"

using namespace std;

int main() {
	cout << "Working..." << endl;

	/* Open config and text files */
	ifstream myReadFile;

	/*Map of branches*/
	map <unsigned int, Branch> branchMap;

	int m = 0;
	int k = 0;
	string address;
	int state;

	myReadFile.open("config.txt");
	if (myReadFile.fail()) {
		cerr << "Error opening file" << endl;
		exit(1);
	}
	myReadFile >> m >> k;
	myReadFile.close();

	myReadFile.open("trace.txt");

	if (myReadFile.fail()) {
		cerr << "Error opening file" << endl;
		exit(1);
	}

	ofstream outputFile("trace.out.txt");
	vector<unsigned int> BHR(pow(2, k), 1); // Intitialize BHR to all Taken

	while (myReadFile >> address >> state) {

		Branch currentBranch(k);

		// Reconstruct branch address
		string sReturn = "";
		for (int i = 0; i < address.length(); ++i)
		{
			switch (address[i])
			{
				case '0': sReturn.append("0000"); break;
				case '1': sReturn.append("0001"); break;
				case '2': sReturn.append("0010"); break;
				case '3': sReturn.append("0011"); break;
				case '4': sReturn.append("0100"); break;
				case '5': sReturn.append("0101"); break;
				case '6': sReturn.append("0110"); break;
				case '7': sReturn.append("0111"); break;
				case '8': sReturn.append("1000"); break;
				case '9': sReturn.append("1001"); break;
				case 'a': sReturn.append("1010"); break;
				case 'b': sReturn.append("1011"); break;
				case 'c': sReturn.append("1100"); break;
				case 'd': sReturn.append("1101"); break;
				case 'e': sReturn.append("1110"); break;
				case 'f': sReturn.append("1111"); break;
			}
		}
		sReturn.substr(32 - m, m);
		bitset<32> binaryBranch(sReturn);
		unsigned int row = binaryBranch.to_ulong(); //Key to our PHT

		if (branchMap.find(row) == branchMap.end())
		{
			currentBranch.columns.resize(pow(2, m+k), 3); //If this branch has not been 'hit' yet.
			branchMap[row] = currentBranch;
		}

		else
		{
			currentBranch = branchMap[row]; //If it does exist (we have 'hit' it) then just set equal to existing branch.
		}

		// Setup BHR
		string BHRstring;

		for (int i = 0; i < k; i++)
		{
			BHRstring += to_string(BHR[i]);
		}

		bitset<32> binaryBHR(BHRstring);

		// Multiply the bits by the youngest BHR value
		unsigned int column = binaryBHR.to_ulong();

		int PHT = currentBranch.columns[row*column]; //Index into the column at the current branch to see it's history.

		if (PHT == 3)
		{
			if (state == 0)
			{
				PHT = 2;
			}
			else
			{
				PHT = 3;
			}
		}

		else if (PHT == 2)
		{
			if (state == 0)
			{
				PHT = 1;
			}
			else
			{
				PHT = 3;
			}
		}

		else if (PHT == 1)
		{
			if (state == 0)
			{
				PHT = 0;
			}
			else
			{
				PHT = 2;
			}
		}

		else
		{
			if (state == 0)
			{
				PHT = 0;
			}
			else
			{
				PHT = 1;
			}
		}
		currentBranch.columns[column] = PHT;
		int newState; // Translate confidence (Strong/weak) to a definite state (T/NT)

		switch (PHT)
		{
		case 0:
			newState = 0;
			break;
		case 1:
			newState = 0;
			break;
		case 2:
			newState = 1;
			break;
		case 3:
			newState = 1;
			break;
		}
		
		// Update BHR with actual result, push out oldest state
		// Example: k = 3 and BHR = 101 {1, 0, 1}

		for (int d = 0; d < BHR.size() - 1; d++)
		{
			BHR[d + 1] = BHR[d]; //Shifting our BHR over by 1.
		}

		if (state == 1) // BHR = 110  {1 or 0, 1, 0)
		{
			BHR[0] = 1;
		}
		else // BHR = 010
		{
			BHR[0] = 0;
		}

		outputFile << address << " " << newState << endl;
	}

	myReadFile.close();

	cout << m << endl;
	cout << k << endl;

	cout << "Done";
	cin.get();
	return 0;
}