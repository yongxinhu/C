#include "unitTest.h"
#include <stdio.h>
int UT_AllTestNum = 0, UT_PassedTestNum = 0;
void assertionError(int line){
    printf("assertion error at line %d\n",line);
}
