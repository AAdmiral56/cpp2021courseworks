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
#include "Functions.h"

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
	HEADER_D* pStructInfo = GetStruct4(4, 30);
	const char* testcase[13] = { "Z A", "Z Z", "Z K", "A Z", "A A", "A K", "G Z", "G A", "G K", "M A", "M Ba", "M Bb", "M Z" };

	cout << "Create Initial structure" << endl;
	PrintDataStructure(pStructInfo);
	cout << endl;

	cout << "Add items" << endl;
	for (int i = 0; i < 13; i++) {
		pStructInfo = InsertItem(pStructInfo, (char*)testcase[i]);
	}

	PrintDataStructure(pStructInfo);
	cout << endl;

	cout << "Remove items" << endl;
	for (int i = 0; i < 13; i++) {
		pStructInfo = RemoveItem(pStructInfo, (char*)testcase[i]);
	}
	PrintDataStructure(pStructInfo);

	return 0;
}

char id1(char* name) { return *name; }
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
	for (HEADER_D* pStructInfo = structure; pStructInfo; pStructInfo = pStructInfo->pNext) 
	{
	for (HEADER_A* ppHeaderStuff = pStructInfo->pHeaderA; ppHeaderStuff; ppHeaderStuff = ppHeaderStuff->pNext) 
	    {
	for (ITEM4* i = (ITEM4*)(ppHeaderStuff->pItems); i; i = i->pNext, n++) 
	        {
	cout << n << ')' << i->pID << ' ' << i->Code << ' ' << i->pDate << endl;
			}
		}
	}
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

	//if the item exists
	for (HEADER_D* pStructInfo = pList; pStructInfo != nullptr && id1(item->pID) >= pStructInfo->cBegin; pStructInfo = pStructInfo->pNext) {
		if (id1(item->pID) == pStructInfo->cBegin) {
			add_header_a(pStructInfo, item);
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

	HEADER_D* pStructInfo = pList;
	for (pStructInfo; pStructInfo != nullptr && id1(item->pID) > pStructInfo->cBegin; pStructInfo = pStructInfo->pNext) { // if item in last
		if (pStructInfo->pNext == nullptr) {
			hd->pPrior = pStructInfo;
			pStructInfo->pNext = hd;
			add_header_a(hd, item);
			return pList;
		}
	}
	// append to middle 
	hd->pPrior = pStructInfo->pPrior;
	hd->pNext = pStructInfo;
	pStructInfo->pPrior->pNext = hd;
	pStructInfo->pPrior = hd;
	add_header_a(hd, item);
	return pList;
}

void add_header_a(HEADER_D* pStructInfo, ITEM4* item) {

	for (HEADER_A* ppHeaderStuff = pStructInfo->pHeaderA; ppHeaderStuff != nullptr && id2(item->pID) >= ppHeaderStuff->cBegin; ppHeaderStuff = ppHeaderStuff->pNext) {
		if (id2(item->pID) == ppHeaderStuff->cBegin) {
			add_item(ppHeaderStuff, item);
			return;
		}
	}
	// does not
	HEADER_A* ha = new headerA(); //create new header
	ha->cBegin = id2(item->pID);
	if (pStructInfo->pHeaderA == nullptr) { //first only
		ha->pNext = pStructInfo->pHeaderA;
		pStructInfo->pHeaderA = ha;
		add_item(ha, item);
		return;
	}
	else if (id2(item->pID) < pStructInfo->pHeaderA->cBegin) { // just first
		HEADER_A* temp = pStructInfo->pHeaderA;
		ha->pNext = temp;
		pStructInfo->pHeaderA = ha;
		add_item(ha, item);
		return;
	}

	HEADER_A* ppHeaderStuff = pStructInfo->pHeaderA;
	HEADER_A* prevpp = ppHeaderStuff;
	for (ppHeaderStuff; ppHeaderStuff != nullptr && id2(item->pID) > ppHeaderStuff->cBegin; ppHeaderStuff = ppHeaderStuff->pNext) { //add to end
		if (ppHeaderStuff->pNext == nullptr) {
			ppHeaderStuff->pNext = ha;
			add_item(ha, item);
			return;
		}
		prevpp = ppHeaderStuff;
	}
	// add to middle
	ha->pNext = ppHeaderStuff;
	prevpp->pNext = ha;
	add_item(ha, item);
}

void add_item(HEADER_A* ppHeaderStuff, ITEM4* item) {

	for (ITEM4* ppp = (ITEM4*)ppHeaderStuff->pItems; ppp != nullptr; ppp = ppp->pNext) {
		if (!strcmp(ppp->pID, item->pID)) {
			cout << "Item " << item->pID << " already exists!" << endl;
			return;
		}
	}

	ITEM4* ppp = (ITEM4*)ppHeaderStuff->pItems;
	for (ppp; ppp != nullptr; ppp = ppp->pNext) { // goes to the last item
		if (ppp->pNext == nullptr) {
			break;
		}
	}
	if (ppp == nullptr) {
		ppHeaderStuff->pItems = item;
	}
	else {
		ppp->pNext = item;
	}
}

HEADER_D* RemoveItem(HEADER_D* pList, char* pID) {

	if (!isupper(*pID) || !isupper(*(pID + 2)) || *(pID + 1) != ' ' || *(pID + 3) != '\0') {
		return pList;
	}

	HEADER_D* pStructInfo = pList;
	for (pStructInfo; pStructInfo != nullptr && pStructInfo->cBegin <= id1(pID); pStructInfo = pStructInfo->pNext) {
		if (id1(pID) == pStructInfo->cBegin) {
			HEADER_A* ppHeaderStuff, * prevpp = pStructInfo->pHeaderA;
			for (ppHeaderStuff = pStructInfo->pHeaderA; ppHeaderStuff != nullptr && ppHeaderStuff->cBegin <= id2(pID); ppHeaderStuff = ppHeaderStuff->pNext) {
				if (id2(pID) == ppHeaderStuff->cBegin) {
					ITEM4* ppp, * prevppp = (ITEM4*)ppHeaderStuff->pItems;
					for (ppp = (ITEM4*)ppHeaderStuff->pItems; ppp != nullptr; ppp = ppp->pNext) {
						if (!strcmp(ppp->pID, pID)) {

							ITEM4* comp = (ITEM4*)ppHeaderStuff->pItems; //first item to compare 
							if (ppp->pID == comp->pID && ppp->pNext == nullptr) { //only item
								return remove_header_a(prevpp, pStructInfo, id2(pID), pList);
							}
							else if (ppp->pID == comp->pID) { //remove first
								ppHeaderStuff->pItems = ppp->pNext;
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

				prevpp = ppHeaderStuff;
			}
		}
	}

	cout << "Item " << pID << " doesn't exist" << endl;
	return pList;
}

HEADER_D* remove_header_a(HEADER_A* prevpp, HEADER_D* pStructInfo, char C, HEADER_D* pList) {
	if (prevpp == pStructInfo->pHeaderA && prevpp->pNext == nullptr) {// remove header d if only
		delete prevpp;
		return remove_header_d(pStructInfo, pList);
	}
	else if (prevpp == pStructInfo->pHeaderA && prevpp->cBegin == C) { // remove first
		pStructInfo->pHeaderA = prevpp->pNext;
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

HEADER_D* remove_header_d(HEADER_D* pStructInfo, HEADER_D* pList) {
	if (pStructInfo == pList) { // first item 
		pList = pStructInfo->pNext;
		delete pStructInfo;
	}
	else if (pStructInfo->pNext == nullptr) { //last item
		pStructInfo->pPrior->pNext = nullptr;
		delete pStructInfo;
	}
	else {
		// 
		pStructInfo->pPrior->pNext = pStructInfo->pNext;
		pStructInfo->pNext->pPrior = pStructInfo->pPrior;
		delete pStructInfo;
	}

	return pList;
}