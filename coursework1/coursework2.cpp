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

class DataStructure {
public:
	DataStructure(char* pFilename = nullptr) {

		if (pFilename == nullptr) { structure = nullptr; }
		else {

			ifstream input;
			input.open(pFilename);

			string fID, lID, date;
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

	DataStructure(const DataStructure& Original) { copy_dataStructure(Original.structure); }

	int GetItemsNumber() {
		int n = 0;
		HEADER_D* p; // auxiliary variable
		for (p = this->structure; p; p = p->pNext) {

			HEADER_A* pp;
			for (pp = p->pHeaderA; pp; pp = pp->pNext) {

				ITEM4* i;
				for (i = (ITEM4*)(pp->pItems); i; i = i->pNext, n++);
			}
		}
		return n;
	}

	ITEM4* GetItem(char* pID) {
		if (pID == nullptr) { return NULL; }
		find_IDs(pID);
		for (HEADER_D* p = this->structure; p != nullptr && cBegin1 >= p->cBegin; p = p->pNext)
			if (p->cBegin == cBegin1)
				for (HEADER_A* pp = p->pHeaderA; pp != nullptr && cBegin2 >= pp->cBegin; pp = pp->pNext)
					if (pp->cBegin == cBegin2)
						for (ITEM4* ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext)
							if (!strcmp(ppp->pID, pID)) return ppp;

		return NULL;
	}

	void operator+=(ITEM4* item) { add_header_d(item); }

	void operator-=(char* pID) { remove_item(pID); }

	DataStructure& operator=(const DataStructure& Right) {
		delete_header_d(this->structure);
		copy_dataStructure(Right.structure);
	}

	int operator==(DataStructure& Other) {

		if (this->structure != nullptr && Other.structure == nullptr)
			return 0;
		else if (this->structure == nullptr && Other.structure != nullptr)
			return 0;
		//goes though double loop for header D then A and then item if number of entities is different they are not equal
		for (HEADER_D* p = this->structure, *op = Other.structure;
			p != nullptr && op != nullptr;
			p = p->pNext, op = op->pNext) {
			if (p == nullptr || op == nullptr) return 0;

			for (HEADER_A* pp = p->pHeaderA, *opp = op->pHeaderA;
				pp || opp;
				pp = pp->pNext, opp = opp->pNext) {
				if (pp == nullptr || opp == nullptr) return 0;

				for (ITEM4* i = (ITEM4*)pp->pItems, *oi = (ITEM4*)opp->pItems;
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
		if (this->structure == nullptr) { cout << "structure empty"; }
		ofstream output;
		output.open(pFilename);
		for (HEADER_D* p = this->structure; p; p = p->pNext)
			for (HEADER_A* pp = p->pHeaderA; pp; pp = pp->pNext)
				for (ITEM4* i = (ITEM4*)(pp->pItems); i; i = i->pNext)
					output << i->pID << ' ' << i->Code << ' ' << i->pDate << endl;
		output.close();
	}

	friend ostream& operator<<(ostream& ostr, const DataStructure& str) {

		int n = 1;
		for (HEADER_D* p = str.structure; p; p = p->pNext)
			for (HEADER_A* pp = p->pHeaderA; pp; pp = pp->pNext)
				for (ITEM4* i = (ITEM4*)(pp->pItems); i; i = i->pNext, n++)
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
	void delete_header_a(HEADER_A* pp) {
		if (pp->pNext != nullptr)
			delete_header_a(pp->pNext);

		delete_item((ITEM4*)pp->pItems);
		delete pp;
	}
	void delete_header_d(HEADER_D* p) {
		if (p->pNext != nullptr)
			delete_header_d(p->pNext);

		delete_header_a(p->pHeaderA);
		delete p;
	}
	void copy_dataStructure(HEADER_D* Original) {
		this->structure = new headerD();
		HEADER_D* curPrev = nullptr;
		HEADER_D* curp = this->structure;
		for (HEADER_D* p = Original; p != nullptr; p = p->pNext) {
			curp->pPrior = curPrev;
			curPrev = curp;
			curp->cBegin = p->cBegin;
			curp->pHeaderA = new headerA();
			HEADER_A* curpp = curp->pHeaderA;
			if (p->pNext != nullptr) {
				HEADER_D* curpNext = new headerD();
				curp->pNext = curpNext;
				curp = curpNext;
			}

			for (HEADER_A* pp = p->pHeaderA; pp != nullptr; pp = pp->pNext) {
				curpp->cBegin = pp->cBegin;
				curpp->pItems = new ITEM4();
				ITEM4* curppp = (ITEM4*)curpp->pItems;
				if (pp->pNext != nullptr) {
					HEADER_A* curppNext = new headerA();
					curpp->pNext = curppNext;
					curpp = curppNext;
				}

				for (ITEM4* ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext) {
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
		for (HEADER_D* p = this->structure; p != nullptr && cBegin1 >= p->cBegin; p = p->pNext) {
			if (cBegin1 == p->cBegin) {
				add_header_a(p, item);
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

		HEADER_D* p = this->structure;
		for (p; p != nullptr && cBegin1 > p->cBegin; p = p->pNext) { // if item in last
			if (p->pNext == nullptr) {
				hd->pPrior = p;
				p->pNext = hd;
				add_header_a(hd, item);
				return;
			}
		}
		// append to middle 
		hd->pPrior = p->pPrior;
		hd->pNext = p;
		p->pPrior->pNext = hd;
		p->pPrior = hd;
		add_header_a(hd, item);
	}
	void add_header_a(HEADER_D* p, ITEM4* item) {

		for (HEADER_A* pp = p->pHeaderA; pp != nullptr && cBegin2 >= pp->cBegin; pp = pp->pNext) {
			if (cBegin2 == pp->cBegin) {
				add_item(pp, item);
				return;
			}
		}
		// does not
		HEADER_A* ha = new headerA(); //create new header
		ha->cBegin = cBegin2;
		if (p->pHeaderA == nullptr) { //first only
			ha->pNext = p->pHeaderA;
			p->pHeaderA = ha;
			add_item(ha, item);
			return;
		}
		else if (cBegin2 < p->pHeaderA->cBegin) { // just first
			HEADER_A* temp = p->pHeaderA;
			ha->pNext = temp;
			p->pHeaderA = ha;
			add_item(ha, item);
			return;
		}

		HEADER_A* pp = p->pHeaderA;
		HEADER_A* prevpp = pp;
		for (pp; pp != nullptr && cBegin2 > pp->cBegin; pp = pp->pNext) { //add to end
			if (pp->pNext == nullptr) {
				pp->pNext = ha;
				add_item(ha, item);
				return;
			}
			prevpp = pp;
		}
		// add to middle
		ha->pNext = pp;
		prevpp->pNext = ha;
		add_item(ha, item);
	}
	void add_item(HEADER_A* pp, ITEM4* item) {

		for (ITEM4* ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext) {
			if (!strcmp(ppp->pID, item->pID)) {
				cout << "Item " << item->pID << " already exists!" << endl;
				return;
			}
		}

		ITEM4* ppp = (ITEM4*)pp->pItems;
		for (ppp; ppp != nullptr; ppp = ppp->pNext) { // goes to the last item
			if (ppp->pNext == nullptr) { break; }
		}
		if (ppp == nullptr) { pp->pItems = item; }
		else { ppp->pNext = item; }
	}
	bool validate_id(char* pID) {
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

		HEADER_D* p = this->structure;
		for (p; p != nullptr && p->cBegin <= cBegin1; p = p->pNext) {
			if (cBegin1 == p->cBegin) {
				HEADER_A* pp, * prevpp = p->pHeaderA;
				for (pp = p->pHeaderA; pp != nullptr && pp->cBegin <= cBegin2; pp = pp->pNext) {
					if (cBegin2 == pp->cBegin) {
						ITEM4* ppp, * prevppp = (ITEM4*)pp->pItems;
						for (ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext) {
							if (!strcmp(ppp->pID, pID)) {

								ITEM4* comp = (ITEM4*)pp->pItems; //first item to compare 
								if (ppp->pID == comp->pID && ppp->pNext == nullptr) { //only item
									remove_header_a(prevpp, p, cBegin2);
								}
								else if (ppp->pID == comp->pID) { //remove first
									pp->pItems = ppp->pNext;
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

					prevpp = pp;
				}
			}
		}

		cout << "Item " << pID << " doesn't exist" << endl;
		return;
	}
	void remove_header_a(HEADER_A* prevpp, HEADER_D* p, char C) {
		if (prevpp == p->pHeaderA && prevpp->pNext == nullptr) {// remove header d if only
			remove_header_d(p);
			delete prevpp;
		}
		else if (prevpp == p->pHeaderA && prevpp->cBegin == C) { // remove first
			p->pHeaderA = prevpp->pNext;
			delete prevpp;
		}
		else {
			// remove any other item 
			HEADER_A* remove = prevpp->pNext;
			prevpp->pNext = remove->pNext;
			delete remove;
		}

	}
	void remove_header_d(HEADER_D* p) {
		if (p == this->structure) { // first item 
			this->structure = p->pNext;
			delete p;
		}
		else if (p->pNext == nullptr) { //last item
			p->pPrior->pNext = nullptr;
			delete p;
		}
		else {
			// 
			p->pPrior->pNext = p->pNext;
			p->pNext->pPrior = p->pPrior;
			delete p;
		}
	}
};

int main()
{
	cout << "\033[1;31m[Step 1-3]\033[0m" << endl;
	DataStructure* pds = new DataStructure;

	for (int i = 0; i < 10; i++) { *pds += (ITEM4*)GetItem(4); }

	cout << *pds << endl << endl;

	cout << "\033[1;31m[Step 4]\033[0m" << endl;
	cout << "Number of items is: " << pds->GetItemsNumber() << endl << endl;

	cout << "\033[1;31m[Step 5]\033[0m" << endl;
	ITEM4* LC = pds->GetItem((char*)"Light Cyan");
	cout << LC->pID << ' ' << LC->Code << ' ' << LC->pDate << endl << endl;

	cout << "\033[1;31m[Step 6]\033[0m" << endl;
	ITEM4* XX = pds->GetItem((char*)"X X");
	if (XX == NULL) { cout << "Item doesn't exist" << endl << endl; }

	cout << "\033[1;31m[Step 7]\033[0m" << endl;
	DataStructure* copy = new DataStructure(*pds);
	cout << *copy << endl << endl;

	cout << "\033[1;31m[Step 8]\033[0m" << endl;
	*pds -= (char*)"Banana Mania";
	*pds -= (char*)"Persian Green";
	*pds -= (char*)"Vegas Gold";
	cout << *pds << endl << endl;

	cout << "\033[1;31m[Step 9]\033[0m" << endl;
	cout << (*copy == *pds) << endl;

	cout << "\033[1;31m[Step 10-11]\033[0m" << endl;
	pds->Write((char*)"file.txt");

	DataStructure* copy2 = new DataStructure((char*)"file.txt");
	cout << *copy2 << endl << endl;
	cout << (*copy2 == *pds) << endl << endl;

	cout << "\033[1;31m[Step 12]\033[0m" << endl;
	copy2 = copy;
	cout << *copy2 << endl << endl;

	return 0;
}