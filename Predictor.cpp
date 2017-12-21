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
#include <iomanip>
#include"Branch.h"

using namespace std;

int main() {
	cout << "Working..." << endl;

	/* Open config and text files */
	ifstream myReadFile;

	/*Map of branches*/
	map <unsigned int, Branch*> branchMap; // Ties int to whatever columns we give to that integer
	// Example: be202002 -> {3, 3, 3, 3} when k = 2

	int m = 0;
	int k = 0;
	string address;
	string BHRstring;
	int state;
	float mispredict = 0;
	float numberOfBranches = 0;

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
	ofstream compareFile("compare.txt");
	
	vector<unsigned int> BHR(k, 1); // Intitialize BHR to size k and set to all Taken

	while (myReadFile >> address >> state) {
		Branch* currentBranch = new Branch();

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
		sReturn = sReturn.substr(32 - m, m);
		bitset<32> binaryBranch(sReturn);
		unsigned int row = binaryBranch.to_ulong(); //Key to our PHT

		// Adjust resolution of branch to m LSB
		while (row > (pow(2, m) - 1))
		{
			row = row - pow(2, m);
		}

		if (branchMap.find(row) == branchMap.end())
		{
			currentBranch->columns.resize(pow(2, k), 3); // Number of 2^k columns for each branch
			branchMap[row] = currentBranch;
		}

		else
		{
			Branch mapPointer = {}; //If row does exist (we have 'hit' it) then just set equal to existing branch.
			currentBranch = branchMap[row];
		}

		// Setup BHR
		for (int i = 0; i < k; i++)
		{
			BHRstring += to_string(BHR[i]);
		}

		bitset<32> binaryBHR(BHRstring);

		unsigned int column = binaryBHR.to_ulong(); // Column ranges from 1 to 2^k

		int PHT = currentBranch->columns[column]; //Index into the column at the current branch to see it's history.
		// Example: m = 3, k = 2, Address 09, PHT = {3, 3, 3, 3}
		// Example; m = 3, k = 0, Address 09, PHT = {3}

		if (PHT == 3) // PHT = Previous state in history table
		{
			if (state == 0) // State = Actual state of branch
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
		
		currentBranch->columns[column] = PHT; // UPDATE INDEX based on actual state
											  
		// Example: k = 3 and BHR = 101 {1, 0, 1}
		for (int d = 0; d < BHR.size() - 1; d++)
		{
			BHR[d + 1] = BHR[d]; // Update BHR with actual result, push out oldest state
		}

		if (state == 1) // BHR = 110  {1 or 0, 1, 0)
		{
			BHR[0] = 1;
		}
		else // BHR = 010
		{
			BHR[0] = 0;
		}

		int predictState; // Translate confidence (Strong/weak) to a definite state (T/NT)

		switch (PHT)
		{
		case 0:
			predictState = 0;
			break;
		case 1:
			predictState = 0;
			break;
		case 2:
			predictState = 1;
			break;
		case 3:
			predictState = 1;
			break;
		}

		// Comparison between Actual state and Predicted state
		if (predictState != state)
		{
			mispredict = mispredict + 1;
		}

		numberOfBranches = numberOfBranches + 1;
		
		outputFile << address << " " << predictState << endl;
	}
	
	float mispredictRate = (mispredict / numberOfBranches) * 100;
	
	myReadFile.close();

	cout << "m: " << m << endl;
	cout << "k: " << k << endl;
	cout << "Num of mispredicts: " << mispredict << endl;
	cout << setprecision(5) << mispredictRate << endl;

	cout << "Done";
	cin.get();
	return 0;
}