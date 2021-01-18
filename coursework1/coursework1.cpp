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
	system("Color 1A");
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

HEADER_D* InsertItem(HEADER_D* pList, char* pID = 0) 
{

	if (pList == nullptr) { return pList; } //if no data was collected from the starting staement in main
    ITEM4* item;

	if (pID == nullptr) { //if data was collected and custom id was no chosen 
		item = (ITEM4*)GetItem(4);
		return add_header_d(pList, item);
	}
	else if (!isupper(*pID) || !isupper(*(pID + 2)) || *(pID + 1) != ' ' || *(pID + 3) != '\0') { //if data is collected and the custom ID doesnt match the required pattern
		cout << "item " << pID << " is invalid!" << endl;
		return pList;
	}

	item = (ITEM4*)GetItem(4, pID); //if everything is okay
	return add_header_d(pList, item);
}

HEADER_D* add_header_d(HEADER_D* pList, ITEM4* item) {

	if (item == nullptr) { return pList; } // no ITEM4 is found

	for (HEADER_D* pStructInfo = pList; pStructInfo != nullptr && id1(item->pID) >= pStructInfo->cBegin; pStructInfo = pStructInfo->pNext) { //Item is present in selection
		if (id1(item->pID) == pStructInfo->cBegin) {
			add_header_a(pStructInfo, item);
			return pList;
		}
	}
	// No matching item found
	HEADER_D* headerDmember = new headerD();
	headerDmember->cBegin = id1(item->pID);
	if (pList == nullptr) { //first only
		headerDmember->pNext = pList;
		pList = headerDmember;
		add_header_a(headerDmember, item);
		return pList;
	}
	else if (id1(item->pID) < pList->cBegin) { // just first
		headerDmember->pNext = pList;
		pList->pPrior = headerDmember;
		pList = headerDmember;
		add_header_a(headerDmember, item);
		return pList;
	}

	HEADER_D* pStructInfo = pList;
	for (pStructInfo; pStructInfo != nullptr && id1(item->pID) > pStructInfo->cBegin; pStructInfo = pStructInfo->pNext)
	{ // if item in last
		if (pStructInfo->pNext == nullptr) 
		{
			headerDmember->pPrior = pStructInfo;
			pStructInfo->pNext = headerDmember;
			add_header_a(headerDmember, item);
			return pList;
		}
	}
	headerDmember->pPrior = pStructInfo->pPrior; //appending to middle
	headerDmember->pNext = pStructInfo;
	pStructInfo->pPrior->pNext = headerDmember;
	pStructInfo->pPrior = headerDmember;
	add_header_a(headerDmember, item);
	return pList;
}

void add_header_a(HEADER_D* pStructInfo, ITEM4* item) {

	for (HEADER_A* ppHeaderStuff = pStructInfo->pHeaderA; ppHeaderStuff != nullptr && id2(item->pID) >= ppHeaderStuff->cBegin; ppHeaderStuff = ppHeaderStuff->pNext) {
		if (id2(item->pID) == ppHeaderStuff->cBegin) 
		{
			add_item(ppHeaderStuff, item);
			return;
		}
	}
	// does not
	HEADER_A* headerAmember = new headerA(); //creating new header
	headerAmember->cBegin = id2(item->pID);
	if (pStructInfo->pHeaderA == nullptr) { //First place
		headerAmember->pNext = pStructInfo->pHeaderA;
		pStructInfo->pHeaderA = headerAmember;
		add_item(headerAmember, item);
		return;
	}
	else if (id2(item->pID) < pStructInfo->pHeaderA->cBegin) { // just first
		HEADER_A* temp = pStructInfo->pHeaderA;
		headerAmember->pNext = temp;
		pStructInfo->pHeaderA = headerAmember;
		add_item(headerAmember, item);
		return;
	}

	HEADER_A* ppHeaderStuff = pStructInfo->pHeaderA;
	HEADER_A* linePrev = ppHeaderStuff;
	for (ppHeaderStuff; ppHeaderStuff != nullptr && id2(item->pID) > ppHeaderStuff->cBegin; ppHeaderStuff = ppHeaderStuff->pNext) { //Append the set to the end
		if (ppHeaderStuff->pNext == nullptr) 
		{
			ppHeaderStuff->pNext = headerAmember;
			add_item(headerAmember, item);
			return;
		}
		linePrev = ppHeaderStuff;
	}
	// Append to middle
	headerAmember->pNext = ppHeaderStuff;
	linePrev->pNext = headerAmember;
	add_item(headerAmember, item);
}

void add_item(HEADER_A* ppHeaderStuff, ITEM4* item) 
{

	for (ITEM4* otheHeaderVar3 = (ITEM4*)ppHeaderStuff->pItems; otheHeaderVar3 != nullptr; otheHeaderVar3 = otheHeaderVar3->pNext)
	{
		if (!strcmp(otheHeaderVar3->pID, item->pID))
		{
			cout << "Item " << item->pID << " already exists!" << endl;
			return;
		}
	}

	ITEM4* otheHeaderVar3 = (ITEM4*)ppHeaderStuff->pItems;
	for (otheHeaderVar3; otheHeaderVar3 != nullptr; otheHeaderVar3 = otheHeaderVar3->pNext) { // Break off adding items on the last one possible
		if (otheHeaderVar3->pNext == nullptr) {
			break;
		}
	}
	if (otheHeaderVar3 == nullptr) { ppHeaderStuff->pItems = item; }
	else { otheHeaderVar3->pNext = item; }
}

HEADER_D* RemoveItem(HEADER_D* pList, char* pID) {

	if (!isupper(*pID) || !isupper(*(pID + 2)) || *(pID + 1) != ' ' || *(pID + 3) != '\0') { return pList; } //wrong ID format check

	HEADER_D* pStructInfo = pList; //this giant loop here is for checking that all data portions are present and in the correct format before deleting them
	for (pStructInfo; pStructInfo != nullptr && pStructInfo->cBegin <= id1(pID); pStructInfo = pStructInfo->pNext) {
		if (id1(pID) == pStructInfo->cBegin) {
			HEADER_A* ppHeaderStuff, * prevpp = pStructInfo->pHeaderA;
			for (ppHeaderStuff = pStructInfo->pHeaderA; ppHeaderStuff != nullptr && ppHeaderStuff->cBegin <= id2(pID); ppHeaderStuff = ppHeaderStuff->pNext) {
				if (id2(pID) == ppHeaderStuff->cBegin) {
					ITEM4* otheHeaderVar3, * prevppp = (ITEM4*)ppHeaderStuff->pItems;
					for (otheHeaderVar3 = (ITEM4*)ppHeaderStuff->pItems; otheHeaderVar3 != nullptr; otheHeaderVar3 = otheHeaderVar3->pNext) {
						if (!strcmp(otheHeaderVar3->pID, pID))  //strcmp checks for differences in these IDs. Since we want them to be identical, we put the '!' there
						{

							ITEM4* comp = (ITEM4*)ppHeaderStuff->pItems; //first item to compare 
							if (otheHeaderVar3->pID == comp->pID && otheHeaderVar3->pNext == nullptr) { return remove_header_a(prevpp, pStructInfo, id2(pID), pList); } //remove the entire header of pNext is a null pointer
							else if (otheHeaderVar3->pID == comp->pID) { ppHeaderStuff->pItems = otheHeaderVar3->pNext;}
							else if (otheHeaderVar3->pNext == nullptr) { prevppp->pNext = nullptr;}
							else { prevppp->pNext = otheHeaderVar3->pNext; }
							delete otheHeaderVar3->pID;
							delete otheHeaderVar3->pDate;
							delete otheHeaderVar3;
							return pList;
						}

						prevppp = otheHeaderVar3;
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
	if (prevpp == pStructInfo->pHeaderA && prevpp->pNext == nullptr) {//remove the entire header of pNext is a null pointer
		delete prevpp;
		return remove_header_d(pStructInfo, pList);
	}
	else if (prevpp == pStructInfo->pHeaderA && prevpp->cBegin == C) { // remove the first variable in list
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
	if (pStructInfo == pList) { // first variables
		pList = pStructInfo->pNext;
		delete pStructInfo;
	}
	else if (pStructInfo->pNext == nullptr) { //last in the list
		pStructInfo->pPrior->pNext = nullptr;
		delete pStructInfo;
	}
	else {
		// 
		pStructInfo->pPrior->pNext = pStructInfo->pNext; //everything in between
		pStructInfo->pNext->pPrior = pStructInfo->pPrior;
		delete pStructInfo;
	}

	return pList;
}