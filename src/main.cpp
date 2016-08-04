#include "OldParser.h"

#include <cstdlib>
#include <string.h>
using namespace ff;

#include "Scanner.h"

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
    return 0;
    PyObjPtr mainMod = new PyObjModule("__main__");
    ParseHelper parserHelper(strCode);
    ParseTool tool(mainMod);
    try{
        tool.MainLoop(parserHelper);
    }
    catch(exception& e){
        fprintf(stderr, "exception=%s\n", e.what());
    }
    
    if (argc >= 2 && string(argv[1]) == "-dump")
        mainMod->dump();
	return 0;
}
