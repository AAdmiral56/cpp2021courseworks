#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"
#include "Functions.h"

using namespace std;

class DataStructure {      //now doing smth similar to what we did in Java
public:
	DataStructure(char* pFilename = nullptr) {

		if (pFilename == nullptr) { structure = nullptr; } //if the file is gone, so is the structure
		else {

			ifstream input;
			input.open(pFilename); //opening file

			string fID, lID, date; //gettign the file data 
			unsigned long int code;

			while (input >> fID >> lID >> code >> date) {
				fID = fID + ' ' + lID;

				ITEM4* item = new ITEM4();
				item->pID = _strdup((char*)fID.c_str());
				item->Code = code;
				item->pDate = _strdup((char*)date.c_str());

				this->operator+=(item);
			}

			input.close();
		}
	}

	~DataStructure() { delete_header_d(this->structure); }
//function call
	DataStructure(const DataStructure& Original) { copy_dataStructure(Original.structure); }

	int GetItemsNumber() {
		int n = 0;
		HEADER_D* headerVariable; // auxiliary variable
		for (headerVariable = this->structure; headerVariable; headerVariable = headerVariable->pNext) {

			HEADER_A* pp;
			for (pp = headerVariable->pHeaderA; pp; pp = pp->pNext) {

				ITEM4* i;
				for (i = (ITEM4*)(pp->pItems); i; i = i->pNext, n++);
			}
		}
		return n;
	}

	ITEM4* GetItem(char* pID) {
		if (pID == nullptr) { return NULL; }
		find_IDs(pID);
		for (HEADER_D* headerVariable = this->structure; headerVariable != nullptr && cBegin1 >= headerVariable->cBegin; headerVariable = headerVariable->pNext) //items are inserted as long as headerVariable is present in struct, is not null and cBegin1 is more than the original (cBegin1 is the cBegin that will be usd to insert variables on a loop)
			if (headerVariable->cBegin == cBegin1)
				for (HEADER_A* pp = headerVariable->pHeaderA; pp != nullptr && cBegin2 >= pp->cBegin; pp = pp->pNext)
					if (pp->cBegin == cBegin2)
						for (ITEM4* ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext)
							if (!strcmp(ppp->pID, pID)) return ppp;

		return NULL;
	}

	void operator+=(ITEM4* item) { add_header_d(item); }

	void operator-=(char* pID) { remove_item(pID); }

	DataStructure& operator=(const DataStructure& Right) { delete_header_d(this->structure); copy_dataStructure(Right.structure);}

	int operator==(DataStructure& Other) {

		if (this->structure != nullptr && Other.structure == nullptr)
			return 0;
		else if (this->structure == nullptr && Other.structure != nullptr)
			return 0;
		//loops through both headers and determines if the number of entities is equal or not
		for (HEADER_D* headerVariable = this->structure, *op = Other.structure;
			headerVariable != nullptr && op != nullptr;
			headerVariable = headerVariable->pNext, op = op->pNext) {
			if (headerVariable == nullptr || op == nullptr) return 0;

			for (HEADER_A* otheHeaderV = headerVariable->pHeaderA, *opp = op->pHeaderA;
				otheHeaderV || opp;
				otheHeaderV = otheHeaderV->pNext, opp = opp->pNext) {
				if (otheHeaderV == nullptr || opp == nullptr) return 0;

				for (ITEM4* i = (ITEM4*)otheHeaderV->pItems, *oi = (ITEM4*)opp->pItems;
					i || oi;
					i = i->pNext, oi = oi->pNext) {
					if (i == nullptr || oi == nullptr) return 0;
					if (strcmp(i->pID, oi->pID)) return 0;
				}
			}
		}

		return 1;
	}

	void Write(char* pFilename) {
		if (this->structure == nullptr) { cout << "Structure is missing data. Check the file provided"; }
		ofstream output;
		output.open(pFilename);
		for (HEADER_D* headerVariable = this->structure; headerVariable; headerVariable = headerVariable->pNext) {
			for (HEADER_A* otheHeaderV = headerVariable->pHeaderA; otheHeaderV; otheHeaderV = otheHeaderV->pNext) {
				for (ITEM4* i = (ITEM4*)(otheHeaderV->pItems); i; i = i->pNext) {
					output << i->pID << ' ' << i->Code << ' ' << i->pDate << endl;
					output.close();
				}
			}
		}
	}

	friend ostream& operator<<(ostream& ostr, const DataStructure& str) {

		int n = 1;
		for (HEADER_D* headerVariable = str.structure; headerVariable; headerVariable = headerVariable->pNext)
			for (HEADER_A* otheHeaderV = headerVariable->pHeaderA; otheHeaderV; otheHeaderV = otheHeaderV->pNext)
				for (ITEM4* i = (ITEM4*)(otheHeaderV->pItems); i; i = i->pNext, n++)
					ostr << n << ')' << i->pID << ' ' << i->Code << ' ' << i->pDate << '\n';

		return ostr;
	}

	HEADER_D* structure = nullptr;

private:
	char cBegin1 = ' ';
	char cBegin2 = ' ';
	void find_IDs(char* name) {
		cBegin1 = *name;
		char* pName = name;
		for (int i = 1; !isupper(*(pName + i)); i++) { cBegin2 = *(pName + i + 1); }
	}
	void delete_item(ITEM4* item) {
		if (item->pNext != nullptr)
			delete_item(item->pNext);

		delete item;
	}
	void delete_header_a(HEADER_A* otheHeaderV) {
		if (otheHeaderV->pNext != nullptr)
			delete_header_a(otheHeaderV->pNext);

		delete_item((ITEM4*)otheHeaderV->pItems);
		delete otheHeaderV;
	}
	void delete_header_d(HEADER_D* headerVariable) {
		if (headerVariable->pNext != nullptr)
			delete_header_d(headerVariable->pNext);

		delete_header_a(headerVariable->pHeaderA);
		delete headerVariable;
	}
	void copy_dataStructure(HEADER_D* Original) {
		this->structure = new headerD();
		HEADER_D* curPrev = nullptr;
		HEADER_D* curp = this->structure;
		for (HEADER_D* headerVariable = Original; headerVariable != nullptr; headerVariable = headerVariable->pNext) {
			curp->pPrior = curPrev;
			curPrev = curp;
			curp->cBegin = headerVariable->cBegin;
			curp->pHeaderA = new headerA();
			HEADER_A* curpp = curp->pHeaderA;
			if (headerVariable->pNext != nullptr) {
				HEADER_D* curpNext = new headerD();
				curp->pNext = curpNext;
				curp = curpNext;
			}

			for (HEADER_A* otheHeaderV = headerVariable->pHeaderA; otheHeaderV != nullptr; otheHeaderV = otheHeaderV->pNext) {
				curpp->cBegin = otheHeaderV->cBegin;
				curpp->pItems = new ITEM4();
				ITEM4* curppp = (ITEM4*)curpp->pItems;
				if (otheHeaderV->pNext != nullptr) {
					HEADER_A* curppNext = new headerA();
					curpp->pNext = curppNext;
					curpp = curppNext;
				}

				for (ITEM4* ppp = (ITEM4*)otheHeaderV->pItems; ppp != nullptr; ppp = ppp->pNext) {
					// todo strcpy
					curppp->pID = _strdup(ppp->pID);
					curppp->Code = ppp->Code;
					curppp->pDate = ppp->pDate;
					if (ppp->pNext != nullptr) {
						ITEM4* curpppNext = new ITEM4();
						curppp->pNext = curpppNext;
						curppp = curpppNext;
					}
				}
			}
		}
	}
	void add_header_d(ITEM4* item) {

		if (item == nullptr) { return; }

		find_IDs(item->pID);
		// exists
		for (HEADER_D* headerVariable = this->structure; headerVariable != nullptr && cBegin1 >= headerVariable->cBegin; headerVariable = headerVariable->pNext) {
			if (cBegin1 == headerVariable->cBegin) {
				add_header_a(headerVariable, item);
				return;
			}
		}
		// does not
		HEADER_D* hd = new headerD();
		hd->cBegin = cBegin1;
		if (this->structure == nullptr) { //first only
			hd->pNext = this->structure;
			this->structure = hd;
			add_header_a(hd, item);
			return;
		}
		else if (cBegin1 < this->structure->cBegin) { // just first
			hd->pNext = this->structure;
			this->structure->pPrior = hd;
			this->structure = hd;
			add_header_a(hd, item);
			return;
		}

		HEADER_D* headerVariable = this->structure;
		for (headerVariable; headerVariable != nullptr && cBegin1 > headerVariable->cBegin; headerVariable = headerVariable->pNext) { // if item in last
			if (headerVariable->pNext == nullptr) {
				hd->pPrior = headerVariable;
				headerVariable->pNext = hd;
				add_header_a(hd, item);
				return;
			}
		}
		hd->pPrior = headerVariable->pPrior;
		hd->pNext = headerVariable;
		headerVariable->pPrior->pNext = hd;
		headerVariable->pPrior = hd;
		add_header_a(hd, item);
	}
	void add_header_a(HEADER_D* headerVariable, ITEM4* item) {

		for (HEADER_A* otheHeaderV = headerVariable->pHeaderA; otheHeaderV != nullptr && cBegin2 >= otheHeaderV->cBegin; otheHeaderV = otheHeaderV->pNext) {
			if (cBegin2 == otheHeaderV->cBegin) {
				add_item(otheHeaderV, item);
				return;
			}
		}
		// does not
		HEADER_A* ha = new headerA(); //create new header
		ha->cBegin = cBegin2;
		if (headerVariable->pHeaderA == nullptr) { //first only
			ha->pNext = headerVariable->pHeaderA;
			headerVariable->pHeaderA = ha;
			add_item(ha, item);
			return;
		}
		else if (cBegin2 < headerVariable->pHeaderA->cBegin) { // just first
			HEADER_A* temp = headerVariable->pHeaderA;
			ha->pNext = temp;
			headerVariable->pHeaderA = ha;
			add_item(ha, item);
			return;
		}

		HEADER_A* otheHeaderV = headerVariable->pHeaderA;
		HEADER_A* prevpp = otheHeaderV;
		for (otheHeaderV; otheHeaderV != nullptr && cBegin2 > otheHeaderV->cBegin; otheHeaderV = otheHeaderV->pNext) { //add to end
			if (otheHeaderV->pNext == nullptr) {
				otheHeaderV->pNext = ha;
				add_item(ha, item);
				return;
			}
			prevpp = otheHeaderV;
		}
		// add to middle
		ha->pNext = otheHeaderV;
		prevpp->pNext = ha;
		add_item(ha, item);
	}
	void add_item(HEADER_A* otheHeaderV, ITEM4* item) {

		for (ITEM4* ppp = (ITEM4*)otheHeaderV->pItems; ppp != nullptr; ppp = ppp->pNext) {
			if (!strcmp(ppp->pID, item->pID)) {
				cout << "Item " << item->pID << " already exists!" << endl;
				return;
			}
		}

		ITEM4* ppp = (ITEM4*)otheHeaderV->pItems;
		for (ppp; ppp != nullptr; ppp = ppp->pNext) { // goes to the last item
			if (ppp->pNext == nullptr) { break; }
		}
		if (ppp == nullptr) { otheHeaderV->pItems = item; }
		else { ppp->pNext = item; }
	}
	bool validate_id(char* pID) { //error codes
		if (pID == nullptr) { 
			printf("parameters are empty\n");
			return false;
		}

		if (!isupper(*pID)) {
			printf("Identifier \"%s\" is in Wrong Format\n", pID);;
			return false;
		}
		char* letter;
		for (letter = pID + 1; isalpha(*letter) || '-' == *letter; letter++);
		if (!isspace(*letter)) {
			printf("Identifier \"%s\" is in Wrong Format\n", pID);;
			return false;
		}
		letter++;
		if (!isupper(*letter)) {
			printf("Identifier \"%s\" is in Wrong Format\n", pID);;
			return false;
		}

		for (letter++; isalpha(*letter) || '-' == *letter; letter++);
		if ('\0' != *letter) {
			printf("Identifier \"%s\" is in Wrong Format\n", pID);;
			return false;
		}

		return true;
	}
	void remove_item(char* pID) {

		if (!validate_id(pID)) { return; }

		find_IDs(pID);

		HEADER_D* headerVariable = this->structure;
		for (headerVariable; headerVariable != nullptr && headerVariable->cBegin <= cBegin1; headerVariable = headerVariable->pNext) {
			if (cBegin1 == headerVariable->cBegin) {
				HEADER_A* otheHeaderV, * prevpp = headerVariable->pHeaderA;
				for (otheHeaderV = headerVariable->pHeaderA; otheHeaderV != nullptr && otheHeaderV->cBegin <= cBegin2; otheHeaderV = otheHeaderV->pNext) {
					if (cBegin2 == otheHeaderV->cBegin) {
						ITEM4* ppp, * prevppp = (ITEM4*)otheHeaderV->pItems;
						for (ppp = (ITEM4*)otheHeaderV->pItems; ppp != nullptr; ppp = ppp->pNext) {
							if (!strcmp(ppp->pID, pID)) {

								ITEM4* comp = (ITEM4*)otheHeaderV->pItems; //first item to compare 
								if (ppp->pID == comp->pID && ppp->pNext == nullptr) { //only item
									remove_header_a(prevpp, headerVariable, cBegin2);
								}
								else if (ppp->pID == comp->pID) { //remove first
									otheHeaderV->pItems = ppp->pNext;
								}
								else if (ppp->pNext == nullptr) { //last 
									prevppp->pNext = nullptr;
								}
								else {
									prevppp->pNext = ppp->pNext;
								}
								delete ppp->pID;
								delete ppp;
								return;
							}

							prevppp = ppp;
						}
					}

					prevpp = otheHeaderV;
				}
			}
		}

		cout << "Item " << pID << " doesn't exist" << endl;
		return;
	}
	void remove_header_a(HEADER_A* prevpp, HEADER_D* headerVariable, char C) {
		if (prevpp == headerVariable->pHeaderA && prevpp->pNext == nullptr) {// removing HEADER_D entirely if there is no previous variable
			remove_header_d(headerVariable);
			delete prevpp;
		}
		else if (prevpp == headerVariable->pHeaderA && prevpp->cBegin == C) { // remove first line
			headerVariable->pHeaderA = prevpp->pNext;
			delete prevpp;
		}
		else {
			// remove anything other than those two conditions above
			HEADER_A* remove = prevpp->pNext;
			prevpp->pNext = remove->pNext;
			delete remove;
		}

	}
	void remove_header_d(HEADER_D* headerVariable) {
		if (headerVariable == this->structure) { // first line 
			this->structure = headerVariable->pNext;
			delete headerVariable;
		}
		else if (headerVariable->pNext == nullptr) { //last line
			headerVariable->pPrior->pNext = nullptr;
			delete headerVariable;
		}
		else {
			// middle
			headerVariable->pPrior->pNext = headerVariable->pNext;
			headerVariable->pNext->pPrior = headerVariable->pPrior;
			delete headerVariable;
		}
	}
};

int main()
{
	system("Color 1A");
	cout << "----------[Step 1-3]----------" << endl;
	DataStructure* printData = new DataStructure;
    for (int i = 0; i < 10; i++) { *printData += (ITEM4*)GetItem(4); }

	cout << *printData << endl << endl;

	cout << "----------[Step 4]----------" << endl;
	cout << "Number of items is: " << printData->GetItemsNumber() << endl << endl;

	cout << "----------[Step 5]----------" << endl;
	ITEM4* LC = printData->GetItem((char*)"Light Cyan");
	cout << LC->pID << ' ' << LC->Code << ' ' << LC->pDate << endl << endl;

	cout << "----------[Step 6]----------" << endl;
	ITEM4* XX = printData->GetItem((char*)"X X");
	if (XX == NULL) { cout << "Item doesn't exist" << endl << endl; }

	cout << "----------[Step 7]----------" << endl;
	DataStructure* copy = new DataStructure(*printData);
	cout << *copy << endl << endl;

	cout << "----------[Step 8]----------" << endl;
	*printData -= (char*)"Banana Mania";
	*printData -= (char*)"Persian Green";
	*printData -= (char*)"Vegas Gold";
	cout << *printData << endl << endl;

	cout << "----------[Step 9]----------" << endl;
	cout << (*copy == *printData) << endl;

	cout << "----------[Step 10-11]----------" << endl;
	printData->Write((char*)"file.txt");

	DataStructure* copy2 = new DataStructure((char*)"file.txt");
	cout << *copy2 << endl << endl;
	cout << (*copy2 == *printData) << endl << endl;

	cout << "----------[Step 12]----------" << endl;
	copy2 = copy;
	cout << *copy2 << endl << endl;

	return 0;
}