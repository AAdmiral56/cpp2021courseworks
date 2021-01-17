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

void PrintDataStructure(HEADER_D*);
HEADER_D* FillInAllItems(HEADER_D*, char* pNewItemID);
HEADER_D* InsertItem(HEADER_D* pStruct, char* pNewItemID = 0);
HEADER_D* RemoveItem(HEADER_D* pStruct, char* pItemID) {
	printf("Removing");
	return pStruct;
}
int cti(char c) {
	if (isupper(c)) {
		return (int)c - 65;
	}
	return -1;
}

int main()
{
    HEADER_D* pList = GetStruct4(4, 10);
    ITEM4* pToItems = (ITEM4*)GetItem(4);
    char str[] = "M C";
    char* ID = str;
    PrintDataStructure(pList);
    return 0;
	const char* str[13] = { "Z A", "Z Z", "Z K", "A Z", "A A", "A K", "G Z", "G A", "G K", "M A", "M Ba", "M Bb", "M Z" };


	for (int i = 0; i < 13; i++) {
		pList = InsertItem(pList, (char*)str[i]);
	}
	PrintDataStructure(pList);

	printf("--------------------\n");

	for (int i = 0; i < 13; i++) {
		RemoveItem(pList, (char*)str[i]);
	}

	PrintDataStructure(pList);

	
	return 0;
}


void PrintDataStructure(HEADER_D* pList)
{
    if (!pList) // check input
        return; // errors
    int counter1 = 1;
    headerD* pToPlacehVariable;
    for (pToPlacehVariable = pList; pToPlacehVariable; pToPlacehVariable = pToPlacehVariable->pNext)
    {
		HEADER_A* AnotherV;
		for (AnotherV = pToPlacehVariable->pHeaderA;AnotherV;AnotherV = AnotherV->pNext)
		{
			ITEM4* itemsV;
			for (itemsV = (ITEM4*)(AnotherV->pItems); itemsV; itemsV = itemsV->pNext)
			{
				printf("%d)%s %lu %s\n", counter1, itemsV->pID, itemsV->Code, itemsV->pDate);
				counter1++;
			}

		}
	}
	return;
}
HEADER_D* FillInAllItems(HEADER_D* pList, char* pNewItemID)
{
	if (!pList) 
	{ //check if pList is empty
		printf("Empty parameters");
		return pList;
	}

	char ID1{}, ID2{};
	if (pNewItemID == 0)
	{
	}
	else if (isupper(*pNewItemID) && isspace(*(pNewItemID + 1)) && isupper(*(pNewItemID + 2)) && '\0' == *(pNewItemID + 3))
	{ //if everything is ok
		ID1 = *(pNewItemID);
		ID2 = *(pNewItemID + 2);
	}
	else //if pList is for smth different
	{
		printf("Wrong Format");
		return pList;
	}
    HEADER_D* p;
	bool jump = false;
	for (p = pList;p;p = p->pNext)
	{
		if (p->pNext == nullptr || p->pNext->cBegin > ID1)
		{
			break;
		}
		jump = true;
	}

	if (p->cBegin == ID1)
	{ //It exsists
		//printf("1");
		printf("%c\n", p->cBegin);

		HEADER_A* pp;
		bool jump2 = false;
		for (pp = p->pHeaderA;pp;pp = pp->pNext)
		{
			if (pp->pNext == nullptr || pp->pNext->cBegin > ID2)
			{
				break;
			}
			jump2 = true; //may be need to put before if statment
		}

		if (pp->cBegin == ID2)
		{ //11 case

			ITEM4* i = (ITEM4*)(pp->pItems);
			ITEM4* pNewItem = (ITEM4*)GetItem(9, pNewItemID);
			pNewItem->pNext = i;
			pp->pItems = pNewItem;
		}

		else 
		{ //10 case
			if (!jump2 && pp->cBegin > ID2) 
			{ //if its first
				printf("FIRST!");
				HEADER_A* newHeader_A = new headerA();
				newHeader_A->cBegin = ID2;
				newHeader_A->pItems = (ITEM4*)GetItem(9, pNewItemID);
				newHeader_A->pNext = pp;
				p->pHeaderA = newHeader_A;

			}
			else
			{ //if not
				HEADER_A* newHeader_A = new headerA();
				newHeader_A->cBegin = ID2;
				newHeader_A->pItems = (ITEM4*)GetItem(9, pNewItemID);
				newHeader_A->pNext = pp->pNext;
				pp->pNext = newHeader_A;

			}
		}
	}

	else { //00 case

		HEADER_A* newHeader_A = new headerA();
		newHeader_A->cBegin = ID2;
		newHeader_A->pItems = (ITEM4*)GetItem(9, pNewItemID);
		newHeader_A->pNext = nullptr;

		HEADER_D* newHeader_D = new headerD();
		newHeader_D->cBegin = ID1;
		newHeader_D->pHeaderA = newHeader_A;

		if (!jump && p->cBegin > ID1) 
		{ //if first

			newHeader_D->pNext = pList;
			pList = newHeader_D;
		}
		else
		{
			newHeader_D->pNext = p->pNext;
			p->pNext = newHeader_D;
		}
	}

	return pList;

}



HEADER_D* InsertItem(HEADER_D* pStruct, char* pNewItemID) {
	if (!pStruct) {
		//throw("Invalid pointer\n");
		printf("Invalid pointer\n");
		return pStruct;
	}

	char ID1, ID2;
	ITEM4* pNewItemBuffer;

	if (pNewItemID == 0) {//if no ID specified
		pNewItemBuffer = (ITEM4*)GetItem(4);
		ID1 = *pNewItemBuffer->pID;
		char* buffer;
		for (buffer = pNewItemBuffer->pID; !isspace(*buffer); *buffer++);
		ID2 = *(buffer + 1);
	}

	else if (sizeof(pNewItemID) == 10)
	{
		printf_s("size is correct");
		ID1 = *(pNewItemID), ID2 = rand() % 9999999999 + 1111111111;
		if(ID1 == ID2)
		{
			printf_s("That one in a million chance has come up when the IDs are the same. Regenerating ID");
		  ID2 = rand() % 9999999999 + 1111111111;
		}
	}
	/*else if (isupper(*pNewItemID) && //ID specified and correct
		isspace(*(pNewItemID + 1)) &&
		isupper(*(pNewItemID + 2)) &&
		'\0' == *(pNewItemID + 3)) {
		ID1 = *(pNewItemID), ID2 = *(pNewItemID + 2);
		pNewItemBuffer = (ITEM4*)GetItem(4, pNewItemID);
	}
	*/
	else { //ID specified and wrong
		printf("Identifier \"%s\" is in Wrong Format\n", pNewItemID);
		return pStruct;
	}

	HEADER_D* p;
	bool jump = false;
	for (p = pStruct; p; p = p->pNext) {
		if (p->pNext == nullptr || p->pNext->cBegin > ID1) {
			break;
		}
		jump = true;
	}

	std::string ss;
	//std::stringstream ss;
	std::cout << "my_struct has " << boost::pfr::tuple_size<HEADER_D>::value << " fields: " << boost::pfr::io()<< "\n";

	/*if (p->cBegin == ID1) { //Checking IDs
		int pp = cti(ID2);
		HEADER_D p;
		ITEM4* cycle;
		for(cycle = (ITEM4*)(HEADER_D::pPrior->pHeaderA[pp]);  < length; ++)
		{

		} */
	/*
		ITEM4* cycle;
		for (cycle = (ITEM4*)(p->pHeaderA[pp]); cycle; cycle = cycle->pNext)
		{
			if (!strcmp(cycle->pID, pNewItemBuffer->pID)) {
				//throw("Item already exists\n");
				printf("\"%s\" already exsists\n", pNewItemBuffer->pID);
				return pStruct;
			}
		}

		ITEM4* ppp = (ITEM4*)GetItem(4, pNewItemID);
		ppp->pNext = (ITEM4*)p->*cBegin[pp];
		p->*pItems[pp] = ppp;
		printf("Item added: %s\n", ppp->pID);
		return pStruct;
		*/
	}
	else { //00
			   //printf("not found");
		HEADER_D* newHeader_D = new headerD();
		ITEM4** ptr = (ITEM4**)calloc(26, sizeof(ITEM4*));
		newHeader_D->cBegin = ID1;
		int pp;
		for (pp = 0; pp < cti(ID2); pp++);
		ptr[pp] = pNewItemBuffer;

		//newHeader_D->pNext = *ptr;

		if (!jump && p->cBegin > ID1) { //its first
			newHeader_D->pNext = pStruct;
			pStruct = newHeader_D;
		}
		else {//its not
			newHeader_D->pNext = p->pNext;
			p->pNext = newHeader_D;
		}

		return pStruct;
	}
}

/*
HEADER_D* RemoveItem(HEADER_D* pStruct, char* pItemID) {
	printf("Removing");
	return pStruct;
}
	//printf("Item to delete: %s\n", pItemID);
	
	if (!pStruct) {
		//throw("Invalid pointer\n");
		printf("Invalid pointer\n");
		return pStruct;
	}

	if (!pItemID) {
		//throw("Invalid pointer\n");
		printf("Invalid item id\n");
		return pStruct;
	}

	char ID1;
	char ID2;
	ITEM3* pItemDelete;


	char* secondWord = strchr(pItemID, ' ');
	if (isupper(*pItemID) && (secondWord != NULL) && isupper(secondWord[1])) {
		ID1 = *(pItemID), ID2 = secondWord[1];
	}
	else
	{ //ID specified and wrong
			   //throw("IIdentifier has a Wrong Format\n");
		printf("Identifier \"%s\" is in Wrong Format\n", pItemID);
		return pStruct;
	}

	HEADER_A* pHeader = pStruct;
	//for (; pHeader && pHeader->cBegin != ID1; pHeader = pHeader->pNext);

	do
	{
		if (!pHeader) break;
		if (pHeader->cBegin == ID1)
		{
			break;
		}

		pHeader = pHeader->pNext;

	} while (true);

	if (!pHeader) {
		//throw("IIdentifier has a Wrong Format\n");
		printf("No ID found\n");
		return pStruct;
	}

	ITEM3* pList = (ITEM3*)pHeader->headerA::pItems[ID2 - 'A'];
	if (!pList) {
		//throw("No ID found\n");
		printf("No ID found\n");
		return pHeader;
	}
	ITEM3* pItem = pList; //(ITEM3*)pHeader->ppItems[ID2]
	ITEM3* pItemBefore = pList; //(ITEM3*)pHeader->ppItems[ID2]

	//for (; pItem && strcmp(pItem->pID, pItemID); pItemBefore = pItem, pItem = pItem->pNext){}

	do
	{
		if (!pItem) break;
		if (strcmp(pItem->pID, pItemID) == 0)
		{
			break;
		}

		pItemBefore = pItem;
		pItem = pItem->pNext;

	} while (true);


	if (!pItem) {
		//throw("No ID found\n");
		printf("No ID found\n");
		return pStruct;
	}

	if (pItem == pItemBefore) {

		pHeader->headerA::pItems[ID2 - 'A'] = 0;
	}
	else {
		pItemBefore->pNext = pItem->pNext;
	}

	free(pItem);

	if (pHeader->headerA::pItems == 0)
	{
		if (pHeader->headerD::pPrior) {
			pHeader->pPrior->pNext = pHeader->pNext;
		}
		else {
			pHeader->pNext->pPrior = NULL;
			pStruct = pHeader->pNext;
		}
		if (pHeader->pNext) {
			pHeader->pNext->pPrior = pHeader->pPrior;
		}
		else {
			pHeader->pPrior->pNext = NULL;
		}
		free(pHeader->headerA::pItems);
		free(pHeader);
	}

	return pStruct;

	
*/