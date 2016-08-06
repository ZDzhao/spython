#include "Parser.h"
#include "Scanner.h"
#include "ExprAST.h"

using namespace std;
using namespace ff;
#define THROW_ERROR(X) throwError(X, __LINE__)

Parser::Parser():m_curScanner(NULL){
}

ExprASTPtr Parser::parse(Scanner& scanner){
    m_curScanner = &scanner;
    return this->parse_file_input();
}

//! single_input: NEWLINE | simple_stmt | compound_stmt NEWLINE
ExprASTPtr Parser::parse_single_input(){
    return NULL;
}

//! file_input: (NEWLINE | stmt)* ENDMARKER
ExprASTPtr Parser::parse_file_input(){
    StmtAST* allStmt = new StmtAST();
    ExprASTPtr ret = allStmt;
    
    while (m_curScanner->getToken()->nTokenType != TOK_EOF){
        if (m_curScanner->getToken()->strVal == "\n"){
            m_curScanner->seek(1);
        }
        else{
            ExprASTPtr stmt = this->parse_stmt();
            
            if (stmt){
                stmt->dump(0);
                allStmt->exprs.push_back(stmt);
            }else{
                printf("this->parse_stmt ʧ�ܣ�%d %s\n", m_curScanner->seek(0), m_curScanner->getToken()->dump().c_str());
                m_curScanner->seek(1);
            }
        }
    }
    return ret;
}

//! eval_input: testlist NEWLINE* ENDMARKER
ExprASTPtr Parser::parse_eval_input(){
    return NULL;
}

//! decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
ExprASTPtr Parser::parse_decorator(){
    return NULL;
}

//! decorators: decorator+
ExprASTPtr Parser::parse_decorators(){
    return NULL;
}

//! decorated: decorators (classdef | funcdef)
ExprASTPtr Parser::parse_decorated(){
    return NULL;
}

//! funcdef: 'def' NAME parameters ':' suite
ExprASTPtr Parser::parse_funcdef(){
    return NULL;
}

//! parameters: '(' [varargslist] ')'
ExprASTPtr Parser::parse_parameters(){
    return NULL;
}

//! varargslist: ((fpdef ['=' test] ',')*
//!               ('*' NAME [',' '**' NAME] | '**' NAME) |
//!               fpdef ['=' test] (',' fpdef ['=' test])* [','])
ExprASTPtr Parser::parse_varargslist(){
    return NULL;
}

//! fpdef: NAME | '(' fplist ')'
ExprASTPtr Parser::parse_fpdef(){
    return NULL;
}

//! fplist: fpdef (',' fpdef)* [',']
ExprASTPtr Parser::parse_fplist(){
    return NULL;
}

//! stmt: simple_stmt | compound_stmt
ExprASTPtr Parser::parse_stmt(){
    ExprASTPtr retExpr = parse_simple_stmt();
    if (!retExpr){
        retExpr = parse_compound_stmt();
    }
    return retExpr;
}

//! simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
ExprASTPtr Parser::parse_simple_stmt(){
    DTRACE(("parse_simple_stmt begin..."));
    ExprASTPtr small_stmt = parse_small_stmt();
    
    if (small_stmt && m_curScanner->getToken()->strVal == ";"){
        StmtAST* allStmt = new StmtAST();
        ExprASTPtr ret = allStmt;
        allStmt->exprs.push_back(small_stmt);
        
        while (m_curScanner->getToken()->strVal == ";"){
            m_curScanner->seek(1);
            small_stmt = parse_small_stmt();
            if (!small_stmt){
                break;
            }
            allStmt->exprs.push_back(small_stmt);
        }

        if (m_curScanner->getToken()->strVal == ";"){
            m_curScanner->seek(1);
        }
        
        if (m_curScanner->getToken()->strVal == "\n"){
            m_curScanner->seek(1);
        }

        DTRACE(("parse_simple_stmt end muti small..."));
        return ret;
    }
    DTRACE(("parse_simple_stmt end small..."));
    return small_stmt;
}

//! small_stmt: (expr_stmt | print_stmt  | del_stmt | pass_stmt | flow_stmt |
//!              import_stmt | global_stmt | exec_stmt | assert_stmt)
ExprASTPtr Parser::parse_small_stmt(){
    ExprASTPtr retExpr = parse_expr_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_print_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_del_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_pass_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_flow_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_import_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_global_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_exec_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_assert_stmt();
    if (retExpr){
        return retExpr;
    }
    return retExpr;
}

//! expr_stmt: testlist (augassign (yield_expr|testlist) |
//!                      ('=' (yield_expr|testlist))*)
ExprASTPtr Parser::parse_expr_stmt(){
    DTRACE(("parse_expr_stmt begin..."));
    ExprASTPtr testlist = parse_testlist();
    if (!testlist){
        DTRACE(("parse_expr_stmt ignore"));
        return NULL;
    }

    ExprASTPtr augassign = parse_augassign();
    if (augassign){
        ExprASTPtr yield_expr = parse_yield_expr();
        if (yield_expr){
            augassign.cast<AugassignAST>()->left  = testlist;
            augassign.cast<AugassignAST>()->right = testlist;
        }
        else{
            ExprASTPtr testlist2 = parse_testlist();
            if (!testlist2){
                THROW_ERROR("parse_expr_stmt failed augassign-2");
            }
            augassign.cast<AugassignAST>()->left  = testlist;
            augassign.cast<AugassignAST>()->right = testlist2;
        }
        return augassign;
    }
    else{
        if (m_curScanner->getToken()->strVal == "="){
            m_curScanner->seek(1);
            
            DMSG(("parse_expr_stmt 2 `=`"));
            
            ExprASTPtr yield_expr = parse_yield_expr();
            if (yield_expr){
                return new BinaryExprAST("=", testlist, yield_expr);
            }
            ExprASTPtr testlist2  = parse_testlist();
            if (!testlist2){
                THROW_ERROR("parse_expr_stmt failed assign-2");
            }
            
            return new BinaryExprAST("=", testlist, testlist2);
        }
        else{
            THROW_ERROR("parse_expr_stmt failed");
        }
    }
    
    return NULL;
}

//! augassign: ('+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' |
//!             '<<=' | '>>=' | '**=' | '//=')
ExprASTPtr Parser::parse_augassign(){
    if (m_curScanner->getToken()->strVal == "+=" ||
        m_curScanner->getToken()->strVal == "-=" ||
        m_curScanner->getToken()->strVal == "*=" ||
        m_curScanner->getToken()->strVal == "/=" ||
        m_curScanner->getToken()->strVal == "%=" ||
        m_curScanner->getToken()->strVal == "&=" ||
        m_curScanner->getToken()->strVal == "|=" ||
        m_curScanner->getToken()->strVal == "^=" ||
        m_curScanner->getToken()->strVal == "<<=" ||
        m_curScanner->getToken()->strVal == ">>=" ||
        m_curScanner->getToken()->strVal == "**=" ||
        m_curScanner->getToken()->strVal == "//=")
    {
        m_curScanner->seek(1);
        return new AugassignAST(m_curScanner->getToken()->strVal, NULL, NULL);
    }
    return NULL;
}

//! print_stmt: 'print' ( [ test (',' test)* [','] ] |
//!                       '>>' test [ (',' test)+ [','] ] )
ExprASTPtr Parser::parse_print_stmt(){
    if (m_curScanner->getToken()->strVal == "print"){
        m_curScanner->seek(1);
        
        if (m_curScanner->getToken()->strVal != ">>"){
            ExprASTPtr test = parse_test();
            if (!test){
                THROW_ERROR("test needed when parse print");
            }
            PrintAST* printAst = new PrintAST();
            ExprASTPtr ret = printAst;
            printAst->exprs.push_back(test);
            
            while (m_curScanner->getToken()->strVal == ","){
                m_curScanner->seek(1);
                test = parse_test();
                if (!test){
                    break;
                }
                
                printAst->exprs.push_back(test);
            }
            
            return ret;
        }
        else{
            m_curScanner->seek(1);
            ExprASTPtr test = parse_test();
        }
        
    }
    return NULL;
}

//! del_stmt: 'del' exprlist
ExprASTPtr Parser::parse_del_stmt(){
    if (m_curScanner->getToken()->strVal == "del"){
        m_curScanner->seek(1);
        
        ExprASTPtr exprlist = parse_exprlist();
        if (!exprlist){
            THROW_ERROR("exprlist needed when parse del");
        }
        return new DelAST(exprlist);
    }
    return NULL;
}

//! pass_stmt: 'pass'
ExprASTPtr Parser::parse_pass_stmt(){
    if (m_curScanner->getToken()->strVal == "pass"){
        m_curScanner->seek(1);
        return new PassAST();
    }
    return NULL;
}

//! flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt | yield_stmt
ExprASTPtr Parser::parse_flow_stmt(){
    ExprASTPtr retExpr = parse_break_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_continue_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_return_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_raise_stmt();
    if (retExpr){
        return retExpr;
    }
    retExpr = parse_yield_stmt();
    if (retExpr){
        return retExpr;
    }
    return NULL;
}

//! break_stmt: 'break'
ExprASTPtr Parser::parse_break_stmt(){
    const Token* token = m_curScanner->getToken();
    if (token->strVal == "break"){
        m_curScanner->seek(1);
        return new BreakAST();
    }
    return NULL;
}

//! continue_stmt: 'continue'
ExprASTPtr Parser::parse_continue_stmt(){
    if (m_curScanner->getToken()->strVal == "continue"){
        m_curScanner->seek(1);
        return new ContinueAST();
    }
    return NULL;
}

//! return_stmt: 'return' [testlist]
ExprASTPtr Parser::parse_return_stmt(){
    if (m_curScanner->getToken()->strVal == "return"){
        m_curScanner->seek(1);
        
        ExprASTPtr testlist = parse_testlist();
        return new ReturnAST(testlist);
    }
    return NULL;
}

//! yield_stmt: yield_expr
ExprASTPtr Parser::parse_yield_stmt(){
    ExprASTPtr yield_expr = parse_yield_expr();
    return yield_expr;
}

//! raise_stmt: 'raise' [test [',' test [',' test]]]
ExprASTPtr Parser::parse_raise_stmt(){
    if (m_curScanner->getToken()->strVal == "raise"){
        m_curScanner->seek(1);
        
        RaiseAST* raiseAST = new RaiseAST();
        ExprASTPtr ret     = raiseAST;
        ExprASTPtr test    = parse_test();
        
        if (test){
            raiseAST->exprs.push_back(test);
            
            if (m_curScanner->getToken()->strVal == ","){
                m_curScanner->seek(1);
                test = parse_test();
                if (!test){
                    THROW_ERROR("test needed when parse raise after ,");
                }
                
                raiseAST->exprs.push_back(test);
                if (m_curScanner->getToken()->strVal == ","){
                    m_curScanner->seek(1);
                    test = parse_test();
                    if (!test){
                        THROW_ERROR("test needed when parse raise after second ,");
                    }
                    raiseAST->exprs.push_back(test);
                }
            }
        }
        return ret;
    }
    return NULL;
}

//! import_stmt: import_name | import_from
ExprASTPtr Parser::parse_import_stmt(){
    ExprASTPtr ret = parse_import_name();
    if (ret){
        return ret;
    }
    ret = parse_import_from();
    return ret;
}

//! import_name: 'import' dotted_as_names
ExprASTPtr Parser::parse_import_name(){
    if (m_curScanner->getToken()->strVal == "import"){
        m_curScanner->seek(1);

        ExprASTPtr dotted_as_names = parse_dotted_as_names();
        return new ImportAST();
    }
    
    return NULL;
}

//! import_from: ('from' ('.'* dotted_name | '.'+)
//!               'import' ('*' | '(' import_as_names ')' | import_as_names))
ExprASTPtr Parser::parse_import_from(){
    return NULL;
}

//! import_as_name: NAME ['as' NAME]
ExprASTPtr Parser::parse_import_as_name(){
    return NULL;
}

//! dotted_as_name: dotted_name ['as' NAME]
ExprASTPtr Parser::parse_dotted_as_name(){
    return NULL;
}

//! import_as_names: import_as_name (',' import_as_name)* [',']
ExprASTPtr Parser::parse_import_as_names(){
    return NULL;
}

//! dotted_as_names: dotted_as_name (',' dotted_as_name)*
ExprASTPtr Parser::parse_dotted_as_names(){
    ExprASTPtr dotted_as_name = parse_dotted_as_name();
    if (!dotted_as_name){
        return NULL;
    }
    while (m_curScanner->getToken()->strVal == ","){
        m_curScanner->seek(1);
        dotted_as_name = parse_dotted_as_name();
    }
    return dotted_as_name;
}

//! dotted_name: NAME ('.' NAME)*
ExprASTPtr Parser::parse_dotted_name(){
    return NULL;
}

//! global_stmt: 'global' NAME (',' NAME)*
ExprASTPtr Parser::parse_global_stmt(){
    return NULL;
}

//! exec_stmt: 'exec' expr ['in' test [',' test]]
ExprASTPtr Parser::parse_exec_stmt(){
    return NULL;
}

//! assert_stmt: 'assert' test [',' test]
ExprASTPtr Parser::parse_assert_stmt(){
    return NULL;
}

//! compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt | funcdef | classdef | decorated
ExprASTPtr Parser::parse_compound_stmt(){
    return NULL;
}

//! if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ['else' ':' suite]
ExprASTPtr Parser::parse_if_stmt(){
    return NULL;
}

//! while_stmt: 'while' test ':' suite ['else' ':' suite]
ExprASTPtr Parser::parse_while_stmt(){
    return NULL;
}

//! for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
ExprASTPtr Parser::parse_for_stmt(){
    return NULL;
}

//! try_stmt: ('try' ':' suite
//!            ((except_clause ':' suite)+
//!             ['else' ':' suite]
//!             ['finally' ':' suite] |
//!            'finally' ':' suite))
ExprASTPtr Parser::parse_try_stmt(){
    return NULL;
}

//! with_stmt: 'with' with_item (',' with_item)*  ':' suite
ExprASTPtr Parser::parse_with_stmt(){
    return NULL;
}

//! with_item: test ['as' expr]
ExprASTPtr Parser::parse_with_item(){
    return NULL;
}

//! except_clause: 'except' [test [('as' | ',') test]]
ExprASTPtr Parser::parse_except_clause(){
    return NULL;
}

//! suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
ExprASTPtr Parser::parse_suite(){
    return NULL;
}

//! testlist_safe: old_test [(',' old_test)+ [',']]
ExprASTPtr Parser::parse_testlist_safe(){
    return NULL;
}

//! old_test: or_test | old_lambdef
ExprASTPtr Parser::parse_old_test(){
    return NULL;
}

//! old_lambdef: 'lambda' [varargslist] ':' old_test
ExprASTPtr Parser::parse_old_lambdef(){
    return NULL;
}

//! test: or_test ['if' or_test 'else' test] | lambdef
ExprASTPtr Parser::parse_test(){
    ExprASTPtr or_test = parse_or_test();
    return or_test;
}

//! or_test: and_test ('or' and_test)*
ExprASTPtr Parser::parse_or_test(){
    ExprASTPtr and_test = parse_and_test();
    return and_test;
}

//! and_test: not_test ('and' not_test)*
ExprASTPtr Parser::parse_and_test(){
    ExprASTPtr not_test = parse_not_test();
    return not_test;
}

//! not_test: 'not' not_test | comparison
ExprASTPtr Parser::parse_not_test(){
    ExprASTPtr comparison = parse_comparison();
    return comparison;
}

//! comparison: expr (comp_op expr)*
ExprASTPtr Parser::parse_comparison(){
    ExprASTPtr expr = parse_expr();
    return expr;
}

//! comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is' 'not'
ExprASTPtr Parser::parse_comp_op(){
    return NULL;
}

//! expr: xor_expr ('|' xor_expr)*
ExprASTPtr Parser::parse_expr(){
    ExprASTPtr xor_expr = parse_xor_expr();
    return xor_expr;
}

//! xor_expr: and_expr ('^' and_expr)*
ExprASTPtr Parser::parse_xor_expr(){
    ExprASTPtr and_expr = parse_and_expr();
    return and_expr;
}

//! and_expr: shift_expr ('&' shift_expr)*
ExprASTPtr Parser::parse_and_expr(){
    ExprASTPtr shift_expr = parse_shift_expr();
    return shift_expr;
}

//! shift_expr: arith_expr (('<<'|'>>') arith_expr)*
ExprASTPtr Parser::parse_shift_expr(){
    ExprASTPtr arith_expr = parse_arith_expr();
    return arith_expr;
}

//! arith_expr: term (('+'|'-') term)*
ExprASTPtr Parser::parse_arith_expr(){
    ExprASTPtr term = parse_term();
    return term;
}

//! term: factor (('*'|'/'|'%'|'//') factor)*
ExprASTPtr Parser::parse_term(){
    ExprASTPtr factor = parse_factor();
    return factor;
}

//! factor: ('+'|'-'|'~') factor | power
ExprASTPtr Parser::parse_factor(){
    ExprASTPtr power = parse_power();
    return power;
}

//! power: atom trailer* ['**' factor]
ExprASTPtr Parser::parse_power(){
    ExprASTPtr atom = parse_atom();
    return atom;
}

//! atom: ('(' [yield_expr|testlist_comp] ')' |
//!        '[' [listmaker] ']' |
//!        '{' [dictorsetmaker] '}' |
//!        '`' testlist1 '`' |
//!        NAME | NUMBER | STRING+)
ExprASTPtr Parser::parse_atom(){
    DMSG(("parse_atom %s", m_curScanner->getToken()->dump().c_str()));
    
    ExprASTPtr retExpr;
    if (m_curScanner->getToken()->nTokenType == TOK_INT){
        retExpr = new NumberExprAST(m_curScanner->getToken()->nVal);
    }
    else if (m_curScanner->getToken()->nTokenType == TOK_FLOAT){
        retExpr = new FloatExprAST(m_curScanner->getToken()->fVal);
    }
    else if (m_curScanner->getToken()->nTokenType == TOK_STR){
        retExpr = new StrExprAST(m_curScanner->getToken()->strVal);
    }
    else if (m_curScanner->getToken()->nTokenType == TOK_VAR){
        if (singleton_t<PyHelper>::instance_ptr()->isKeyword(m_curScanner->getToken()->strVal)){
            return NULL;
        }
        retExpr = new VariableExprAST(m_curScanner->getToken()->strVal);
    }
    else{
        return retExpr;
    }
    m_curScanner->seek(1);
    return retExpr;
}

//! listmaker: test ( list_for | (',' test)* [','] )
ExprASTPtr Parser::parse_listmaker(){
    return NULL;
}

//! testlist_comp: test ( comp_for | (',' test)* [','] )
ExprASTPtr Parser::parse_testlist_comp(){
    return NULL;
}

//! lambdef: 'lambda' [varargslist] ':' test
ExprASTPtr Parser::parse_lambdef(){
    return NULL;
}

//! trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME
ExprASTPtr Parser::parse_trailer(){
    return NULL;
}

//! subscriptlist: subscript (',' subscript)* [',']
ExprASTPtr Parser::parse_subscriptlist(){
    return NULL;
}

//! subscript: '.' '.' '.' | test | [test] ':' [test] [sliceop]
ExprASTPtr Parser::parse_subscript(){
    return NULL;
}

//! sliceop: ':' [test]
ExprASTPtr Parser::parse_sliceop(){
    return NULL;
}

//! exprlist: expr (',' expr)* [',']
ExprASTPtr Parser::parse_exprlist(){
    ExprASTPtr expr = parse_expr();
    if (!expr){
        return NULL;
    }

    StmtAST* stmtAST = new StmtAST();
    stmtAST->exprs.push_back(expr);
    ExprASTPtr ret   = stmtAST;

    const Token* token = m_curScanner->getToken();
    while (token->strVal == ","){
        m_curScanner->seek(1);
        expr = parse_expr();
        if (!expr){
            break;
        }
        stmtAST->exprs.push_back(expr);
        token = m_curScanner->getToken();
    }
    return ret;
}

//! testlist: test (',' test)* [',']
ExprASTPtr Parser::parse_testlist(){
    ExprASTPtr retExpr = parse_test();
    return retExpr;
}

//! dictorsetmaker: ( (test ':' test (comp_for | (',' test ':' test)* [','])) |
//!                   (test (comp_for | (',' test)* [','])) )
ExprASTPtr Parser::parse_dictorsetmaker(){
    return NULL;
}

//! classdef: 'class' NAME ['(' [testlist] ')'] ':' suite
ExprASTPtr Parser::parse_classdef(){
    return NULL;
}

//! arglist: (argument ',')* (argument [',']
//!                          |'*' test (',' argument)* [',' '**' test] 
//!                          |'**' test)
ExprASTPtr Parser::parse_arglist(){
    return NULL;
}

//! argument: test [comp_for] | test '=' test
ExprASTPtr Parser::parse_argument(){
    return NULL;
}

//! list_iter: list_for | list_if
ExprASTPtr Parser::parse_list_iter(){
    return NULL;
}

//! list_for: 'for' exprlist 'in' testlist_safe [list_iter]
ExprASTPtr Parser::parse_list_for(){
    return NULL;
}

//! list_if: 'if' old_test [list_iter]
ExprASTPtr Parser::parse_list_if(){
    return NULL;
}

//! comp_iter: comp_for | comp_if
ExprASTPtr Parser::parse_comp_iter(){
    return NULL;
}

//! comp_for: 'for' exprlist 'in' or_test [comp_iter]
ExprASTPtr Parser::parse_comp_for(){
    return NULL;
}

//! comp_if: 'if' old_test [comp_iter]
ExprASTPtr Parser::parse_comp_if(){
    return NULL;
}

//! testlist1: test (',' test)*
ExprASTPtr Parser::parse_testlist1(){
    ExprASTPtr test = parse_test();
    if (!test){
        return NULL;
    }
    StmtAST* stmp = new StmtAST();
    ExprASTPtr ret= stmp;
    
    stmp->exprs.push_back(test);
    
    while (m_curScanner->getToken()->strVal == ","){
        m_curScanner->seek(1);
        
        test = parse_test();
        if (!test){
            THROW_ERROR("test expr needed when parse testlist1 after ,");
        }
        stmp->exprs.push_back(test);
    }
    return NULL;
}

//! encoding_decl: NAME
ExprASTPtr Parser::parse_encoding_decl(){
    return NULL;
}

//! yield_expr: 'yield' [testlist]
ExprASTPtr Parser::parse_yield_expr(){
    if (m_curScanner->getToken()->strVal == "yield"){
        THROW_ERROR("yield not supported");
    }
    
    return NULL;
}

void Parser::throwError(const string& err, int nLine){
    char msg[256] = {0};
    snprintf(msg, sizeof(msg), "%s(%d)", err.c_str(), nLine);
    throw PyException::buildException(msg);
}

