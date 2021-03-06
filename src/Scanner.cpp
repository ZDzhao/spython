#include <stdlib.h>

#include "Base.h"
#include "Scanner.h"

using namespace std;
using namespace ff;

string Token::dump() const{
    char msg[64] = {0};
    switch(nTokenType){
        case TOK_EOF:{
            return "TOK_EOF";
            break;
        }
        case TOK_VAR:{
            return strVal + "(var)";
            break;
        }
        case TOK_INT:{
            snprintf(msg, sizeof(msg), "%ld(int)", long(nVal));
            return msg;
            break;
        }
        case TOK_FLOAT:{
            snprintf(msg, sizeof(msg), "%f(float)", fVal);
            return msg;
            break;
        }
        case TOK_CHAR:{
            if (strVal.empty() == false && strVal[0] == '\n'){
                return "\\n(char)";
            }
            char buff[64] = {0};
            snprintf(buff, sizeof(buff), "%c,%d(char)", strVal[0], int(strVal[0]));
            return buff;
            break;
        }
        default:{
            char buff[64] = {0};
            snprintf(buff, sizeof(buff), "%s,(%d)", strVal.c_str(), nTokenType);
            return buff;
            break;
        }
            break;
    }
    return msg;
}

char Scanner::getCharNext(const string& content, int& index) {
    char ret = 0;
    do{
        if (index < (int)content.size()){
            ret = content[index++];
        }else{
            ret = 0;
        }
    }while(ret == '\r');
    
    
    return ret;
}
/// gettok - Return the next token from standard input.
Token Scanner::getOneToken(const std::string& content, int& index) {
    Token retToken;

    //!ignore comment 
    char cLastOne = getCharNext(content, index);
    
    while (cLastOne == ' ') {
        // Comment until end of line.
        cLastOne = getCharNext(content, index);
        if (cLastOne == '\\'){
            char nextOne = getCharNext(content, index);
            if (nextOne == '\n'){
                if (index > (int)m_hasSearchMaxIndex){
                    m_hasSearchMaxIndex = index;
                    m_nCurLine++;
                }
                cLastOne = getCharNext(content, index);
            }
            else{
                index--;//!pop last char \n
            }
        }
    }
    while (cLastOne == '#') {
        // Comment until end of line.
        do{
            cLastOne = getCharNext(content, index);
        }
        while (cLastOne != TOK_EOF && cLastOne != '\n');
    }
 
    if (::isalpha(cLastOne) || cLastOne == '_') { // identifier: [a-zA-Z][a-zA-Z0-9]*
        retToken.strVal = cLastOne;
        while (::isalnum((cLastOne = getCharNext(content, index))) || cLastOne == '_'){
            retToken.strVal += cLastOne;
        }
        if (cLastOne != 0)
            index--;//!pop last char \n
        retToken.nTokenType = TOK_VAR;
        return retToken;
    }

    int nPlusMinus = 1;
    if (cLastOne == '-'){//! check plus symbol
       char nextOne = getCharNext(content, index);
       if (::isdigit(nextOne) && nextOne != '0'){
           nPlusMinus = -1;
           cLastOne = nextOne;
       }
       else{
           index--;//! back push one char
       }
    }
    //!check 0x prefix
    bool bit16Flag = false;
    if (cLastOne == '0'){//! check plus symbol
       char nextOne = getCharNext(content, index);
       if (nextOne == 'x' || nextOne == 'X'){
           bit16Flag = true;
       }
       else{
           index--;//! back push one char
       }
    }
    if (::isdigit(cLastOne)) { // Number: [0-9.]+
        string strNum;
        if (bit16Flag){
            strNum = "0x";
        }
        bool isFloat = false;
        do {
            if (cLastOne == '.'){
                isFloat = true;
            }
            strNum += cLastOne;
            cLastOne = getCharNext(content, index);
        } while (::isdigit(cLastOne) || cLastOne == '.' || (bit16Flag && ::isalpha(cLastOne)));

        if (false == isFloat){
            if (bit16Flag){
                PyInt nTmp = 0;
                m_istr.clear();
                m_istr << std::hex << strNum;
                m_istr >> nTmp;
                
                //::sscanf(strNum.c_str(), "%lx", &nTmp);
                retToken.nVal = nPlusMinus * nTmp;
            }
            else{
                m_istr.clear();
                m_istr << strNum;
                m_istr >> retToken.nVal;
                retToken.nVal = nPlusMinus * retToken.nVal;
            }
            retToken.nTokenType = TOK_INT;
        }
        else{
            m_istr.clear();
            m_istr << strNum;
            m_istr >> retToken.fVal;
            
            //retToken.fVal = nPlusMinus * ::atof(strNum.c_str());
            retToken.fVal = nPlusMinus * retToken.fVal;
            retToken.nTokenType = TOK_FLOAT;
        }

        if (cLastOne != 0)
            index--;//!pop last char \n
        return retToken;
    }

    if (cLastOne == '\'' || cLastOne == '\"') {
        string tmpC;
        tmpC += cLastOne; //!check ''' or """
        char nextC = getCharNext(content, index);
        if (nextC == cLastOne){
            char nextC2 = getCharNext(content, index);
            if (nextC2 == cLastOne){
                tmpC += cLastOne;
                tmpC += cLastOne;
            }
            else{
                index -= 2;
            }
        }
        else{
            index -= 1;
        }
        retToken.strVal.clear();
        do {
            cLastOne = getCharNext(content, index);
            if (cLastOne == '\\'){
                char nextOne = getCharNext(content, index);
                if (nextOne == '\n'){
                    cLastOne = getCharNext(content, index);
                    if (index > (int)m_hasSearchMaxIndex){
                        m_hasSearchMaxIndex = index;
                        m_nCurLine++;
                    }
                }
                else{
                    index--;//!pop last char \n
                }
            }
            else if (cLastOne == tmpC[0]){
                if (tmpC.size() == 1){
                    char nextOne = getCharNext(content, index);
                    char nextOne2 = getCharNext(content, index);
                    char nextOne3 = getCharNext(content, index);
                    if (nextOne == '\\' && nextOne2 == '\n' && nextOne3 == tmpC[0]){
                        if (index > (int)m_hasSearchMaxIndex){
                            m_hasSearchMaxIndex = index;
                            m_nCurLine++;
                        }
                    }
                    else{
                        index -= 3;
                        break;
                    }
                }
                else {
                    char nextOne = getCharNext(content, index);
                    char nextOne2 = getCharNext(content, index);
                    if (nextOne == tmpC[1] && nextOne2 == tmpC[2]){
                        break;
                    }
                    index -= 2;
                }
            }
            retToken.strVal += cLastOne;
            if (cLastOne == '\n' && index > (int)m_hasSearchMaxIndex){
                m_hasSearchMaxIndex = index;
                m_nCurLine++;
            }
        } while (cLastOne != TOK_EOF);

        retToken.nTokenType = TOK_STR;
        return retToken;
    }

    // Check for end of file.  Don't eat the EOF.
    if (cLastOne == TOK_EOF)
        return retToken;

    retToken.strVal        = cLastOne;
    if (cLastOne ==';'){
        retToken.strVal        = cLastOne;
    }
    //! augassign: ('+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' |
    //!             '<<=' | '>>=' | '**=' | '//=')
    //! comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|
    switch (cLastOne){
        case '+':
        case '-':
        case '%':
        case '&':
        case '^':
        case '|':
        {
            cLastOne = getCharNext(content, index);
            if (cLastOne == '='){
                retToken.strVal += '=';
            }
            else{
                --index;
            }
        }break;
        case '<':
        case '>':
        {
            char cLastOne2 = getCharNext(content, index);
            char cLastOne3 = getCharNext(content, index);
            if (cLastOne2 == '='){
                retToken.strVal += cLastOne2;
                index -= 1;
            }
            else if (cLastOne == cLastOne2 && cLastOne3 != '='){
                retToken.strVal += cLastOne2;
                index -= 1;
            }
            else if (cLastOne == cLastOne2 && cLastOne3 == '='){
                retToken.strVal += cLastOne2;
                retToken.strVal += cLastOne3;
            }
            else if (cLastOne == '<' && cLastOne2 == '>'){
                retToken.strVal += cLastOne2;
                index -= 1;
            }
            else{
                index -= 2;
            }
        }break;
        case '/':
        case '*':{
            char cLastOne2 = getCharNext(content, index);
            char cLastOne3 = getCharNext(content, index);
            if (cLastOne2 == '='){
                retToken.strVal += cLastOne2;
                if (cLastOne3 != TOK_EOF){
                    index -= 1;
                }
            }
            else if (cLastOne == cLastOne2 && cLastOne3 == '='){
                retToken.strVal += cLastOne2;
                retToken.strVal += cLastOne3;
            }
            else{
                if (cLastOne3 != TOK_EOF){
                    index -= 1;
                }
                if (cLastOne2 != TOK_EOF){
                    index -= 1;
                }
            }
            break;
        }
        case '=':
        case '!':
        {
            cLastOne = getCharNext(content, index);
            if (cLastOne == '='){
                retToken.strVal += '=';
            }
            else{
                --index;
            }
        }break;
        default:
            break;
    }
    // Otherwise, just return the character as its ascii value.
    if (retToken.strVal[0] == '\n' && index > (int)m_hasSearchMaxIndex){
        m_hasSearchMaxIndex = index;
        m_nCurLine++;
    }
    retToken.nTokenType    = TOK_CHAR;
    return retToken;
}

Scanner::Scanner():m_nSeekIndex(0), m_nCurLine(1), m_hasSearchMaxIndex(0), m_nCurFileId(0){
}
void Scanner::calLineIndentInfo(const std::string& content){
    int nLine = 1;
    LineInfo& lineInfo = m_allLines[nLine];
    bool bLineCal = true;
    for (unsigned int j = 0; j < content.size(); ++j){
        if (content[j] == '\n')
            break;
        lineInfo.strLine += content[j];
        
        if (bLineCal && content[j] == ' '){
            lineInfo.nIndent ++;
        }
        else{
            bLineCal = false;
        }
    }

    for (unsigned int i = 0; i < content.size(); ++i)
    {
        char ret = content[i];
        if (ret != '\n'){
            continue;
        }

        ++nLine;
        
        LineInfo& lineInfo = m_allLines[nLine];
        
        bool bLineCal = true;
        for (unsigned int j = i + 1; j < content.size(); ++j){
            if (content[j] == '\n')
                break;
            lineInfo.strLine += content[j];
            
            if (bLineCal && (content[j] == ' ' || content[j] == '\t')){
                lineInfo.nIndent ++;
            }
            else{
                bLineCal = false;
            }
        }
        string& lastLine = m_allLines[nLine - 1].strLine;
        if (lastLine.empty() == false && lastLine[lastLine.size() - 1] == '\\'){
            lineInfo.nIndent = m_allLines[nLine - 1].nIndent;
        }
        //printf("line:%d indent:%d\n", nLine, lineInfo.nIndent);
    }
}
static bool isutf8(const std::string& content){
    if (content.size() >= 3 && content[0] == -17 && content[1] == -69 && content[2] == -65){
        return true;
    }
    return false;
}
bool Scanner::tokenizeFile(const std::string& path, int nFileId){
    FILE* fp = ::fopen(path.c_str(), "r");
    string strCode;
    if(NULL == fp)
    {
        printf("��ȡʧ�ܣ�\n");
        return false;
    }
    
    char buf[1024];
    int numread = 0;
    do {
        numread  = fread(buf, 1, sizeof(buf), fp);
        if (numread> 0){
            strCode.append(buf, numread);
        }
        //printf("11111111\n");
    }while (numread == sizeof(buf));
    ::fclose(fp);
    m_nCurFileId = nFileId;
    if (isutf8(strCode)){
        strCode.erase(strCode.begin(), strCode.begin() + 3);
    }
    
    return tokenize(strCode);
}

bool Scanner::tokenize(const std::string& content){
    //printf("tokenize 11111111\n");
    
    int nIndex = 0;
    m_nCurLine = 1;
    m_hasSearchMaxIndex = 0;
    Token retToken;
    
    calLineIndentInfo(content);
    //if (content.find("createRole") != std::string::npos)
    //    dump();
    do{
        retToken = getOneToken(content, nIndex);
        if (retToken.strVal == " "||retToken.strVal == "\t"){
            continue;
        }
        if (retToken.strVal == "\r"){
            retToken.strVal = "\n";
        }
        //if (retToken.strVal == "*"){
        //    printf("token2:%s, Line:%d\n", retToken.dump().c_str(), retToken.nLine);
        //}
        retToken.nLine = m_nCurLine;
        m_allTokens.push_back(retToken);

        //printf("token:%s, Line:%d %d\n", retToken.dump().c_str(), retToken.nLine, m_allLines[retToken.nLine].nIndent);

    }while (retToken.nTokenType != TOK_EOF && m_allTokens.size() < content.size());

    //printf("tokenize 11111111 end\n");
    m_nSeekIndex = 0;
    return true;
}
void Scanner::dump(){
    map<int, LineInfo>::iterator it = m_allLines.begin();
    for (; it != m_allLines.end(); ++it){
        printf("line:%d,code:%s,indent:%d\n", it->first, it->second.strLine.c_str(), it->second.nIndent);
    }
}
const Token* Scanner::getToken(int nOffset){
    
    int nIndex = m_nSeekIndex + nOffset;
    if (nIndex >= 0 && nIndex < (int)m_allTokens.size()){
        return &(m_allTokens[nIndex]);
    }
    
    return &(m_tokenEOF);
}

int Scanner::seek(int nOffset){
    m_nSeekIndex += nOffset;
    return m_nSeekIndex;
}

int Scanner::skipEnterChar(){
    int nRet = m_nSeekIndex;
    do{
        while (this->getToken()->strVal == " "||this->getToken()->strVal == "\t"||this->getToken()->strVal == "\\"){
            ++m_nSeekIndex;
        }
        if (this->getToken()->strVal == "\n"){
            ++m_nSeekIndex;
            nRet = m_nSeekIndex;
        }
        else{
            //printf("dump %s %d\n", this->getToken()->strVal.c_str(), this->getToken()->nLine);
            break;
        }
    }while (true);
    m_nSeekIndex = nRet;
    return m_nSeekIndex;
}
int Scanner::curIndentWidth(int nOffset){
    return m_allLines[getToken(nOffset)->nLine].nIndent;
}

std::string Scanner::getLineCode(int nLine){
    return m_allLines[nLine].strLine; 
}
 
std::map<int, std::string> Scanner::getAllLineCode(){
    std::map<int, std::string> ret;
    std::map<int, LineInfo>::iterator it = m_allLines.begin();
    for (; it != m_allLines.end(); ++it){
        ret[it->first] = it->second.strLine;
    }
    return ret;
}

