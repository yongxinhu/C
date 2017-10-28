#include <stdio.h>
#include <stdlib.h>
#include "src/utils.h"
#include "src/tables.h"
#include "src/translate_utils.h"
#include "src/translate.h"

#include "src/unitTest.h"
#include "mytest_assembler.h"

#define UT_BUF_SIZE 512
const char* TMP_FILE = "test_output.txt";

int init_log_file() {
    set_log_file(TMP_FILE);
    return 0;
}

int check_lines_equal(char **arr, int num) {
    char buf[UT_BUF_SIZE];

    FILE *f = fopen(TMP_FILE, "r");
    if (!f) {
        printf("Could not open temporary file\n");
        return 0;
    }
    int match = 0;
    for (int i = 0; i < num; i++) {
        if (!fgets(buf, UT_BUF_SIZE, f)) {
            printf("Reached end of file\n");
            return 0;
        }
        match = strncmp(buf, arr[i], strlen(arr[i])) == 0;
        Assert(match);
        if(!match) {
            printf("want:\n%s\nBut get:\n%s\n",arr[i],buf);
        }
    }
    fclose(f);
    return 0;
}

void test_translate_reg() {
    AssertEqual(translate_reg("$0"), 0);
    AssertEqual(translate_reg("$at"), 1);
    AssertEqual(translate_reg("$v0"), 2);
    AssertEqual(translate_reg("$a0"), 4);
    AssertEqual(translate_reg("$a1"), 5);
    AssertEqual(translate_reg("$a2"), 6);
    AssertEqual(translate_reg("$a3"), 7);
    AssertEqual(translate_reg("$t0"), 8);
    AssertEqual(translate_reg("$t1"), 9);
    AssertEqual(translate_reg("$t2"), 10);
    AssertEqual(translate_reg("$t3"), 11);
    AssertEqual(translate_reg("$s0"), 16);
    AssertEqual(translate_reg("$s1"), 17);
    AssertEqual(translate_reg("$3"), -1);
    AssertEqual(translate_reg("asdf"), -1);
    AssertEqual(translate_reg("hey there"), -1);
}
void test_translate_num() {
    long int output;
    AssertEqual(translate_num(&output, "35", -1000, 1000), 0);
    AssertEqual(output, 35);
    AssertEqual(translate_num(&output, "145634236", 0, 9000000000), 0);
    AssertEqual(output, 145634236);
    AssertEqual(translate_num(&output, "0xC0FFEE", -9000000000, 9000000000), 0);
    AssertEqual(output, 12648430);
    AssertEqual(translate_num(&output, "72", -16, 72), 0);
    AssertEqual(output, 72);
    AssertEqual(translate_num(&output, "72", -16, 71), -1);
    AssertEqual(translate_num(&output, "72", 72, 150), 0);
    AssertEqual(output, 72);
    AssertEqual(translate_num(&output, "72", 73, 150), -1);
    AssertEqual(translate_num(&output, "35x", -100, 100), -1);
    AssertEqual(translate_num(&output, "X35", -100, 100), -1);
    AssertEqual(translate_num(&output, "3 5", -100, 100), -1);
    AssertEqual(translate_num(&output, "", -100, 100), -1);
    AssertEqual(translate_num(&output, " 0", -100, 100), 0);
    AssertEqual(translate_num(&output, "  ", -100, 100), -1);
    AssertEqual(translate_num(&output, " 0x", -100, 100), -1);
}
void test_table_0(){
    SymbolTable* tb = create_table(SYMTBL_UNIQUE_NAME);
    add_to_table(tb,"aaa",0);
    add_to_table(tb,"b",4);
    add_to_table(tb,"cc",8);
    add_to_table(tb,"ddd",12);
    add_to_table(tb,"e",16);
    add_to_table(tb,"ff",20);
    Assert(add_to_table(tb,"aaa",28)==-1);
    Assert(add_to_table(tb,"hh",2)==-1);
    Assert(get_addr_for_symbol(tb,"aaa")== 0);
    Assert(get_addr_for_symbol(tb,"ddd")== 12);
    Assert(get_addr_for_symbol(tb,"kkk") == -1);
    Assert(tb->cap==10);
    FILE *tmp = fopen(TMP_FILE,"wt");
    if(tmp == NULL)
        printf("cannot create temp file for testing in testTables().\n");
    write_table(tb,tmp);
    fclose(tmp);
    char *arr[] = {
        "0\taaa",
        "4\tb",
        "8\tcc",
        "12\tddd",
        "16\te",
        "20\tff"
    };
    check_lines_equal(arr,6);
    remove(TMP_FILE);
}
void test_table_1() {
    int retval;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    Assert(tbl!=NULL);

    retval = add_to_table(tbl, "abc", 8);
    AssertEqual(retval, 0);
    retval = add_to_table(tbl, "efg", 12);
    AssertEqual(retval, 0);
    retval = add_to_table(tbl, "q45", 16);
    AssertEqual(retval, 0);
    retval = add_to_table(tbl, "q45", 24);
    AssertEqual(retval, -1);
    retval = add_to_table(tbl, "bob", 14);
    AssertEqual(retval, -1);

    retval = get_addr_for_symbol(tbl, "abc");
    AssertEqual(retval, 8);
    retval = get_addr_for_symbol(tbl, "q45");
    AssertEqual(retval, 16);
    retval = get_addr_for_symbol(tbl, "ef");
    AssertEqual(retval, -1);

    free_table(tbl);

    char* arr[] = { "Error: name 'q45' already exists in table.",
                    "Error: address is not a multiple of 4." };
    check_lines_equal(arr, 2);

    SymbolTable* tbl2 = create_table(SYMTBL_NON_UNIQUE);
    Assert(tbl2!=NULL);

    retval = add_to_table(tbl2, "q45", 16);
    AssertEqual(retval, 0);
    retval = add_to_table(tbl2, "q45", 24);
    AssertEqual(retval, 0);

    free_table(tbl2);
    remove(TMP_FILE);

}
void test_table_2() {

    int retval, max = 100;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    Assert(tbl!=NULL);

    char buf[10];
    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = add_to_table(tbl, buf, 4 * i);
        AssertEqual(retval, 0);
    }

    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = get_addr_for_symbol(tbl, buf);
        AssertEqual(retval, 4 * i);
    }
    free_table(tbl);
    remove(TMP_FILE);
}

void test_translate_inst()
{
    SymbolTable* sbltbl = create_table(SYMTBL_UNIQUE_NAME);
    Assert(sbltbl!=NULL);
    SymbolTable* roltbl = create_table(SYMTBL_UNIQUE_NAME);
    Assert(roltbl!=NULL);
    FILE *f = fopen(TMP_FILE,"w");

    //sll $a0,$t0,2
    char *args1[] = {"$a0","$t0","2"};
    translate_inst(f,"sll",args1,3,0,sbltbl,roltbl);


    //lw $t0, 4($s0)
    char *args2[] = {"$t0","4","$s0"};
    translate_inst(f,"lw",args2,3,0,sbltbl,roltbl);

    //j fib
    char *args3[] = {"fib"};
    translate_inst(f,"j",args3,1,0,sbltbl,roltbl);
    Assert(get_addr_for_symbol(roltbl,"fib")!=-1);

    //beq $t0, $t3, NEXT
    add_to_table(sbltbl,"NEXT",0x0040011C);
    char *args4[] = {"$t0","$t3","NEXT"};
    translate_inst(f,"beq",args4,3,0x00400108,sbltbl,roltbl);

    //xor $v0, $a0, $a1
    char *args5[] = {"$v0","$a0","$a1"};
    translate_inst(f,"xor",args5,3,0,sbltbl,roltbl);

    //jal func2
    char *args6[] = {"func2"};
    translate_inst(f,"jal",args6,1,0,sbltbl,roltbl);
    Assert(get_addr_for_symbol(roltbl,"func2")!=-1);

    //sb $t0, 10($s3)
    char *args7[] = {"$t0","10","$s3"};
    translate_inst(f,"sb",args7,3,0,sbltbl,roltbl);


    //check
    fclose(f);
    char *wanted[] = {"00082080","8e080004","08000000","110b0010"
                      ,"00851026","0c000000","a268000a"};
    check_lines_equal(wanted,7);

    free_table(sbltbl);
    free_table(roltbl);
    remove(TMP_FILE);
}

void runTests() {
    test_translate_reg();
    test_translate_num();

    init_log_file();
    test_table_0();
    test_table_1();
    test_table_2();

    test_translate_inst();


    printf("\n-----test result: %d / %d passed-----\n",UT_PassedTestNum,UT_AllTestNum);

    exit(0);
}
int main() {
    runTests();
    return 0;
}


