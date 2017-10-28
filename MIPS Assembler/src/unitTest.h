#ifndef UNITTEST_H
#define UNITTEST_H
void assertionError(int line);
extern int UT_AllTestNum, UT_PassedTestNum;
#define Assert(_exp_) \
do{\
    UT_AllTestNum++;\
	if(!(_exp_)) {\
		assertionError(__LINE__);\
    }else{\
        UT_PassedTestNum++;\
    }\
}while(0)

#define AssertEqual(_a,_b) \
do{\
    UT_AllTestNum++;\
    if(((_a)!=(_b))) {\
        assertionError(__LINE__);\
    }else{\
        UT_PassedTestNum++;\
    }\
}while(0)

#endif
