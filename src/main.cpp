#include <cstdlib>
#include <string.h>

#include "Scanner.h"
#include "Parser.h"

using namespace std;
using namespace ff;

int main(int argc, char** argv) {
    string strCode;
    FILE* fp = ::fopen("./test.py", "r");
    if(NULL != fp)
    {
        char buf[2048];
        int n = fread(buf, 1, sizeof(buf), fp);
        ::fclose(fp);
        strCode.assign(buf, n);
        //printf("��ȡ:\n%s\n", strCode.c_str());
    }
    else{
        printf("��ȡʧ�ܣ�\n");
    }

    Scanner scanner;
    scanner.tokenize(strCode);
    
    
    Parser parser;
    try{
        ExprASTPtr rootExpr = parser.parse(scanner);
    
        if (rootExpr){
            printf("%s", rootExpr->dump(0).c_str());
        }else{
            printf("parser.parse ʧ�ܣ�\n");
        }
    }
    catch(exception& e){
        printf("%s\n", e.what());
    }
    
    return 0;
}
