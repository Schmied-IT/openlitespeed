
/*****************************************************************************
*    Open LiteSpeed is an open source HTTP server.                           *
*    Copyright (C) 2013 - 2015  LiteSpeed Technologies, Inc.                 *
*                                                                            *
*    This program is free software: you can redistribute it and/or modify    *
*    it under the terms of the GNU General Public License as published by    *
*    the Free Software Foundation, either version 3 of the License, or       *
*    (at your option) any later version.                                     *
*                                                                            *
*    This program is distributed in the hope that it will be useful,         *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
*    GNU General Public License for more details.                            *
*                                                                            *
*    You should have received a copy of the GNU General Public License       *
*    along with this program. If not, see http://www.gnu.org/licenses/.      *
*****************************************************************************/
#ifdef RUN_TEST

#include <lsdef.h>
#include <util/radixtree.h>

#include <stdio.h>
#include <stdlib.h>

#include "test/unittest-cpp/UnitTest++/src/UnitTest++.h"

// #define RADIXTREE_DEBUG

const char *pInputs[] =
{
    "/home/one/user/dir",
    "/home/two/user/dir",
    "/home/one/client/",
    "/home/one/client/dir",
    "/home/one",
    "/home/one/client1",
    "/home/one/client2",
    "/home/one/client3",
    "/home/one/client4",
    "/home/one/client5",
    "/home/one/client6",
    "/home/one/client7",
    "/home/one/client8",
    "/home/one/client9",
    "/home/one/client10",
    "/home/one/client11",
    "/home/one/client12",
    "/home/one/client13",
    "/home/one/client14",
    "/home/one/client15",
    "/home/one/client16",
    "/home/one/client17",
    "/home/one/client18",
    "/home/one/client19",
    "/not/starting/with/slash",
    "/not",
    "/home/one/two/three/four/five/six/seven/eight/nine/ten/"
};
const int iInputLen = 27;

static int test_for_each(void *pObj)
{
#ifdef RADIXTREE_DEBUG
    printf("%p\n", pObj);
#endif
    return 0;
}


static int test_for_each2(void *pObj, void *pUData)
{
    int *p = (int *)pUData;
#ifdef RADIXTREE_DEBUG
    printf("%p %d\n", pObj, ++(*p));
#endif
    return 0;
}


void doTest(RadixTree *pTree, char **pDynamicInputs, int count)
{
    int i;
    RadixTree tree2;
    pTree->printTree();
    for (i = 0; i < count; ++i)
    {
#ifdef RADIXTREE_DEBUG
        printf("Testing add %s\n", pDynamicInputs[i]);
#endif
        CHECK(pTree->insert(pDynamicInputs[i], strlen(pDynamicInputs[i]),
                            pTree) != NULL);
#ifdef RADIXTREE_DEBUG
        pTree->printTree();
#endif
    }

    for (i = 0; i < count; i += 2)
    {
#ifdef RADIXTREE_DEBUG
        printf("Testing update %s\n", pDynamicInputs[i]);
#endif
        CHECK(pTree->update(pDynamicInputs[i], strlen(pDynamicInputs[i]),
                            &tree2) == pTree);
#ifdef RADIXTREE_DEBUG
        pTree->printTree();
#endif
    }

    for (i = 0; i < count; i += 2)
    {
#ifdef RADIXTREE_DEBUG
        printf("Testing find %s\n", pDynamicInputs[i]);
#endif
        CHECK(pTree->find(pDynamicInputs[i], strlen(pDynamicInputs[i])) == &tree2);
#ifdef RADIXTREE_DEBUG
        pTree->printTree();
#endif
    }

    for (i = 1; i < count; i += 2)
    {
#ifdef RADIXTREE_DEBUG
        printf("Testing find %s\n", pDynamicInputs[i]);
#endif
        CHECK(pTree->find(pDynamicInputs[i], strlen(pDynamicInputs[i])) == pTree);
    }

    const char *pBegin = pDynamicInputs[26];
    const char *pNext = (const char *)memchr(pBegin + 1, '/',
                                             strlen(pBegin) - 1);
    void *pOut, *data = pTree->bestMatch(pDynamicInputs[4],
                                  strlen(pDynamicInputs[4]));

    CHECK((pOut = pTree->bestMatch(pBegin, pNext - pBegin)) == NULL);
#ifdef RADIXTREE_DEBUG
    printf("Should be null: %p\n", pOut);
#endif
    while ((pNext = (const char *)memchr(pNext + 1, '/',
                                         strlen(pNext) - 1)) != NULL)
    {
        CHECK((pOut = pTree->bestMatch(pBegin, pNext - pBegin)) == data);
#ifdef RADIXTREE_DEBUG
        printf("Should match: %p %p\n", pOut, data);
#endif
    }

    CHECK(pTree->for_each(test_for_each) == count);
    int p = 0;
    CHECK(pTree->for_each2(test_for_each2, &p) == count);
}

TEST(stringtreetest)
{
    printf("Start Radix Tree Test!\n");
    int i, count = iInputLen, iDynamicCount = iInputLen;
    char **pDynamicInputs = (char **)malloc(sizeof(char *)*iDynamicCount);
    for (i = 0; i < iDynamicCount; ++i)
        pDynamicInputs[i] = strdup(pInputs[i]);

    RadixTree *pContTree = new RadixTree(RTMODE_CONTIGUOUS);
    RadixTree *pPtrTree = new RadixTree(RTMODE_POINTER);
    pContTree->setRootLabel("/", 1);
    pPtrTree->setRootLabel("/", 1);
    printf("CONTIGUOUS TEST\n");
    doTest(pContTree, pDynamicInputs, count);
    printf("END CONTIGUOUS TEST\n");
    printf("POINTER TEST\n");
    doTest(pPtrTree, pDynamicInputs, count);
    printf("END POINTER TEST\n");
    delete pContTree;
    delete pPtrTree;

    for (i = 0; i < iDynamicCount; ++i)
        free(pDynamicInputs[i]);
    free(pDynamicInputs);
    printf("End Radix Tree Test!\n");
}


#endif
