#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "DateTime.h"
#include "Items.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"
#include <string>
#include <random>
#include <iterator>
#include <algorithm>
#include <functional>
#include "boost/pfr.hpp"
#include <io.h>
#include <iostream>

using namespace std;

void PrintDataStructure(HEADER_D*);
char id1(char*);
char id2(char*);
HEADER_D* InsertItem(HEADER_D*, char*);
HEADER_D* add_header_d(HEADER_D*, ITEM4*);
void add_header_a(HEADER_D*, ITEM4*);
void add_item(HEADER_A*, ITEM4*);
HEADER_D* RemoveItem(HEADER_D*, char*);
HEADER_D* remove_header_a(HEADER_A*, HEADER_D*, char, HEADER_D*);
HEADER_D* remove_header_d(HEADER_D*, HEADER_D*);

int main()
{
	HEADER_D* p = GetStruct4(4, 30);
	const char* testcase[13] = { "Z A", "Z Z", "Z K", "A Z", "A A", "A K", "G Z", "G A", "G K", "M A", "M Ba", "M Bb", "M Z" };

	cout << "Create Initial structure" << endl;
	PrintDataStructure(p);
	cout << endl;

	cout << "Add items" << endl;
	for (int i = 0; i < 13; i++) {
		p = InsertItem(p, (char*)testcase[i]);
	}

	PrintDataStructure(p);
	cout << endl;

	cout << "Remove items" << endl;
	for (int i = 0; i < 13; i++) {
		p = RemoveItem(p, (char*)testcase[i]);
	}
	PrintDataStructure(p);

	return 0;
}

char id1(char* name) {
	return *name;
}
char id2(char* name) {

	char* pName = name;
	char c = ' ';
	for (int i = 1; !isupper(*(pName + i)); i++) {
		c = *(pName + i + 1);
	}
	return c;
}

void PrintDataStructure(HEADER_D* structure) {

	int n = 1;
	for (HEADER_D* p = structure; p; p = p->pNext)
		for (HEADER_A* pp = p->pHeaderA; pp; pp = pp->pNext)
			for (ITEM4* i = (ITEM4*)(pp->pItems); i; i = i->pNext, n++)
				cout << n << ')' << i->pID << ' ' << i->Code << ' ' << i->pDate << endl;
}

HEADER_D* InsertItem(HEADER_D* pList, char* pID = 0) {

	if (pList == nullptr) {
		return pList;
	}

	ITEM4* item;

	if (pID == nullptr) {
		item = (ITEM4*)GetItem(4);
		return add_header_d(pList, item);
	}
	else if (!isupper(*pID) || !isupper(*(pID + 2)) || *(pID + 1) != ' ' || *(pID + 3) != '\0') {
		cout << "item " << pID << " is invalid!" << endl;
		return pList;
	}

	item = (ITEM4*)GetItem(4, pID);
	return add_header_d(pList, item);
}

HEADER_D* add_header_d(HEADER_D* pList, ITEM4* item) {

	if (item == nullptr) {
		return pList;
	}

	// exists
	for (HEADER_D* p = pList; p != nullptr && id1(item->pID) >= p->cBegin; p = p->pNext) {
		if (id1(item->pID) == p->cBegin) {
			add_header_a(p, item);
			return pList;
		}
	}
	// does not
	HEADER_D* hd = new headerD();
	hd->cBegin = id1(item->pID);
	if (pList == nullptr) { //first only
		hd->pNext = pList;
		pList = hd;
		add_header_a(hd, item);
		return pList;
	}
	else if (id1(item->pID) < pList->cBegin) { // just first
		hd->pNext = pList;
		pList->pPrior = hd;
		pList = hd;
		add_header_a(hd, item);
		return pList;
	}

	HEADER_D* p = pList;
	for (p; p != nullptr && id1(item->pID) > p->cBegin; p = p->pNext) { // if item in last
		if (p->pNext == nullptr) {
			hd->pPrior = p;
			p->pNext = hd;
			add_header_a(hd, item);
			return pList;
		}
	}
	// append to middle 
	hd->pPrior = p->pPrior;
	hd->pNext = p;
	p->pPrior->pNext = hd;
	p->pPrior = hd;
	add_header_a(hd, item);
	return pList;
}

void add_header_a(HEADER_D* p, ITEM4* item) {

	for (HEADER_A* pp = p->pHeaderA; pp != nullptr && id2(item->pID) >= pp->cBegin; pp = pp->pNext) {
		if (id2(item->pID) == pp->cBegin) {
			add_item(pp, item);
			return;
		}
	}
	// does not
	HEADER_A* ha = new headerA(); //create new header
	ha->cBegin = id2(item->pID);
	if (p->pHeaderA == nullptr) { //first only
		ha->pNext = p->pHeaderA;
		p->pHeaderA = ha;
		add_item(ha, item);
		return;
	}
	else if (id2(item->pID) < p->pHeaderA->cBegin) { // just first
		HEADER_A* temp = p->pHeaderA;
		ha->pNext = temp;
		p->pHeaderA = ha;
		add_item(ha, item);
		return;
	}

	HEADER_A* pp = p->pHeaderA;
	HEADER_A* prevpp = pp;
	for (pp; pp != nullptr && id2(item->pID) > pp->cBegin; pp = pp->pNext) { //add to end
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
		if (ppp->pNext == nullptr) {
			break;
		}
	}
	if (ppp == nullptr) {
		pp->pItems = item;
	}
	else {
		ppp->pNext = item;
	}
}

HEADER_D* RemoveItem(HEADER_D* pList, char* pID) {

	if (!isupper(*pID) || !isupper(*(pID + 2)) || *(pID + 1) != ' ' || *(pID + 3) != '\0') {
		return pList;
	}

	HEADER_D* p = pList;
	for (p; p != nullptr && p->cBegin <= id1(pID); p = p->pNext) {
		if (id1(pID) == p->cBegin) {
			HEADER_A* pp, * prevpp = p->pHeaderA;
			for (pp = p->pHeaderA; pp != nullptr && pp->cBegin <= id2(pID); pp = pp->pNext) {
				if (id2(pID) == pp->cBegin) {
					ITEM4* ppp, * prevppp = (ITEM4*)pp->pItems;
					for (ppp = (ITEM4*)pp->pItems; ppp != nullptr; ppp = ppp->pNext) {
						if (!strcmp(ppp->pID, pID)) {

							ITEM4* comp = (ITEM4*)pp->pItems; //first item to compare 
							if (ppp->pID == comp->pID && ppp->pNext == nullptr) { //only item
								return remove_header_a(prevpp, p, id2(pID), pList);
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
							delete ppp->pDate;
							delete ppp;
							return pList;
						}

						prevppp = ppp;
					}
				}

				prevpp = pp;
			}
		}
	}

	cout << "Item " << pID << " doesn't exist" << endl;
	return pList;
}

HEADER_D* remove_header_a(HEADER_A* prevpp, HEADER_D* p, char C, HEADER_D* pList) {
	if (prevpp == p->pHeaderA && prevpp->pNext == nullptr) {// remove header d if only
		delete prevpp;
		return remove_header_d(p, pList);
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

	return pList;
}

HEADER_D* remove_header_d(HEADER_D* p, HEADER_D* pList) {
	if (p == pList) { // first item 
		pList = p->pNext;
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

	return pList;
}