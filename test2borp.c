#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "borp_value.h"
#include "borp_array.h"
#include "borp_table.h"
#include "borp_helpers.h"

enum types {
ANY = 0,
NUMBER = 1,
STRING = 2,
BOOLEAN = 3,
DOUBLE = 4,
FLOAT = 5,
TABLE = 6,
ENUM = 7,
ARRAY = 8,
INTEGER = 9};
enum astTypes {
NUMBER_EXPR = 0,
STRING_EXPR = 1,
BOOL_EXPR = 2,
NULL_EXPR = 3,
IDENTIFIER_EXPR = 4,
TABLE_EXPR = 5,
ARRAY_EXPR = 6,
BINARY_EXPR = 7,
UNARY_EXPR = 8,
CALL_EXPR = 9,
INDEX_EXPR = 10,
ASSIGN_EXPR = 11,
INDEX_ASSIGN_EXPR = 12,
ENUM_EXPR = 13};
enum statementTypes {
IF_STATEMENT = 0,
BLOCK_STATEMENT = 1,
EXPRESSION_STATEMENT = 2,
RETURN_STATEMENT = 3,
FUNCTION_STATEMENT = 4,
WHILE_STATEMENT = 5,
FOR_STATEMENT = 6,
VARDECLARE_STATEMENT = 7,
BREAK_STATEMENT = 8,
IMPORT_STATEMENT = 9};
enum tokenTypes {
TOKEN_NUMBER = 0,
TOKEN_STRING = 1,
TOKEN_IDENTIFIER = 2,
TOKEN_LBRACE = 3,
TOKEN_RBRACE = 4,
TOKEN_ASSIGN = 5,
TOKEN_EQUALS = 6,
TOKEN_NOT_EQUAL = 7,
TOKEN_GREATER = 8,
TOKEN_GREATER_EQUAL = 9,
TOKEN_LESSER = 10,
TOKEN_LESSER_EQUAL = 11,
TOKEN_PLUS = 12,
TOKEN_MINUS = 13,
TOKEN_STAR = 14,
TOKEN_SLASH = 15,
TOKEN_SEMICOLON = 16,
TOKEN_COLON = 17,
TOKEN_LPAREN = 18,
TOKEN_RPAREN = 19,
TOKEN_COMMA = 20,
TOKEN_IF = 21,
TOKEN_THEN = 22,
TOKEN_ELSE = 23,
TOKEN_ELSEIF = 24,
TOKEN_END = 25,
TOKEN_END_OF_FILE = 26,
TOKEN_NEWLINE = 27,
TOKEN_AND = 28,
TOKEN_OR = 29,
TOKEN_FUNCTION = 30,
TOKEN_RETURN = 31,
TOKEN_DOTDOT = 32,
TOKEN_LAZY = 33,
TOKEN_INHERIT = 34,
TOKEN_WHILE = 35,
TOKEN_DO = 36,
TOKEN_LBRACKET = 37,
TOKEN_RBRACKET = 38,
TOKEN_DOT = 39,
TOKEN_BREAK = 40,
TOKEN_TRUE = 41,
TOKEN_FALSE = 42,
TOKEN_FOR = 43,
TOKEN_IMPORT = 44,
TOKEN_LOCAL = 45,
TOKEN_PRIVATE = 46,
TOKEN_OPPOSE = 47,
TOKEN_NULL = 48,
TOKEN_QUESTION = 49};

BorpTable* keywords;

BorpTable* makeNumberExpr(double val, double ln);
BorpTable* makeStringExpr(const char* val, double ln);
BorpTable* makeIdentifierExpr(const char* val, double ln);
BorpTable* makeBooleanExpr(bool val, double ln);
BorpTable* makeNullExpr(double ln);
BorpTable* makeEnumExpr(BorpArray* membs, double ln);
BorpTable* makeTableExpr(BorpArray* f, double ln);
BorpTable* makeArrayExpr(BorpArray* el, double ln);
BorpTable* makeIndexExpr(BorpTable* tar, BorpTable* k, double ln);
BorpTable* makeCallExpr(BorpTable* ce, BorpArray* args, double ln);
BorpTable* makeUnaryExpr(double tkType, BorpTable* op, double ln);
BorpTable* makeBinaryExpr(double tkType, BorpTable* le, BorpTable* ri, double ln);
BorpTable* makeAssignExpr(const char* n, BorpTable* val, double ln);
BorpTable* makeIndexAssignExpr(BorpTable* t, BorpTable* k, BorpTable* val, double ln);
BorpTable* makeExpressionStatement(BorpValue expr, double ln);
BorpTable* makeBlockStatement(BorpValue stmnts, double ln);
BorpTable* makeIfStatement(BorpTable* cond, BorpValue thenB, BorpValue elseB, double ln);
BorpTable* makeVarDeclareStatement(const char* na, BorpTable* init, double typeAnno, bool isPriv, double ln);
BorpTable* makeReturnStatement(BorpTable* val, double ln);
BorpTable* makeFunctionStatement(const char* n, BorpArray* par, BorpArray* bo, const char* pName, BorpArray* pArgs, bool hReturn, double rType, bool isLa, bool isPriv, double ln);
BorpTable* makeForStatement(BorpTable* cl1, BorpTable* cl2, BorpValue cl3, bool cStyle, BorpArray* bo, double ln);
BorpTable* makeWhileStatement(BorpTable* cond, BorpArray* bo, double ln);
BorpTable* makeBreakStatement(double ln);
BorpTable* makeImportStatement(const char* pa, double ln);
BorpTable* lexerState(const char* src);
static bool containsChar(const char* haystack, const char* needle);
static bool isAlpha(const char* c);
static bool isAlnum(const char* c);
bool isAtEnd(BorpTable* state);
BorpValue peek(BorpTable* state);
BorpValue peekNext(BorpTable* state);
const char* read(BorpTable* state);
void move(BorpTable* state);
static BorpTable* makeToken(BorpTable* state, BorpValue tokenType, BorpValue val);
static BorpTable* makeStringToken(BorpTable* state, const char* quoteChar);
static BorpTable* makeNumberToken(BorpTable* state);
static BorpTable* scanIdentifierOrKeyword(BorpValue state);
BorpArray* tokenize(BorpValue state);
BorpValue parseState(BorpArray* tokens);
BorpValue parseProgram(BorpValue state);
bool check(BorpTable* state, double expectedType);
BorpValue match(BorpTable* state, double expectedType);
BorpValue matchMulti(BorpTable* state, BorpArray* expectedTypes);
BorpValue consume(BorpTable* state, double expectedType, const char* errorMsg);
BorpTable* advance(BorpTable* state);
static BorpValue expect(BorpTable* state, double expectedType, const char* errorMsg);
static BorpValue expression(BorpTable* state);
static double parseTypeAnnotation(BorpTable* state);
static double parseBaseType(BorpTable* state);
static BorpTable* desugarEnumTable(BorpTable* tableExpr);
static BorpValue statementList(BorpTable* state, BorpArray* tokens);
static BorpValue statement(BorpTable* state);
static BorpValue importStatement(BorpTable* state);
static BorpValue localStatement(BorpTable* state, BorpValue isPrivate);
static BorpValue breakStatement(BorpTable* state);
static BorpValue whileStatement(BorpTable* state);
static BorpValue forStatement(BorpTable* state);
static bool hasReturnStatement(BorpArray* body);
static BorpValue functionStatement(BorpTable* state, BorpValue isPrivate);
static BorpValue returnStatement(BorpTable* state);
static BorpValue varDeclareStatement(BorpTable* state, BorpValue isPrivate);
static BorpValue ifStatement(BorpTable* state);
static BorpValue blockStatement(BorpTable* state);
static BorpValue expressionStatement(BorpTable* state);
static BorpValue logicalAnd(BorpTable* state);
static BorpValue logicalOr(BorpTable* state);
static BorpValue equality(BorpTable* state);
static BorpValue relational(BorpTable* state);
static BorpValue additive(BorpTable* state);
static BorpValue multiplicative(BorpTable* state);
static BorpValue unary(BorpTable* state);
static BorpValue assignment(BorpTable* state);
static BorpTable* primaryBase(BorpTable* state);
static BorpValue primary(BorpTable* state);
static BorpValue finishCall(BorpTable* state, BorpValue callee, BorpValue line);
static const char* asStr(BorpTable* e);
static bool isMathematical(double tokenType);
static bool isComparative(double tokenType);
static bool isEnumRef(BorpTable* target, BorpTable* env);
static BorpTable* createEnv(BorpTable* parentEnv);
static void defineEnv(BorpTable* env, BorpValue name, double type);
static BorpValue lookupEnv(BorpValue env, BorpValue name);
static BorpTable* lookupFunc(BorpValue env, BorpValue name);
static double resolveExpr(BorpTable* expr, BorpTable* env);
static void resolveStatement(BorpValue stmt, BorpValue env);
static void resolveStatementList(BorpArray* stmts, BorpTable* env);
static void defineBuiltin(BorpTable* env, const char* name, const char* cName, double retType, BorpArray* paramTypes);
void resolveProgram(BorpArray* statements);
static const char* escapeC(const char* s);
static const char* boxValue(BorpTable* expr);
static const char* coerceTo(BorpTable* expr, double targetType);
static const char* provFunctions(const char* fnName, BorpTable* expr);
static const char* cType(double typeAnno);
static const char* dirOf(const char* path);
static BorpArray* expandImports(BorpArray* statements, const char* baseDir, BorpTable* visited);
static const char* generateHeaders(BorpTable* statement);
static const char* genExpr(BorpTable* expr);
static const char* genStatementList(BorpArray* statements, double fnReturnType);
static const char* genStatement(BorpTable* statement, double fnReturnType);
static const char* genFunction(BorpTable* statement);
static const char* generateProgram(BorpArray* statements);
void testCompiler(void);

BorpTable* makeNumberExpr(double val, double ln) {
BorpTable* numberExpr = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){borp_val_num(NUMBER_EXPR), borp_val_num(val), borp_val_num(ln)});
return numberExpr;
}

BorpTable* makeStringExpr(const char* val, double ln) {
BorpTable* stringExpr = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){borp_val_num(STRING_EXPR), borp_val_string(val), borp_val_num(ln)});
return stringExpr;
}

BorpTable* makeIdentifierExpr(const char* val, double ln) {
BorpTable* identifierExpr = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){borp_val_num(IDENTIFIER_EXPR), borp_val_string(val), borp_val_num(ln)});
return identifierExpr;
}

BorpTable* makeBooleanExpr(bool val, double ln) {
BorpTable* booleanExpr = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){borp_val_num(BOOL_EXPR), borp_val_boolean(val), borp_val_num(ln)});
return booleanExpr;
}

BorpTable* makeNullExpr(double ln) {
BorpTable* nullExpr = borp_table_from_entries(2, (const char*[]){"type", "line"}, (BorpValue[]){borp_val_num(NULL_EXPR), borp_val_num(ln)});
return nullExpr;
}

BorpTable* makeEnumExpr(BorpArray* membs, double ln) {
BorpTable* enumExpression = borp_table_from_entries(3, (const char*[]){"type", "members", "line"}, (BorpValue[]){borp_val_num(ENUM_EXPR), borp_val_array(membs), borp_val_num(ln)});
return enumExpression;
}

BorpTable* makeTableExpr(BorpArray* f, double ln) {
BorpTable* tableExpr = borp_table_from_entries(3, (const char*[]){"type", "fields", "line"}, (BorpValue[]){borp_val_num(TABLE_EXPR), borp_val_array(f), borp_val_num(ln)});
return tableExpr;
}

BorpTable* makeArrayExpr(BorpArray* el, double ln) {
BorpTable* arrayExpr = borp_table_from_entries(3, (const char*[]){"type", "elements", "line"}, (BorpValue[]){borp_val_num(ARRAY_EXPR), borp_val_array(el), borp_val_num(ln)});
return arrayExpr;
}

BorpTable* makeIndexExpr(BorpTable* tar, BorpTable* k, double ln) {
BorpTable* indexExpr = borp_table_from_entries(4, (const char*[]){"type", "target", "key", "line"}, (BorpValue[]){borp_val_num(INDEX_EXPR), borp_val_table(tar), borp_val_table(k), borp_val_num(ln)});
return indexExpr;
}

BorpTable* makeCallExpr(BorpTable* ce, BorpArray* args, double ln) {
BorpTable* callExpr = borp_table_from_entries(4, (const char*[]){"type", "callee", "arguments", "line"}, (BorpValue[]){borp_val_num(CALL_EXPR), borp_val_table(ce), borp_val_array(args), borp_val_num(ln)});
return callExpr;
}

BorpTable* makeUnaryExpr(double tkType, BorpTable* op, double ln) {
BorpTable* unaryExpr = borp_table_from_entries(4, (const char*[]){"type", "tokenType", "operand", "line"}, (BorpValue[]){borp_val_num(UNARY_EXPR), borp_val_num(tkType), borp_val_table(op), borp_val_num(ln)});
return unaryExpr;
}

BorpTable* makeBinaryExpr(double tkType, BorpTable* le, BorpTable* ri, double ln) {
BorpTable* binaryExpr = borp_table_from_entries(5, (const char*[]){"type", "tokenType", "left", "right", "line"}, (BorpValue[]){borp_val_num(BINARY_EXPR), borp_val_num(tkType), borp_val_table(le), borp_val_table(ri), borp_val_num(ln)});
return binaryExpr;
}

BorpTable* makeAssignExpr(const char* n, BorpTable* val, double ln) {
BorpTable* assignExpr = borp_table_from_entries(4, (const char*[]){"type", "name", "value", "line"}, (BorpValue[]){borp_val_num(ASSIGN_EXPR), borp_val_string(n), borp_val_table(val), borp_val_num(ln)});
return assignExpr;
}

BorpTable* makeIndexAssignExpr(BorpTable* t, BorpTable* k, BorpTable* val, double ln) {
BorpTable* indexAssignExpr = borp_table_from_entries(5, (const char*[]){"type", "target", "key", "value", "line"}, (BorpValue[]){borp_val_num(INDEX_ASSIGN_EXPR), borp_val_table(t), borp_val_table(k), borp_val_table(val), borp_val_num(ln)});
return indexAssignExpr;
}

BorpTable* makeExpressionStatement(BorpValue expr, double ln) {
BorpTable* expressionStatement = borp_table_from_entries(3, (const char*[]){"type", "expression", "line"}, (BorpValue[]){borp_val_num(EXPRESSION_STATEMENT), expr, borp_val_num(ln)});
return expressionStatement;
}

BorpTable* makeBlockStatement(BorpValue stmnts, double ln) {
BorpTable* blockStatement = borp_table_from_entries(3, (const char*[]){"type", "statements", "line"}, (BorpValue[]){borp_val_num(BLOCK_STATEMENT), stmnts, borp_val_num(ln)});
return blockStatement;
}

BorpTable* makeIfStatement(BorpTable* cond, BorpValue thenB, BorpValue elseB, double ln) {
BorpTable* ifStatement = borp_table_from_entries(5, (const char*[]){"type", "condition", "thenBranch", "elseBranch", "line"}, (BorpValue[]){borp_val_num(IF_STATEMENT), borp_val_table(cond), thenB, elseB, borp_val_num(ln)});
return ifStatement;
}

BorpTable* makeVarDeclareStatement(const char* na, BorpTable* init, double typeAnno, bool isPriv, double ln) {
BorpTable* varDeclareStatement = borp_table_from_entries(6, (const char*[]){"type", "name", "initializer", "typeAnnotation", "isPrivate", "line"}, (BorpValue[]){borp_val_num(VARDECLARE_STATEMENT), borp_val_string(na), borp_val_table(init), borp_val_num(typeAnno), borp_val_boolean(isPriv), borp_val_num(ln)});
return varDeclareStatement;
}

BorpTable* makeReturnStatement(BorpTable* val, double ln) {
BorpTable* returnStatement = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){borp_val_num(RETURN_STATEMENT), borp_val_table(val), borp_val_num(ln)});
return returnStatement;
}

BorpTable* makeFunctionStatement(const char* n, BorpArray* par, BorpArray* bo, const char* pName, BorpArray* pArgs, bool hReturn, double rType, bool isLa, bool isPriv, double ln) {
BorpTable* functionStatement = borp_table_from_entries(11, (const char*[]){"type", "name", "parameters", "body", "parentName", "parentArgs", "hasReturn", "returnType", "isLazy", "isPrivate", "line"}, (BorpValue[]){borp_val_num(FUNCTION_STATEMENT), borp_val_string(n), borp_val_array(par), borp_val_array(bo), borp_val_string(pName), borp_val_array(pArgs), borp_val_boolean(hReturn), borp_val_num(rType), borp_val_boolean(isLa), borp_val_boolean(isPriv), borp_val_num(ln)});
return functionStatement;
}

BorpTable* makeForStatement(BorpTable* cl1, BorpTable* cl2, BorpValue cl3, bool cStyle, BorpArray* bo, double ln) {
BorpTable* forStatement = borp_table_from_entries(7, (const char*[]){"type", "clause1", "clause2", "clause3", "isCStyle", "body", "line"}, (BorpValue[]){borp_val_num(FOR_STATEMENT), borp_val_table(cl1), borp_val_table(cl2), cl3, borp_val_boolean(cStyle), borp_val_array(bo), borp_val_num(ln)});
return forStatement;
}

BorpTable* makeWhileStatement(BorpTable* cond, BorpArray* bo, double ln) {
BorpTable* whileStatement = borp_table_from_entries(4, (const char*[]){"type", "condition", "body", "line"}, (BorpValue[]){borp_val_num(WHILE_STATEMENT), borp_val_table(cond), borp_val_array(bo), borp_val_num(ln)});
return whileStatement;
}

BorpTable* makeBreakStatement(double ln) {
BorpTable* breakStatement = borp_table_from_entries(2, (const char*[]){"type", "line"}, (BorpValue[]){borp_val_num(BREAK_STATEMENT), borp_val_num(ln)});
return breakStatement;
}

BorpTable* makeImportStatement(const char* pa, double ln) {
BorpTable* importStatement = borp_table_from_entries(3, (const char*[]){"type", "path", "line"}, (BorpValue[]){borp_val_num(IMPORT_STATEMENT), borp_val_string(pa), borp_val_num(ln)});
return importStatement;
}

BorpTable* lexerState(const char* src) {
BorpTable* state = borp_table_from_entries(3, (const char*[]){"source", "cursor", "line"}, (BorpValue[]){borp_val_string(src), borp_val_num(0), borp_val_num(1)});
return state;
}

static bool containsChar(const char* haystack, const char* needle) {
if ((borp_length(borp_val_string(needle)) == 0)) {
return false;
}
double i = 0;
while ((i < borp_length(borp_val_string(haystack)))) {
if ((strcmp(charAt(haystack, i), needle) == 0)) {
return true;
}
i = (i + 1);
}
return false;
}

static bool isAlpha(const char* c) {
return containsChar("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", c);
}

static bool isAlnum(const char* c) {
return containsChar("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", c);
}

bool isAtEnd(BorpTable* state) {
if (borp_table_get(state, "tokens").type != VAL_NULL) {
if ((borp_to_num(borp_table_get(state, "cursor")) >= borp_length(borp_table_get(state, "tokens")))) {
return true;
}
return (borp_to_num(borp_table_get(borp_index_get(borp_table_get(state, "tokens"), borp_table_get(state, "cursor")).as.borp_table, "type")) == TOKEN_END_OF_FILE);
}
return (borp_to_num(borp_table_get(state, "cursor")) >= borp_length(borp_table_get(state, "source")));
}

BorpValue peek(BorpTable* state) {
if (isAtEnd(state)) {
if (borp_table_get(state, "tokens").type != VAL_NULL) {
return borp_array_get(borp_table_get(state, "tokens").as.borp_array, (int)((borp_length(borp_table_get(state, "tokens")) - 1)));
}
return borp_val_string("");
}
if (borp_table_get(state, "tokens").type != VAL_NULL) {
return borp_index_get(borp_table_get(state, "tokens"), borp_table_get(state, "cursor"));
}
return borp_val_string(charAt(borp_to_str(borp_table_get(state, "source")), (int)borp_to_num(borp_table_get(state, "cursor"))));
}

BorpValue peekNext(BorpTable* state) {
if (borp_table_get(state, "tokens").type != VAL_NULL) {
if (((borp_to_num(borp_table_get(state, "cursor")) + 1) >= borp_length(borp_table_get(state, "tokens")))) {
return borp_array_get(borp_table_get(state, "tokens").as.borp_array, (int)((borp_length(borp_table_get(state, "tokens")) - 1)));
}
return borp_array_get(borp_table_get(state, "tokens").as.borp_array, (int)((borp_to_num(borp_table_get(state, "cursor")) + 1)));
}
if (((borp_to_num(borp_table_get(state, "cursor")) + 1) >= borp_length(borp_table_get(state, "source")))) {
return borp_val_string("");
}
return borp_val_string(charAt(borp_to_str(borp_table_get(state, "source")), (borp_to_num(borp_table_get(state, "cursor")) + 1)));
}

const char* read(BorpTable* state) {
if (isAtEnd(state)) {
return "";
}
return charAt(borp_to_str(borp_table_get(state, "source")), (int)borp_to_num(borp_table_get(state, "cursor")));
}

void move(BorpTable* state) {
borp_table_set(state, "cursor", borp_val_num((borp_to_num(borp_table_get(state, "cursor")) + 1)));
}

static BorpTable* makeToken(BorpTable* state, BorpValue tokenType, BorpValue val) {
BorpTable* Token = borp_table_from_entries(3, (const char*[]){"type", "value", "line"}, (BorpValue[]){tokenType, val, borp_table_get(state, "line")});
return Token;
}

static BorpTable* makeStringToken(BorpTable* state, const char* quoteChar) {
const char* string = "";
while (((! isAtEnd(state)) && (strcmp(borp_to_str(peek(state)), quoteChar) != 0))) {
const char* c = read(state);
move(state);
if ((strcmp(c, "\n") == 0)) {
borp_table_set(state, "line", borp_val_num((borp_to_num(borp_table_get(state, "line")) + 1)));
string = borp_concat(string, c);
} else if (((strcmp(c, "\\") == 0) && (! isAtEnd(state)))) {
const char* esc = read(state);
move(state);
if ((strcmp(esc, "\"") == 0)) {
string = borp_concat(string, "\"");
} else if ((strcmp(esc, "'") == 0)) {
string = borp_concat(string, "'");
} else if ((strcmp(esc, "\\") == 0)) {
string = borp_concat(string, "\\");
} else if ((strcmp(esc, "n") == 0)) {
string = borp_concat(string, "\n");
} else if ((strcmp(esc, "t") == 0)) {
string = borp_concat(string, "\t");
} else if ((strcmp(esc, "r") == 0)) {
string = borp_concat(string, "\r");
} else {
string = borp_concat(string, "\\");
}
} else {
string = borp_concat(string, c);
}
}
if ((! isAtEnd(state))) {
move(state);
}
return makeToken(state, borp_val_num(TOKEN_STRING), borp_val_string(string));
}

static BorpTable* makeNumberToken(BorpTable* state) {
const char* numStr = "";
while (((! isAtEnd(state)) && borp_is_numeric(peek(state)))) {
numStr = borp_concat(numStr, borp_to_text(peek(state)));
move(state);
}
return makeToken(state, borp_val_num(TOKEN_NUMBER), borp_val_num(borp_parse_number(numStr)));
}

static BorpTable* scanIdentifierOrKeyword(BorpValue state) {
const char* identStr = "";
while (((! isAtEnd(borp_to_table(state))) && isAlnum(borp_to_str(peek(borp_to_table(state)))))) {
identStr = borp_concat(identStr, read(borp_to_table(state)));
move(borp_to_table(state));
}
BorpValue type = borp_table_get(keywords, identStr);
if (type.type == VAL_NULL) {
type = borp_val_num(TOKEN_IDENTIFIER);
}
return makeToken(borp_to_table(state), type, borp_val_string(identStr));
}

BorpArray* tokenize(BorpValue state) {
BorpArray* tokens = borp_array_create(0);
while ((! isAtEnd(borp_to_table(state)))) {
const char* c = borp_to_str(peek(borp_to_table(state)));
if ((strcmp(c, "\n") == 0)) {
borp_table_set(state.as.borp_table, "line", borp_val_num((borp_to_num(borp_table_get(state.as.borp_table, "line")) + 1)));
makeToken(borp_to_table(state), borp_val_num(TOKEN_NEWLINE), borp_val_string("\n"));
move(borp_to_table(state));
} else if ((((strcmp(c, " ") == 0) || (strcmp(c, "\t") == 0)) || (strcmp(c, "\r") == 0))) {
move(borp_to_table(state));
} else if ((strcmp(c, ".") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), ".") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_DOTDOT), borp_val_string(".."))));
} else {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_DOT), borp_val_string("."))));
}
} else if (borp_is_numeric(borp_val_string(c))) {
borp_push(tokens, borp_val_table(makeNumberToken(borp_to_table(state))));
} else if (isAlpha(c)) {
borp_push(tokens, borp_val_table(scanIdentifierOrKeyword(state)));
} else if (((strcmp(c, "-") == 0) && (strcmp(borp_to_str(peekNext(borp_to_table(state))), ">") == 0))) {
move(borp_to_table(state));
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_INHERIT), borp_val_string("->"))));
} else if (((strcmp(c, "-") == 0) && (strcmp(borp_to_str(peekNext(borp_to_table(state))), "-") == 0))) {
while (((! isAtEnd(borp_to_table(state))) && (strcmp(borp_to_str(peek(borp_to_table(state))), "\n") != 0))) {
move(borp_to_table(state));
}
} else if (((strcmp(c, "/") == 0) && (strcmp(borp_to_str(peekNext(borp_to_table(state))), "/") == 0))) {
while (((! isAtEnd(borp_to_table(state))) && (strcmp(borp_to_str(peek(borp_to_table(state))), "\n") != 0))) {
move(borp_to_table(state));
}
} else if ((strcmp(c, "=") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "=") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_EQUALS), borp_val_string("=="))));
} else {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_ASSIGN), borp_val_string("="))));
}
} else if (((strcmp(c, "\"") == 0) || (strcmp(c, "'") == 0))) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeStringToken(borp_to_table(state), c)));
} else if (((strcmp(c, "!") == 0) || (strcmp(c, "~") == 0))) {
const char* firstChar = c;
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "=") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_NOT_EQUAL), borp_val_string(borp_concat(firstChar, "=")))));
} else {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_OPPOSE), borp_val_string("!"))));
}
} else if ((strcmp(c, "<") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "=") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_LESSER_EQUAL), borp_val_string("<="))));
} else {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_LESSER), borp_val_string("<"))));
}
} else if ((strcmp(c, ">") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "=") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_GREATER_EQUAL), borp_val_string(">="))));
} else {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_GREATER), borp_val_string(">"))));
}
} else if ((strcmp(c, "&") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "&") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_AND), borp_val_string("&&"))));
}
} else if ((strcmp(c, "|") == 0)) {
move(borp_to_table(state));
if ((strcmp(borp_to_str(peek(borp_to_table(state))), "|") == 0)) {
move(borp_to_table(state));
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_OR), borp_val_string("||"))));
}
} else {
move(borp_to_table(state));
if ((strcmp(c, "{") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_LBRACE), borp_val_string("{"))));
} else if ((strcmp(c, "}") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_RBRACE), borp_val_string("}"))));
} else if ((strcmp(c, "(") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_LPAREN), borp_val_string("("))));
} else if ((strcmp(c, ")") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_RPAREN), borp_val_string(")"))));
} else if ((strcmp(c, ",") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_COMMA), borp_val_string(","))));
} else if ((strcmp(c, "+") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_PLUS), borp_val_string("+"))));
} else if ((strcmp(c, "-") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_MINUS), borp_val_string("-"))));
} else if ((strcmp(c, ";") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_SEMICOLON), borp_val_string(";"))));
} else if ((strcmp(c, "*") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_STAR), borp_val_string("*"))));
} else if ((strcmp(c, "/") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_SLASH), borp_val_string("/"))));
} else if ((strcmp(c, ":") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_COLON), borp_val_string(":"))));
} else if ((strcmp(c, "[") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_LBRACKET), borp_val_string("["))));
} else if ((strcmp(c, "]") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_RBRACKET), borp_val_string("]"))));
} else if ((strcmp(c, "?") == 0)) {
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_QUESTION), borp_val_string("?"))));
}
}
}
borp_push(tokens, borp_val_table(makeToken(borp_to_table(state), borp_val_num(TOKEN_END_OF_FILE), borp_val_string("EOF"))));
return tokens;
}

BorpValue parseState(BorpArray* tokens) {
BorpTable* state = borp_table_from_entries(2, (const char*[]){"tokens", "cursor"}, (BorpValue[]){borp_val_array(tokens), borp_val_num(0)});
return borp_val_table(state);
}

BorpValue parseProgram(BorpValue state) {
BorpArray* statements = borp_array_create(0);
while ((! isAtEnd(borp_to_table(state)))) {
while (check(borp_to_table(state), TOKEN_NEWLINE)) {
advance(borp_to_table(state));
}
if (isAtEnd(borp_to_table(state))) {
break;
}
borp_push(statements, statement(borp_to_table(state)));
}
return borp_val_array(statements);
}

bool check(BorpTable* state, double expectedType) {
if (isAtEnd(state)) {
return false;
}
BorpValue tok = peek(state);
return (borp_to_num(borp_table_get(tok.as.borp_table, "type")) == expectedType);
}

BorpValue match(BorpTable* state, double expectedType) {
if (check(state, expectedType)) {
BorpValue tok = peek(state);
move(state);
return tok;
}
return borp_val_null();
}

BorpValue matchMulti(BorpTable* state, BorpArray* expectedTypes) {
for (double i = 0; i <= (borp_length(borp_val_array(expectedTypes)) - 1); i += 1) {
if (check(state, borp_to_num(borp_array_get(expectedTypes, (int)(i))))) {
move(state);
return borp_val_boolean(true);
}
}
return borp_val_boolean(false);
}

BorpValue consume(BorpTable* state, double expectedType, const char* errorMsg) {
if (check(state, expectedType)) {
BorpValue tok = peek(state);
move(state);
return tok;
}
BorpValue tok = peek(state);
borp_print_val(borp_val_string(borp_concat(borp_concat(borp_concat("Parser Error on line ", borp_to_text(borp_table_get(tok.as.borp_table, "line"))), ": "), errorMsg)));
return borp_val_null();
}

BorpTable* advance(BorpTable* state) {
BorpTable* token = borp_to_table(peek(state));
move(state);
return token;
}

static BorpValue expect(BorpTable* state, double expectedType, const char* errorMsg) {
return consume(state, expectedType, errorMsg);
}

static BorpValue expression(BorpTable* state) {
return assignment(state);
}

static double parseTypeAnnotation(BorpTable* state) {
double baseType = parseBaseType(state);
if (borp_to_bool(match(state, TOKEN_QUESTION))) {
if ((((((baseType == NUMBER) || (baseType == INTEGER)) || (baseType == DOUBLE)) || (baseType == FLOAT)) || (baseType == BOOLEAN))) {
return ANY;
}
}
return baseType;
}

static double parseBaseType(BorpTable* state) {
BorpTable* tokenType = borp_to_table(expect(state, TOKEN_IDENTIFIER, "expected type name"));
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "number") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "num") == 0))) {
return NUMBER;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "integer") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "int") == 0))) {
return INTEGER;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "double") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "dbl") == 0))) {
return DOUBLE;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "float") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "flt") == 0))) {
return FLOAT;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "string") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "str") == 0))) {
return STRING;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "bool") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "boolean") == 0))) {
return BOOLEAN;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "table") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "tbl") == 0))) {
return TABLE;
}
if (((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "enum") == 0) || (strcmp(borp_to_str(borp_table_get(tokenType, "value")), "enumeration") == 0))) {
return ENUM;
}
if ((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "any") == 0)) {
return ANY;
}
if ((strcmp(borp_to_str(borp_table_get(tokenType, "value")), "array") == 0)) {
return ARRAY;
}
borp_print_val(borp_val_string(borp_concat(borp_concat(borp_concat(borp_concat("Unknown type ", "'"), borp_to_text(borp_table_get(tokenType, "value"))), "' "), borp_to_text(borp_table_get(tokenType, "line")))));
return (-1);
}

static BorpTable* desugarEnumTable(BorpTable* tableExpr) {
BorpArray* members = borp_array_create(0);
for (double i = 0; i <= (borp_length(borp_table_get(tableExpr, "fields")) - 1); i += 1) {
BorpValue field = borp_array_get(borp_table_get(tableExpr, "fields").as.borp_array, (int)(i));
if ((borp_to_num(borp_table_get(borp_table_get(field.as.borp_table, "value").as.borp_table, "type")) != NUMBER_EXPR)) {
borp_print_val(borp_val_string(borp_concat(borp_concat(borp_concat("parser error: enum member '", borp_to_text(borp_table_get(field.as.borp_table, "key"))), "' must have a constant numeric value on line "), borp_to_text(borp_table_get(borp_table_get(field.as.borp_table, "value").as.borp_table, "line")))));
}
BorpValue numericVal = borp_table_get(borp_table_get(field.as.borp_table, "value").as.borp_table, "value");
borp_push(members, borp_val_table(borp_table_from_entries(2, (const char*[]){"name", "value"}, (BorpValue[]){borp_table_get(field.as.borp_table, "key"), numericVal})));
}
return makeEnumExpr(members, borp_to_num(borp_table_get(tableExpr, "line")));
}

static BorpValue statementList(BorpTable* state, BorpArray* tokens) {
BorpArray* statementList = borp_array_create(0);
while ((! isAtEnd(state))) {
bool atTerminator = false;
for (double i = 0; i <= (borp_length(borp_val_array(tokens)) - 1); i += 1) {
if (check(state, borp_to_num(borp_array_get(tokens, (int)(i))))) {
atTerminator = true;
break;
}
}
if (atTerminator) {
break;
}
borp_push(statementList, statement(state));
}
return borp_val_array(statementList);
}

static BorpValue statement(BorpTable* state) {
if (check(state, TOKEN_PRIVATE)) {
BorpTable* privToken = advance(state);
bool isPrivate = true;
BorpTable* inner = borp_table_create(0);
if (check(state, TOKEN_FUNCTION)) {
inner = borp_to_table(functionStatement(state, borp_val_boolean(isPrivate)));
} else if ((check(state, TOKEN_IDENTIFIER) && (borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_COLON))) {
inner = borp_to_table(varDeclareStatement(state, borp_val_boolean(isPrivate)));
} else {
borp_print_val(borp_val_string(borp_concat("'private' can only be used to modify a function or a typed variable declaration line: ", borp_to_text(borp_table_get(privToken, "line")))));
}
return borp_val_table(inner);
}
bool isPrivate = false;
if (check(state, TOKEN_IMPORT)) {
return importStatement(state);
}
if (check(state, TOKEN_LOCAL)) {
return localStatement(state, borp_val_boolean(true));
}
if ((check(state, TOKEN_IDENTIFIER) && (borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_COLON))) {
return varDeclareStatement(state, borp_val_boolean(isPrivate));
}
if (check(state, TOKEN_WHILE)) {
return whileStatement(state);
}
if (check(state, TOKEN_IF)) {
return ifStatement(state);
}
if (check(state, TOKEN_LBRACE)) {
return blockStatement(state);
}
if (check(state, TOKEN_FUNCTION)) {
return functionStatement(state, borp_val_boolean(isPrivate));
}
if (check(state, TOKEN_RETURN)) {
return returnStatement(state);
}
if (check(state, TOKEN_BREAK)) {
return breakStatement(state);
}
if (check(state, TOKEN_FOR)) {
return forStatement(state);
}
return expressionStatement(state);
}

static BorpValue importStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
BorpValue pathToken = expect(state, TOKEN_STRING, "expected string path after 'import'");
const char* path = borp_to_str(borp_table_get(pathToken.as.borp_table, "value"));
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
return borp_val_table(makeImportStatement(path, line));
}

static BorpValue localStatement(BorpTable* state, BorpValue isPrivate) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
BorpValue nameToken = expect(state, TOKEN_IDENTIFIER, "expected a variable name after 'local'");
const char* name = borp_to_str(borp_table_get(nameToken.as.borp_table, "value"));
double declaredType = ANY;
if (borp_to_bool(match(state, TOKEN_COLON))) {
declaredType = parseTypeAnnotation(state);
}
expect(state, TOKEN_ASSIGN, "expected '=' after variable name in 'local' declaration");
BorpValue initializer = expression(state);
if ((declaredType == ENUM)) {
if ((borp_to_num(borp_table_get(initializer.as.borp_table, "type")) != TABLE_EXPR)) {
borp_print_val(borp_val_string("expected a table literal ('{...}') to initialize an enum"));
}
initializer = borp_val_table(desugarEnumTable(borp_to_table(initializer)));
}
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
if (isPrivate.type == VAL_NULL) {
isPrivate = borp_val_boolean(true);
}
return borp_val_table(makeVarDeclareStatement(name, borp_to_table(initializer), declaredType, borp_to_bool(isPrivate), line));
}

static BorpValue breakStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
return borp_val_table(makeBreakStatement(line));
}

static BorpValue whileStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(peek(state).as.borp_table, "line"));
move(state);
BorpValue condition = expression(state);
BorpArray* body = borp_array_create(0);
if (check(state, TOKEN_LBRACE)) {
move(state);
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)})));
expect(state, TOKEN_RBRACE, "expected '}' to close while body");
} else {
match(state, TOKEN_DO);
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_END)})));
expect(state, TOKEN_END, "expected 'end' to close while loop");
}
return borp_val_table(makeWhileStatement(borp_to_table(condition), body, line));
}

static BorpValue forStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
BorpValue clause1 = expression(state);
expect(state, TOKEN_COMMA, "expected ',' after first for-loop clause");
BorpValue clause2 = expression(state);
bool isCStyle = false;
if ((borp_to_num(borp_table_get(clause2.as.borp_table, "type")) == BINARY_EXPR)) {
BorpValue op = borp_table_get(clause2.as.borp_table, "tokenType");
if (((((((borp_to_num(op) == TOKEN_LESSER) || (borp_to_num(op) == TOKEN_LESSER_EQUAL)) || (borp_to_num(op) == TOKEN_GREATER)) || (borp_to_num(op) == TOKEN_GREATER_EQUAL)) || (borp_to_num(op) == TOKEN_EQUALS)) || (borp_to_num(op) == TOKEN_NOT_EQUAL))) {
isCStyle = true;
}
}
BorpValue clause3 = borp_val_null();
if (isCStyle) {
expect(state, TOKEN_COMMA, "expected ',' before for-loop increment");
clause3 = expression(state);
} else if (borp_to_bool(match(state, TOKEN_COMMA))) {
clause3 = expression(state);
}
BorpArray* body = borp_array_create(0);
if (borp_to_bool(match(state, TOKEN_LBRACE))) {
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)})));
expect(state, TOKEN_RBRACE, "expected '}' to close for-loop body");
} else {
match(state, TOKEN_DO);
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_END)})));
expect(state, TOKEN_END, "expected 'end' to close for-loop");
}
return borp_val_table(makeForStatement(borp_to_table(clause1), borp_to_table(clause2), clause3, isCStyle, body, line));
}

static bool hasReturnStatement(BorpArray* body) {
for (double i = 0; i <= (borp_length(borp_val_array(body)) - 1); i += 1) {
if ((borp_to_num(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "type")) == RETURN_STATEMENT)) {
return true;
} else if ((borp_to_num(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "type")) == IF_STATEMENT)) {
if ((((borp_length(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "elseBranch")) > 0) && hasReturnStatement(borp_to_array(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "elseBranch")))) && hasReturnStatement(borp_to_array(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "thenBranch"))))) {
return true;
}
} else if ((borp_to_num(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "type")) == BLOCK_STATEMENT)) {
if (hasReturnStatement(borp_to_array(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "statements")))) {
return true;
}
} else if (((borp_to_num(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "type")) == WHILE_STATEMENT) || (borp_to_num(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "type")) == FOR_STATEMENT))) {
if (hasReturnStatement(borp_to_array(borp_table_get(borp_array_get(body, (int)(i)).as.borp_table, "body")))) {
return true;
}
}
}
return false;
}

static BorpValue functionStatement(BorpTable* state, BorpValue isPrivate) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
BorpValue nameToken = expect(state, TOKEN_IDENTIFIER, "expected function name");
expect(state, TOKEN_LPAREN, "expected '(' after function name");
BorpArray* params = borp_array_create(0);
if ((! check(state, TOKEN_RPAREN))) {
while (true) {
BorpValue param = expect(state, TOKEN_IDENTIFIER, "expected parameter name");
double paramType = ANY;
if (borp_to_bool(match(state, TOKEN_COLON))) {
paramType = parseTypeAnnotation(state);
}
borp_push(params, borp_val_table(borp_table_from_entries(2, (const char*[]){"name", "type"}, (BorpValue[]){borp_table_get(param.as.borp_table, "value"), borp_val_num(paramType)})));
if ((! borp_to_bool(match(state, TOKEN_COMMA)))) {
break;
}
}
}
expect(state, TOKEN_RPAREN, "expected ')' after parameters");
double returnType = ANY;
if (borp_to_bool(match(state, TOKEN_COLON))) {
returnType = parseTypeAnnotation(state);
}
const char* parentName = "";
BorpArray* parentArgs = borp_array_create(0);
bool isLazy = false;
if (borp_to_bool(match(state, TOKEN_INHERIT))) {
isLazy = match(state, TOKEN_LAZY).type != VAL_NULL;
BorpValue pToken = expect(state, TOKEN_IDENTIFIER, "expected parent function name after '->'");
parentName = borp_to_str(borp_table_get(pToken.as.borp_table, "value"));
}
BorpArray* body = borp_array_create(0);
if (borp_to_bool(match(state, TOKEN_LBRACE))) {
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)})));
expect(state, TOKEN_RBRACE, "expected '}' to close function body");
} else {
body = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_END)})));
expect(state, TOKEN_END, "expected 'end' to close function body");
}
if (isPrivate.type == VAL_NULL) {
isPrivate = borp_val_boolean(false);
}
bool hasReturn = false;
if (hasReturnStatement(body)) {
hasReturn = true;
}
return borp_val_table(makeFunctionStatement(borp_to_str(borp_table_get(nameToken.as.borp_table, "value")), params, body, parentName, parentArgs, hasReturn, returnType, isLazy, borp_to_bool(isPrivate), line));
}

static BorpValue returnStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(advance(state), "line"));
BorpValue value = borp_val_null();
if (((((! check(state, TOKEN_SEMICOLON)) && (! check(state, TOKEN_END))) && (! check(state, TOKEN_RBRACE))) && (! check(state, TOKEN_NEWLINE)))) {
value = expression(state);
}
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
return borp_val_table(makeReturnStatement(borp_to_table(value), line));
}

static BorpValue varDeclareStatement(BorpTable* state, BorpValue isPrivate) {
BorpTable* nameToken = advance(state);
double line = borp_to_num(borp_table_get(nameToken, "line"));
expect(state, TOKEN_COLON, "expected ':' after variable name");
double declaredType = parseTypeAnnotation(state);
if ((declaredType != ENUM)) {
expect(state, TOKEN_ASSIGN, "expected '=' after type annotation");
} else if (check(state, TOKEN_ASSIGN)) {
move(state);
}
BorpValue initializer = expression(state);
if ((declaredType == ENUM)) {
if ((borp_to_num(borp_table_get(initializer.as.borp_table, "type")) != TABLE_EXPR)) {
borp_print_val(borp_val_string(borp_concat("Expected a table literal '{...}' to initialize an enum on line: ", borp_num_to_str(line))));
}
initializer = borp_val_table(desugarEnumTable(borp_to_table(initializer)));
}
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
if (isPrivate.type == VAL_NULL) {
isPrivate = borp_val_boolean(false);
}
return borp_val_table(makeVarDeclareStatement(borp_to_str(borp_table_get(nameToken, "value")), borp_to_table(initializer), declaredType, borp_to_bool(isPrivate), line));
}

static BorpValue ifStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(peek(state).as.borp_table, "line"));
move(state);
BorpValue condition = expression(state);
BorpArray* thenBranch = borp_array_create(0);
BorpArray* elseBranch = borp_array_create(0);
if (check(state, TOKEN_LBRACE)) {
move(state);
thenBranch = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)})));
expect(state, TOKEN_RBRACE, "expected '}' to close body");
if (check(state, TOKEN_ELSEIF)) {
borp_push(elseBranch, ifStatement(state));
} else if (borp_to_bool(match(state, TOKEN_ELSE))) {
expect(state, TOKEN_LBRACE, "expected '{' to open else body");
elseBranch = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)})));
expect(state, TOKEN_RBRACE, "expected '}' to close else body");
}
} else {
expect(state, TOKEN_THEN, "expected 'then' or '{' after condition");
thenBranch = borp_to_array(statementList(state, borp_array_from_elements(3, (BorpValue[]){borp_val_num(TOKEN_ELSEIF), borp_val_num(TOKEN_ELSE), borp_val_num(TOKEN_END)})));
if (check(state, TOKEN_ELSEIF)) {
borp_push(elseBranch, ifStatement(state));
} else if (borp_to_bool(match(state, TOKEN_ELSE))) {
elseBranch = borp_to_array(statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_END)})));
expect(state, TOKEN_END, "expected 'end' to close if statement");
} else {
expect(state, TOKEN_END, "expected 'end' to close if statement");
}
}
return borp_val_table(makeIfStatement(borp_to_table(condition), borp_val_array(thenBranch), borp_val_array(elseBranch), line));
}

static BorpValue blockStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(peek(state).as.borp_table, "line"));
move(state);
BorpValue statements = statementList(state, borp_array_from_elements(1, (BorpValue[]){borp_val_num(TOKEN_RBRACE)}));
expect(state, TOKEN_RBRACE, "expected '}' to close block");
return borp_val_table(makeBlockStatement(statements, line));
}

static BorpValue expressionStatement(BorpTable* state) {
double line = borp_to_num(borp_table_get(peek(state).as.borp_table, "line"));
BorpValue expr = expression(state);
matchMulti(state, borp_array_from_elements(2, (BorpValue[]){borp_val_num(TOKEN_SEMICOLON), borp_val_num(TOKEN_NEWLINE)}));
return borp_val_table(makeExpressionStatement(expr, line));
}

static BorpValue logicalAnd(BorpTable* state) {
BorpValue expr = equality(state);
while (check(state, TOKEN_AND)) {
BorpTable* token = advance(state);
BorpValue right = equality(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue logicalOr(BorpTable* state) {
BorpValue expr = logicalAnd(state);
while (check(state, TOKEN_OR)) {
BorpTable* token = advance(state);
BorpValue right = logicalAnd(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue equality(BorpTable* state) {
BorpValue expr = relational(state);
while ((check(state, TOKEN_EQUALS) || check(state, TOKEN_NOT_EQUAL))) {
BorpTable* token = advance(state);
BorpValue right = relational(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue relational(BorpTable* state) {
BorpValue expr = additive(state);
while ((((check(state, TOKEN_LESSER) || check(state, TOKEN_LESSER_EQUAL)) || check(state, TOKEN_GREATER)) || check(state, TOKEN_GREATER_EQUAL))) {
BorpTable* token = advance(state);
BorpValue right = additive(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue additive(BorpTable* state) {
BorpValue expr = multiplicative(state);
while (((check(state, TOKEN_PLUS) || check(state, TOKEN_MINUS)) || check(state, TOKEN_DOTDOT))) {
BorpTable* token = advance(state);
BorpValue right = multiplicative(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue multiplicative(BorpTable* state) {
BorpValue expr = unary(state);
while ((check(state, TOKEN_STAR) || check(state, TOKEN_SLASH))) {
BorpTable* token = advance(state);
BorpValue right = unary(state);
expr = borp_val_table(makeBinaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(expr), borp_to_table(right), borp_to_num(borp_table_get(token, "line"))));
}
return expr;
}

static BorpValue unary(BorpTable* state) {
if ((check(state, TOKEN_OPPOSE) || check(state, TOKEN_MINUS))) {
BorpTable* token = advance(state);
BorpValue operand = unary(state);
return borp_val_table(makeUnaryExpr(borp_to_num(borp_table_get(token, "type")), borp_to_table(operand), borp_to_num(borp_table_get(token, "line"))));
}
return primary(state);
}

static BorpValue assignment(BorpTable* state) {
BorpValue expr = logicalOr(state);
if (check(state, TOKEN_ASSIGN)) {
double line = borp_to_num(borp_table_get(peek(state).as.borp_table, "line"));
move(state);
BorpValue value = assignment(state);
if ((borp_to_num(borp_table_get(expr.as.borp_table, "type")) == IDENTIFIER_EXPR)) {
BorpValue name = borp_table_get(expr.as.borp_table, "value");
return borp_val_table(makeAssignExpr(borp_to_str(name), borp_to_table(value), line));
}
if ((borp_to_num(borp_table_get(expr.as.borp_table, "type")) == INDEX_EXPR)) {
return borp_val_table(makeIndexAssignExpr(borp_to_table(borp_table_get(expr.as.borp_table, "target")), borp_to_table(borp_table_get(expr.as.borp_table, "key")), borp_to_table(value), line));
}
borp_print_val(borp_val_string(borp_concat("Invalid assignment target line:", borp_num_to_str(line))));
}
return expr;
}

static BorpTable* primaryBase(BorpTable* state) {
if (check(state, TOKEN_NUMBER)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_NUMBER, "Unexpected type detected"));
return makeNumberExpr(borp_to_num(borp_table_get(token, "value")), borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_STRING)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_STRING, "Unexpected type detected"));
return makeStringExpr(borp_to_str(borp_table_get(token, "value")), borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_IDENTIFIER)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_IDENTIFIER, "Unexpected type detected"));
return makeIdentifierExpr(borp_to_str(borp_table_get(token, "value")), borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_LPAREN)) {
move(state);
BorpValue valExpr = expression(state);
expect(state, TOKEN_RPAREN, "expected ')' after expression");
return borp_to_table(valExpr);
}
if (check(state, TOKEN_LBRACE)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_LBRACE, "expected '{'"));
BorpArray* fields = borp_array_create(0);
double index = 0;
if ((! check(state, TOKEN_RBRACE))) {
while (true) {
if ((check(state, TOKEN_IDENTIFIER) && ((((borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_ASSIGN) || (borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_COLON)) || (borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_COMMA)) || (borp_to_num(borp_table_get(peekNext(state).as.borp_table, "type")) == TOKEN_RBRACE)))) {
BorpValue nameToken = expect(state, TOKEN_IDENTIFIER, "Expected field name");
if (borp_to_bool(match(state, TOKEN_ASSIGN))) {
BorpTable* valExpr = borp_to_table(expression(state));
borp_push(fields, borp_val_table(borp_table_from_entries(3, (const char*[]){"key", "value", "fieldType"}, (BorpValue[]){borp_table_get(nameToken.as.borp_table, "value"), borp_val_table(valExpr), borp_val_num(ANY)})));
} else if (borp_to_bool(match(state, TOKEN_COLON))) {
double fType = parseTypeAnnotation(state);
expect(state, TOKEN_ASSIGN, "Expected '=' after field name");
BorpTable* valExpr = borp_to_table(expression(state));
if ((fType == ENUM)) {
if ((borp_to_num(borp_table_get(valExpr, "type")) != TABLE_EXPR)) {
borp_print_val(borp_val_string(borp_concat("expected a table literal ('{...}') to initialize an enum on line ", borp_to_text(borp_table_get(valExpr, "line")))));
}
valExpr = desugarEnumTable(valExpr);
}
borp_push(fields, borp_val_table(borp_table_from_entries(3, (const char*[]){"key", "value", "fieldType"}, (BorpValue[]){borp_table_get(nameToken.as.borp_table, "value"), borp_val_table(valExpr), borp_val_num(fType)})));
} else {
BorpTable* valExpr = makeNumberExpr(index, borp_to_num(borp_table_get(nameToken.as.borp_table, "line")));
borp_push(fields, borp_val_table(borp_table_from_entries(3, (const char*[]){"key", "value", "fieldType"}, (BorpValue[]){borp_table_get(nameToken.as.borp_table, "value"), borp_val_table(valExpr), borp_val_num(NUMBER)})));
index = (index + 1);
}
} else {
BorpValue valExpr = expression(state);
borp_push(fields, borp_val_table(borp_table_from_entries(3, (const char*[]){"key", "value", "fieldType"}, (BorpValue[]){borp_val_string(borp_stringify(borp_val_num(index))), valExpr, borp_val_num(ANY)})));
index = (index + 1);
}
if (borp_to_bool(match(state, TOKEN_COMMA))) {
if (check(state, TOKEN_RBRACE)) {
break;
}
} else {
break;
}
}
}
expect(state, TOKEN_RBRACE, "expected '}' to close table literal");
return makeTableExpr(fields, borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_LBRACKET)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_LBRACKET, "expected '['"));
BorpArray* elements = borp_array_create(0);
if ((! check(state, TOKEN_RBRACKET))) {
while (true) {
BorpValue valExpr = expression(state);
borp_push(elements, valExpr);
if (borp_to_bool(match(state, TOKEN_COMMA))) {
if (check(state, TOKEN_RBRACKET)) {
break;
}
} else {
break;
}
}
}
expect(state, TOKEN_RBRACKET, "expected ']' to close array literal");
return makeArrayExpr(elements, borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_TRUE)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_TRUE, "expected boolean"));
return makeBooleanExpr(true, borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_FALSE)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_FALSE, "expected boolean"));
return makeBooleanExpr(false, borp_to_num(borp_table_get(token, "line")));
}
if (check(state, TOKEN_NULL)) {
BorpTable* token = borp_to_table(consume(state, TOKEN_NULL, ""));
return makeNullExpr(borp_to_num(borp_table_get(token, "line")));
}
borp_print_val(borp_val_string(borp_concat("Expected expression at line ", borp_to_text(borp_table_get(peek(state).as.borp_table, "line")))));
return borp_table_create(0);
}

static BorpValue primary(BorpTable* state) {
BorpTable* valExpr = primaryBase(state);
while (true) {
if (check(state, TOKEN_DOT)) {
BorpTable* dotToken = borp_to_table(consume(state, TOKEN_DOT, "expected property name after '.'"));
BorpTable* nameToken = borp_to_table(consume(state, TOKEN_IDENTIFIER, "expected property name after '.'"));
BorpTable* keyExpr = makeStringExpr(borp_to_str(borp_table_get(nameToken, "value")), borp_to_num(borp_table_get(nameToken, "line")));
valExpr = makeIndexExpr(valExpr, keyExpr, borp_to_num(borp_table_get(dotToken, "line")));
} else if (check(state, TOKEN_LBRACKET)) {
BorpTable* lbrToken = borp_to_table(consume(state, TOKEN_LBRACKET, "expected '['"));
BorpValue keyExpr = expression(state);
expect(state, TOKEN_RBRACKET, "expected ']' after index expression");
valExpr = makeIndexExpr(valExpr, borp_to_table(keyExpr), borp_to_num(borp_table_get(lbrToken, "line")));
} else if (check(state, TOKEN_LPAREN)) {
valExpr = borp_to_table(finishCall(state, borp_val_table(valExpr), borp_table_get(peek(state).as.borp_table, "line")));
} else {
break;
}
}
return borp_val_table(valExpr);
}

static BorpValue finishCall(BorpTable* state, BorpValue callee, BorpValue line) {
expect(state, TOKEN_LPAREN, "expected '(' to start call arguments");
BorpArray* arguments = borp_array_create(0);
if ((! check(state, TOKEN_RPAREN))) {
borp_push(arguments, expression(state));
while (borp_to_bool(match(state, TOKEN_COMMA))) {
if (check(state, TOKEN_RPAREN)) {
break;
}
borp_push(arguments, expression(state));
}
}
expect(state, TOKEN_RPAREN, "expected ')' after arguments");
return borp_val_table(makeCallExpr(borp_to_table(callee), arguments, borp_to_num(line)));
}

static const char* asStr(BorpTable* e) {
const char* code = genExpr(e);
if ((borp_to_num(borp_table_get(e, "inferredType")) == STRING)) {
return code;
}
if ((borp_to_num(borp_table_get(e, "inferredType")) == NUMBER)) {
return borp_concat(borp_concat("borp_num_to_str(", code), ")");
}
return borp_concat(borp_concat("borp_to_text(", code), ")");
}

static bool isMathematical(double tokenType) {
if (((((tokenType == TOKEN_PLUS) || (tokenType == TOKEN_MINUS)) || (tokenType == TOKEN_STAR)) || (tokenType == TOKEN_SLASH))) {
return true;
}
return false;
}

static bool isComparative(double tokenType) {
if (((((((((tokenType == TOKEN_EQUALS) || (tokenType == TOKEN_NOT_EQUAL)) || (tokenType == TOKEN_LESSER)) || (tokenType == TOKEN_GREATER)) || (tokenType == TOKEN_LESSER_EQUAL)) || (tokenType == TOKEN_GREATER_EQUAL)) || (tokenType == TOKEN_AND)) || (tokenType == TOKEN_OR))) {
return true;
}
return false;
}

static bool isEnumRef(BorpTable* target, BorpTable* env) {
if (((borp_to_num(borp_table_get(target, "type")) == IDENTIFIER_EXPR) && (borp_to_num(lookupEnv(borp_val_table(env), borp_table_get(target, "value"))) == ENUM))) {
return true;
}
return false;
}

static BorpTable* createEnv(BorpTable* parentEnv) {
BorpTable* environment = borp_table_from_entries(3, (const char*[]){"vars", "funcs", "parent"}, (BorpValue[]){borp_val_table(borp_table_create(0)), borp_val_table(borp_table_create(0)), borp_val_table(parentEnv)});
return environment;
}

static void defineEnv(BorpTable* env, BorpValue name, double type) {
borp_index_set(borp_table_get(env, "vars"), name, borp_val_num(type));
}

static BorpValue lookupEnv(BorpValue env, BorpValue name) {
if (borp_index_get(borp_table_get(env.as.borp_table, "vars"), name).type != VAL_NULL) {
return borp_index_get(borp_table_get(env.as.borp_table, "vars"), name);
} else if (borp_table_get(env.as.borp_table, "parent").type != VAL_NULL) {
return lookupEnv(borp_table_get(env.as.borp_table, "parent"), name);
} else {
return borp_val_null();
}
}

static BorpTable* lookupFunc(BorpValue env, BorpValue name) {
if (borp_index_get(borp_table_get(env.as.borp_table, "funcs"), name).type != VAL_NULL) {
return borp_to_table(borp_index_get(borp_table_get(env.as.borp_table, "funcs"), name));
} else if (borp_table_get(env.as.borp_table, "parent").type != VAL_NULL) {
return lookupFunc(borp_table_get(env.as.borp_table, "parent"), name);
} else {
return NULL;
}
}

static double resolveExpr(BorpTable* expr, BorpTable* env) {
if ((borp_to_num(borp_table_get(expr, "type")) == NUMBER_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(NUMBER));
return NUMBER;
} else if ((borp_to_num(borp_table_get(expr, "type")) == STRING_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(STRING));
return STRING;
} else if ((borp_to_num(borp_table_get(expr, "type")) == BOOL_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(BOOLEAN));
return BOOLEAN;
} else if ((borp_to_num(borp_table_get(expr, "type")) == NULL_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(ANY));
return ANY;
} else if ((borp_to_num(borp_table_get(expr, "type")) == IDENTIFIER_EXPR)) {
BorpValue varType = lookupEnv(borp_val_table(env), borp_table_get(expr, "value"));
if (varType.type != VAL_NULL) {
borp_table_set(expr, "inferredType", varType);
} else {
borp_table_set(expr, "inferredType", borp_val_num(ANY));
}
return borp_to_num(borp_table_get(expr, "inferredType"));
} else if ((borp_to_num(borp_table_get(expr, "type")) == TABLE_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(TABLE));
for (double i = 0; i <= (borp_length(borp_table_get(expr, "fields")) - 1); i += 1) {
resolveExpr(borp_to_table(borp_table_get(borp_array_get(borp_table_get(expr, "fields").as.borp_array, (int)(i)).as.borp_table, "value")), env);
}
return TABLE;
} else if ((borp_to_num(borp_table_get(expr, "type")) == ARRAY_EXPR)) {
borp_table_set(expr, "inferredType", borp_val_num(ARRAY));
for (double i = 0; i <= (borp_length(borp_table_get(expr, "elements")) - 1); i += 1) {
resolveExpr(borp_to_table(borp_array_get(borp_table_get(expr, "elements").as.borp_array, (int)(i))), env);
}
return ARRAY;
} else if ((borp_to_num(borp_table_get(expr, "type")) == UNARY_EXPR)) {
resolveExpr(borp_to_table(borp_table_get(expr, "operand")), env);
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_MINUS)) {
borp_table_set(expr, "inferredType", borp_val_num(NUMBER));
return NUMBER;
} else {
borp_table_set(expr, "inferredType", borp_val_num(BOOLEAN));
return BOOLEAN;
}
} else if ((borp_to_num(borp_table_get(expr, "type")) == BINARY_EXPR)) {
double leftType = resolveExpr(borp_to_table(borp_table_get(expr, "left")), env);
double rightType = resolveExpr(borp_to_table(borp_table_get(expr, "right")), env);
if (((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_PLUS) && ((leftType == STRING) || (rightType == STRING)))) {
borp_table_set(expr, "inferredType", borp_val_num(STRING));
return STRING;
} else if (isMathematical(borp_to_num(borp_table_get(expr, "tokenType")))) {
borp_table_set(expr, "inferredType", borp_val_num(NUMBER));
return NUMBER;
} else if (isComparative(borp_to_num(borp_table_get(expr, "tokenType")))) {
borp_table_set(expr, "inferredType", borp_val_num(BOOLEAN));
return BOOLEAN;
} else {
borp_table_set(expr, "inferredType", borp_val_num(STRING));
return STRING;
}
} else if ((borp_to_num(borp_table_get(expr, "type")) == ASSIGN_EXPR)) {
double valType = resolveExpr(borp_to_table(borp_table_get(expr, "value")), env);
borp_table_set(expr, "targetType", lookupEnv(borp_val_table(env), borp_table_get(expr, "name")));
borp_table_set(expr, "inferredType", borp_val_num(valType));
return valType;
} else if ((borp_to_num(borp_table_get(expr, "type")) == CALL_EXPR)) {
BorpTable* sig = lookupFunc(borp_val_table(env), borp_table_get(borp_table_get(expr, "callee").as.borp_table, "value"));
for (double i = 0; i <= (borp_length(borp_table_get(expr, "arguments")) - 1); i += 1) {
resolveExpr(borp_to_table(borp_array_get(borp_table_get(expr, "arguments").as.borp_array, (int)(i))), env);
if ((sig != NULL && (i < borp_length(borp_table_get(sig, "parameters"))))) {
borp_table_set(borp_array_get(borp_table_get(expr, "arguments").as.borp_array, (int)(i)).as.borp_table, "expectedType", borp_table_get(borp_array_get(borp_table_get(sig, "parameters").as.borp_array, (int)(i)).as.borp_table, "type"));
}
}
if (sig != NULL) {
borp_table_set(expr, "cName", borp_table_get(sig, "cName"));
}
BorpValue returnType = lookupEnv(borp_val_table(env), borp_table_get(borp_table_get(expr, "callee").as.borp_table, "value"));
if (returnType.type == VAL_NULL) {
returnType = borp_val_num(ANY);
}
borp_table_set(expr, "inferredType", returnType);
return borp_to_num(returnType);
} else if ((borp_to_num(borp_table_get(expr, "type")) == INDEX_EXPR)) {
resolveExpr(borp_to_table(borp_table_get(expr, "target")), env);
resolveExpr(borp_to_table(borp_table_get(expr, "key")), env);
if (isEnumRef(borp_to_table(borp_table_get(expr, "target")), env)) {
borp_table_set(expr, "isEnumAccess", borp_val_boolean(true));
borp_table_set(expr, "inferredType", borp_val_num(NUMBER));
return NUMBER;
} else {
borp_table_set(expr, "inferredType", borp_val_num(ANY));
return ANY;
}
} else if ((borp_to_num(borp_table_get(expr, "type")) == INDEX_ASSIGN_EXPR)) {
resolveExpr(borp_to_table(borp_table_get(expr, "target")), env);
resolveExpr(borp_to_table(borp_table_get(expr, "key")), env);
double valType = resolveExpr(borp_to_table(borp_table_get(expr, "value")), env);
borp_table_set(expr, "inferredType", borp_val_num(valType));
return valType;
}
return 0;
}

static void resolveStatement(BorpValue stmt, BorpValue env) {
if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == EXPRESSION_STATEMENT)) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "expression")), borp_to_table(env));
BorpValue e = borp_table_get(stmt.as.borp_table, "expression");
if (((borp_to_num(borp_table_get(e.as.borp_table, "type")) == ASSIGN_EXPR) && lookupEnv(env, borp_table_get(e.as.borp_table, "name")).type == VAL_NULL)) {
defineEnv(borp_to_table(env), borp_table_get(e.as.borp_table, "name"), borp_to_num(borp_table_get(e.as.borp_table, "inferredType")));
borp_table_set(e.as.borp_table, "isDeclaration", borp_val_boolean(true));
}
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == RETURN_STATEMENT)) {
if (borp_table_get(stmt.as.borp_table, "value").type != VAL_NULL) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "value")), borp_to_table(env));
}
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == VARDECLARE_STATEMENT)) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "initializer")), borp_to_table(env));
BorpValue stmtType = borp_table_get(stmt.as.borp_table, "typeAnnotation");
if ((stmtType.type == VAL_NULL || (borp_to_num(stmtType) == ANY))) {
stmtType = borp_table_get(borp_table_get(stmt.as.borp_table, "initializer").as.borp_table, "inferredType");
}
defineEnv(borp_to_table(env), borp_table_get(stmt.as.borp_table, "name"), borp_to_num(stmtType));
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == FUNCTION_STATEMENT)) {
defineEnv(borp_to_table(env), borp_table_get(stmt.as.borp_table, "name"), borp_to_num(borp_table_get(stmt.as.borp_table, "returnType")));
borp_index_set(borp_table_get(env.as.borp_table, "funcs"), borp_table_get(stmt.as.borp_table, "name"), stmt);
BorpTable* fnEnv = createEnv(borp_to_table(env));
for (double i = 0; i <= (borp_length(borp_table_get(stmt.as.borp_table, "parameters")) - 1); i += 1) {
defineEnv(fnEnv, borp_table_get(borp_array_get(borp_table_get(stmt.as.borp_table, "parameters").as.borp_array, (int)(i)).as.borp_table, "name"), borp_to_num(borp_table_get(borp_array_get(borp_table_get(stmt.as.borp_table, "parameters").as.borp_array, (int)(i)).as.borp_table, "type")));
}
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "body")), fnEnv);
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == BLOCK_STATEMENT)) {
BorpTable* blockEnv = createEnv(borp_to_table(env));
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "statements")), blockEnv);
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == IF_STATEMENT)) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "condition")), borp_to_table(env));
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "thenBranch")), createEnv(borp_to_table(env)));
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "elseBranch")), createEnv(borp_to_table(env)));
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == WHILE_STATEMENT)) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "condition")), borp_to_table(env));
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "body")), createEnv(borp_to_table(env)));
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == FOR_STATEMENT)) {
BorpTable* forEnv = createEnv(borp_to_table(env));
if ((! borp_to_bool(borp_table_get(stmt.as.borp_table, "isCStyle")))) {
defineEnv(forEnv, borp_table_get(borp_table_get(stmt.as.borp_table, "clause1").as.borp_table, "name"), NUMBER);
}
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "clause1")), forEnv);
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "clause2")), forEnv);
if (borp_table_get(stmt.as.borp_table, "clause3").type != VAL_NULL) {
resolveExpr(borp_to_table(borp_table_get(stmt.as.borp_table, "clause3")), forEnv);
}
resolveStatementList(borp_to_array(borp_table_get(stmt.as.borp_table, "body")), forEnv);
}
}

static void resolveStatementList(BorpArray* stmts, BorpTable* env) {
for (double i = 0; i <= (borp_length(borp_val_array(stmts)) - 1); i += 1) {
resolveStatement(borp_array_get(stmts, (int)(i)), borp_val_table(env));
}
}

static void defineBuiltin(BorpTable* env, const char* name, const char* cName, double retType, BorpArray* paramTypes) {
BorpArray* params = borp_array_create(0);
for (double i = 0; i <= (borp_length(borp_val_array(paramTypes)) - 1); i += 1) {
borp_push(params, borp_val_table(borp_table_from_entries(2, (const char*[]){"name", "type"}, (BorpValue[]){borp_val_string(borp_concat("p", borp_stringify(borp_val_num(i)))), borp_array_get(paramTypes, (int)(i))})));
}
borp_table_set(borp_table_get(env, "funcs").as.borp_table, name, borp_val_table(borp_table_from_entries(4, (const char*[]){"name", "cName", "parameters", "returnType"}, (BorpValue[]){borp_val_string(name), borp_val_string(cName), borp_val_array(params), borp_val_num(retType)})));
defineEnv(env, borp_val_string(name), retType);
}

void resolveProgram(BorpArray* statements) {
BorpTable* globalEnv = createEnv(NULL);
for (double i = 0; i <= (borp_length(borp_val_array(statements)) - 1); i += 1) {
if ((borp_to_num(borp_table_get(borp_array_get(statements, (int)(i)).as.borp_table, "type")) == FUNCTION_STATEMENT)) {
borp_index_set(borp_table_get(globalEnv, "funcs"), borp_table_get(borp_array_get(statements, (int)(i)).as.borp_table, "name"), borp_array_get(statements, (int)(i)));
defineEnv(globalEnv, borp_table_get(borp_array_get(statements, (int)(i)).as.borp_table, "name"), borp_to_num(borp_table_get(borp_array_get(statements, (int)(i)).as.borp_table, "returnType")));
}
}
defineBuiltin(globalEnv, "length", "borp_length", NUMBER, borp_array_from_elements(1, (BorpValue[]){borp_val_num(ANY)}));
defineBuiltin(globalEnv, "push", "borp_push", NUMBER, borp_array_from_elements(2, (BorpValue[]){borp_val_num(ARRAY), borp_val_num(ANY)}));
defineBuiltin(globalEnv, "pop", "borp_pop", ANY, borp_array_from_elements(1, (BorpValue[]){borp_val_num(ARRAY)}));
defineBuiltin(globalEnv, "join", "borp_join", STRING, borp_array_from_elements(2, (BorpValue[]){borp_val_num(ARRAY), borp_val_num(ANY)}));
defineBuiltin(globalEnv, "charAt", "charAt", STRING, borp_array_from_elements(2, (BorpValue[]){borp_val_num(STRING), borp_val_num(INTEGER)}));
defineBuiltin(globalEnv, "substring", "borp_substring", STRING, borp_array_from_elements(3, (BorpValue[]){borp_val_num(STRING), borp_val_num(INTEGER), borp_val_num(INTEGER)}));
defineBuiltin(globalEnv, "isNumeric", "borp_is_numeric", BOOLEAN, borp_array_from_elements(1, (BorpValue[]){borp_val_num(ANY)}));
defineBuiltin(globalEnv, "toNumber", "borp_parse_number", NUMBER, borp_array_from_elements(1, (BorpValue[]){borp_val_num(STRING)}));
defineBuiltin(globalEnv, "toString", "borp_stringify", STRING, borp_array_from_elements(1, (BorpValue[]){borp_val_num(ANY)}));
defineBuiltin(globalEnv, "readFile", "borp_read_file", STRING, borp_array_from_elements(1, (BorpValue[]){borp_val_num(STRING)}));
defineBuiltin(globalEnv, "writeFile", "borp_write_file", BOOLEAN, borp_array_from_elements(2, (BorpValue[]){borp_val_num(STRING), borp_val_num(ANY)}));
defineBuiltin(globalEnv, "runCommand", "borp_run_command", NUMBER, borp_array_from_elements(1, (BorpValue[]){borp_val_num(STRING)}));
defineBuiltin(globalEnv, "argCount", "borp_arg_count", NUMBER, borp_array_create(0));
defineBuiltin(globalEnv, "getArg", "borp_get_arg", STRING, borp_array_from_elements(1, (BorpValue[]){borp_val_num(NUMBER)}));
resolveStatementList(statements, globalEnv);
}

static const char* escapeC(const char* s) {
const char* out = "";
for (double i = 0; i <= (borp_length(borp_val_string(s)) - 1); i += 1) {
const char* ch = charAt(s, i);
if ((strcmp(ch, "\\") == 0)) {
out = borp_concat(out, "\\\\");
} else if ((strcmp(ch, "\"") == 0)) {
out = borp_concat(out, "\\\"");
} else if ((strcmp(ch, "\n") == 0)) {
out = borp_concat(out, "\\n");
} else if ((strcmp(ch, "\t") == 0)) {
out = borp_concat(out, "\\t");
} else if ((strcmp(ch, "\r") == 0)) {
out = borp_concat(out, "\\r");
} else {
out = borp_concat(out, ch);
}
}
return out;
}

static const char* boxValue(BorpTable* expr) {
const char* valCode = genExpr(expr);
if ((borp_to_num(borp_table_get(expr, "inferredType")) == NUMBER)) {
return borp_concat(borp_concat("borp_val_num(", valCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "inferredType")) == STRING)) {
return borp_concat(borp_concat("borp_val_string(", valCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "inferredType")) == BOOLEAN)) {
return borp_concat(borp_concat("borp_val_boolean(", valCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "inferredType")) == TABLE)) {
return borp_concat(borp_concat("borp_val_table(", valCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "inferredType")) == ARRAY)) {
return borp_concat(borp_concat("borp_val_array(", valCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "type")) == NULL_EXPR)) {
return "borp_val_null()";
}
return valCode;
}

static const char* coerceTo(BorpTable* expr, double targetType) {
const char* valCode = genExpr(expr);
if ((borp_to_num(borp_table_get(expr, "type")) == NULL_EXPR)) {
if ((((targetType == STRING) || (targetType == TABLE)) || (targetType == ARRAY))) {
return "NULL";
}
}
if (((targetType == STRING) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("borp_to_str(", valCode), ")");
}
if (((targetType == NUMBER) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("borp_to_num(", valCode), ")");
}
if (((targetType == BOOLEAN) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("borp_to_bool(", valCode), ")");
}
if (((targetType == INTEGER) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("(int)borp_to_num(", valCode), ")");
}
if (((targetType == TABLE) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("borp_to_table(", valCode), ")");
}
if (((targetType == ARRAY) && (borp_to_num(borp_table_get(expr, "inferredType")) == ANY))) {
return borp_concat(borp_concat("borp_to_array(", valCode), ")");
}
return valCode;
}

static const char* provFunctions(const char* fnName, BorpTable* expr) {
if ((strcmp(fnName, "print") == 0)) {
if ((borp_length(borp_table_get(expr, "arguments")) > 0)) {
return borp_concat(borp_concat("borp_print_val(", boxValue(borp_to_table(borp_array_get(borp_table_get(expr, "arguments").as.borp_array, (int)(0))))), ")");
} else {
return "printf(\"\\n\")";
}
}
if ((strcmp(fnName, "length") == 0)) {
return borp_concat(borp_concat("borp_length(", boxValue(borp_to_table(borp_array_get(borp_table_get(expr, "arguments").as.borp_array, (int)(0))))), ")");
}
return "";
}

static const char* cType(double typeAnno) {
if ((typeAnno == NUMBER)) {
return "double";
} else if ((typeAnno == DOUBLE)) {
return "double";
} else if ((typeAnno == FLOAT)) {
return "float";
} else if ((typeAnno == INTEGER)) {
return "int";
} else if ((typeAnno == STRING)) {
return "const char*";
} else if ((typeAnno == BOOLEAN)) {
return "bool";
} else if ((typeAnno == ENUM)) {
return "enum";
} else if ((typeAnno == TABLE)) {
return "BorpTable*";
} else if ((typeAnno == ARRAY)) {
return "BorpArray*";
} else if ((typeAnno == ANY)) {
return "BorpValue";
}
return "void";
}

static const char* dirOf(const char* path) {
double last = (-1);
for (double i = 0; i <= (borp_length(borp_val_string(path)) - 1); i += 1) {
const char* ch = charAt(path, i);
if (((strcmp(ch, "/") == 0) || (strcmp(ch, "\\") == 0))) {
last = i;
}
}
if ((last < 0)) {
return "";
}
return borp_substring(path, 0, (last + 1));
}

static BorpArray* expandImports(BorpArray* statements, const char* baseDir, BorpTable* visited) {
BorpArray* result = borp_array_create(0);
for (double i = 0; i <= (borp_length(borp_val_array(statements)) - 1); i += 1) {
BorpValue stmt = borp_array_get(statements, (int)(i));
if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == IMPORT_STATEMENT)) {
const char* path = borp_concat(baseDir, borp_to_text(borp_table_get(stmt.as.borp_table, "path")));
if (borp_table_get(visited, path).type == VAL_NULL) {
borp_table_set(visited, path, borp_val_boolean(true));
const char* source = borp_read_file(path);
BorpValue imported = parseProgram(parseState(tokenize(borp_val_table(lexerState(source)))));
BorpArray* expanded = expandImports(borp_to_array(imported), dirOf(path), visited);
for (double j = 0; j <= (borp_length(borp_val_array(expanded)) - 1); j += 1) {
borp_push(result, borp_array_get(expanded, (int)(j)));
}
}
} else {
borp_push(result, stmt);
}
}
return result;
}

static const char* generateHeaders(BorpTable* statement) {
const char* retType = cType(borp_to_num(borp_table_get(statement, "returnType")));
const char* params = "";
if ((borp_length(borp_table_get(statement, "parameters")) == 0)) {
params = "";
} else {
for (double i = 0; i <= (borp_length(borp_table_get(statement, "parameters")) - 1); i += 1) {
BorpValue p = borp_array_get(borp_table_get(statement, "parameters").as.borp_array, (int)(i));
if ((i < (borp_length(borp_table_get(statement, "parameters")) - 1))) {
params = borp_concat(borp_concat(borp_concat(borp_concat(params, cType(borp_to_num(borp_table_get(p.as.borp_table, "type")))), " "), borp_to_text(borp_table_get(p.as.borp_table, "name"))), ", ");
} else {
params = borp_concat(borp_concat(borp_concat(params, cType(borp_to_num(borp_table_get(p.as.borp_table, "type")))), " "), borp_to_text(borp_table_get(p.as.borp_table, "name")));
}
}
}
if ((! borp_to_bool(borp_table_get(statement, "hasReturn")))) {
retType = "void";
}
if ((borp_length(borp_table_get(statement, "parameters")) == 0)) {
params = "void";
}
if (borp_to_bool(borp_table_get(statement, "isPrivate"))) {
retType = borp_concat("static ", retType);
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(retType, " "), borp_to_text(borp_table_get(statement, "name"))), "("), params), ");\n");
}

static const char* genExpr(BorpTable* expr) {
if ((borp_to_num(borp_table_get(expr, "type")) == NUMBER_EXPR)) {
return borp_stringify(borp_table_get(expr, "value"));
}
if ((borp_to_num(borp_table_get(expr, "type")) == STRING_EXPR)) {
return borp_concat(borp_concat("\"", escapeC(borp_to_str(borp_table_get(expr, "value")))), "\"");
}
if ((borp_to_num(borp_table_get(expr, "type")) == BOOL_EXPR)) {
if (borp_to_bool(borp_table_get(expr, "value"))) {
return "true";
} else {
return "false";
}
}
if ((borp_to_num(borp_table_get(expr, "type")) == NULL_EXPR)) {
return "NULL";
}
if ((borp_to_num(borp_table_get(expr, "type")) == IDENTIFIER_EXPR)) {
return borp_to_str(borp_table_get(expr, "value"));
}
if ((borp_to_num(borp_table_get(expr, "type")) == ASSIGN_EXPR)) {
const char* valCode = genExpr(borp_to_table(borp_table_get(expr, "value")));
if (borp_table_get(expr, "targetType").type != VAL_NULL) {
if ((borp_to_num(borp_table_get(expr, "targetType")) == ANY)) {
valCode = boxValue(borp_to_table(borp_table_get(expr, "value")));
} else {
valCode = coerceTo(borp_to_table(borp_table_get(expr, "value")), borp_to_num(borp_table_get(expr, "targetType")));
}
}
return borp_concat(borp_concat(borp_to_text(borp_table_get(expr, "name")), " = "), valCode);
}
if ((borp_to_num(borp_table_get(expr, "type")) == CALL_EXPR)) {
const char* args = "";
if ((borp_length(borp_table_get(expr, "arguments")) > 0)) {
for (double i = 0; i <= (borp_length(borp_table_get(expr, "arguments")) - 1); i += 1) {
BorpValue arg = borp_array_get(borp_table_get(expr, "arguments").as.borp_array, (int)(i));
const char* argCode = genExpr(borp_to_table(arg));
if (borp_table_get(arg.as.borp_table, "expectedType").type != VAL_NULL) {
if ((borp_to_num(borp_table_get(arg.as.borp_table, "expectedType")) == ANY)) {
argCode = boxValue(borp_to_table(arg));
} else {
argCode = coerceTo(borp_to_table(arg), borp_to_num(borp_table_get(arg.as.borp_table, "expectedType")));
}
}
if ((i < (borp_length(borp_table_get(expr, "arguments")) - 1))) {
args = borp_concat(borp_concat(args, argCode), ", ");
} else {
args = borp_concat(args, argCode);
}
}
}
const char* builtin = provFunctions(borp_to_str(borp_table_get(borp_table_get(expr, "callee").as.borp_table, "value")), expr);
if ((strcmp(builtin, "") != 0)) {
return builtin;
}
const char* calleeCode = genExpr(borp_to_table(borp_table_get(expr, "callee")));
if (borp_table_get(expr, "cName").type != VAL_NULL) {
calleeCode = borp_to_str(borp_table_get(expr, "cName"));
}
return borp_concat(borp_concat(borp_concat(calleeCode, "("), args), ")");
}
if ((borp_to_num(borp_table_get(expr, "type")) == UNARY_EXPR)) {
const char* operandCode = genExpr(borp_to_table(borp_table_get(expr, "operand")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "operand").as.borp_table, "inferredType")) == ANY)) {
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_MINUS)) {
operandCode = borp_concat(borp_concat("borp_to_num(", operandCode), ")");
} else {
operandCode = borp_concat(borp_concat("borp_to_bool(", operandCode), ")");
}
}
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_MINUS)) {
return borp_concat(borp_concat("(-", operandCode), ")");
} else {
return borp_concat(borp_concat("(! ", operandCode), ")");
}
}
if ((borp_to_num(borp_table_get(expr, "type")) == BINARY_EXPR)) {
const char* operand = "";
bool isMathOrRelational = false;
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_PLUS)) {
operand = "+";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_MINUS)) {
operand = "-";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_STAR)) {
operand = "*";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_SLASH)) {
operand = "/";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_GREATER_EQUAL)) {
operand = ">=";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_LESSER_EQUAL)) {
operand = "<=";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_LESSER)) {
operand = "<";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_GREATER)) {
operand = ">";
isMathOrRelational = true;
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_EQUALS)) {
operand = "==";
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL)) {
operand = "!=";
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_AND)) {
operand = "&&";
} else if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_OR)) {
operand = "||";
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "type")) == NULL_EXPR)) {
const char* leftSide = genExpr(borp_to_table(borp_table_get(expr, "left")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == ANY)) {
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL)) {
return borp_concat(leftSide, ".type != VAL_NULL");
} else {
return borp_concat(leftSide, ".type == VAL_NULL");
}
}
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL)) {
return borp_concat(leftSide, " != NULL");
} else {
return borp_concat(leftSide, " == NULL");
}
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "type")) == NULL_EXPR)) {
const char* rightSide = genExpr(borp_to_table(borp_table_get(expr, "right")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == ANY)) {
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL)) {
return borp_concat(rightSide, ".type != VAL_NULL");
} else {
return borp_concat(rightSide, ".type == VAL_NULL");
}
}
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL)) {
return borp_concat(rightSide, " != NULL");
} else {
return borp_concat(rightSide, " == NULL");
}
}
if (((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_PLUS) && (borp_to_num(borp_table_get(expr, "inferredType")) == STRING))) {
return borp_concat(borp_concat(borp_concat(borp_concat("borp_concat(", asStr(borp_to_table(borp_table_get(expr, "left")))), ", "), asStr(borp_to_table(borp_table_get(expr, "right")))), ")");
}
const char* leftCode = genExpr(borp_to_table(borp_table_get(expr, "left")));
const char* rightCode = genExpr(borp_to_table(borp_table_get(expr, "right")));
if (isMathOrRelational) {
if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == ANY)) {
leftCode = borp_concat(borp_concat("borp_to_num(", leftCode), ")");
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == ANY)) {
rightCode = borp_concat(borp_concat("borp_to_num(", rightCode), ")");
}
}
if (((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_EQUALS) || (borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL))) {
if (((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == ANY) && (borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) != ANY))) {
if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == NUMBER)) {
leftCode = borp_concat(borp_concat("borp_to_num(", leftCode), ")");
} else if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == STRING)) {
leftCode = borp_concat(borp_concat("borp_to_str(", leftCode), ")");
} else if ((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == BOOLEAN)) {
leftCode = borp_concat(borp_concat("borp_to_bool(", leftCode), ")");
}
} else if (((borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == ANY) && (borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) != ANY))) {
if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == NUMBER)) {
rightCode = borp_concat(borp_concat("borp_to_num(", rightCode), ")");
} else if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == STRING)) {
rightCode = borp_concat(borp_concat("borp_to_str(", rightCode), ")");
} else if ((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == BOOLEAN)) {
rightCode = borp_concat(borp_concat("borp_to_bool(", rightCode), ")");
}
}
}
if (((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_EQUALS) || (borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_NOT_EQUAL))) {
if (((borp_to_num(borp_table_get(borp_table_get(expr, "left").as.borp_table, "inferredType")) == STRING) || (borp_to_num(borp_table_get(borp_table_get(expr, "right").as.borp_table, "inferredType")) == STRING))) {
if ((borp_to_num(borp_table_get(expr, "tokenType")) == TOKEN_EQUALS)) {
return borp_concat(borp_concat(borp_concat(borp_concat("(strcmp(", leftCode), ", "), rightCode), ") == 0)");
} else {
return borp_concat(borp_concat(borp_concat(borp_concat("(strcmp(", leftCode), ", "), rightCode), ") != 0)");
}
}
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("(", leftCode), " "), operand), " "), rightCode), ")");
}
if ((borp_to_num(borp_table_get(expr, "type")) == INDEX_EXPR)) {
if (borp_table_get(expr, "isEnumAccess").type != VAL_NULL) {
return borp_to_str(borp_table_get(borp_table_get(expr, "key").as.borp_table, "value"));
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == ANY)) {
return borp_concat(borp_concat(borp_concat(borp_concat("borp_index_get(", boxValue(borp_to_table(borp_table_get(expr, "target")))), ", "), boxValue(borp_to_table(borp_table_get(expr, "key")))), ")");
}
const char* targetCode = genExpr(borp_to_table(borp_table_get(expr, "target")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "target").as.borp_table, "inferredType")) == ANY)) {
if (((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "type")) == STRING_EXPR) || (borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == STRING))) {
targetCode = borp_concat(targetCode, ".as.borp_table");
} else {
targetCode = borp_concat(targetCode, ".as.borp_array");
}
}
if (((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "type")) == STRING_EXPR) || (borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == STRING))) {
const char* key = genExpr(borp_to_table(borp_table_get(expr, "key")));
return borp_concat(borp_concat(borp_concat(borp_concat("borp_table_get(", targetCode), ", "), key), ")");
}
const char* key = genExpr(borp_to_table(borp_table_get(expr, "key")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == ANY)) {
key = borp_concat(borp_concat("(int)borp_to_num(", key), ")");
} else {
key = borp_concat(borp_concat("(int)(", key), ")");
}
return borp_concat(borp_concat(borp_concat(borp_concat("borp_array_get(", targetCode), ", "), key), ")");
}
if ((borp_to_num(borp_table_get(expr, "type")) == INDEX_ASSIGN_EXPR)) {
const char* target = genExpr(borp_to_table(borp_table_get(expr, "target")));
const char* key = genExpr(borp_to_table(borp_table_get(expr, "key")));
const char* wrappedVal = boxValue(borp_to_table(borp_table_get(expr, "value")));
if ((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == ANY)) {
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("borp_index_set(", boxValue(borp_to_table(borp_table_get(expr, "target")))), ", "), boxValue(borp_to_table(borp_table_get(expr, "key")))), ", "), wrappedVal), ")");
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "target").as.borp_table, "inferredType")) == ANY)) {
if (((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "type")) == STRING_EXPR) || (borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == STRING))) {
target = borp_concat(target, ".as.borp_table");
} else {
target = borp_concat(target, ".as.borp_array");
}
}
if (((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "type")) == STRING_EXPR) || (borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == STRING))) {
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("borp_table_set(", target), ", "), key), ", "), wrappedVal), ")");
}
if ((borp_to_num(borp_table_get(borp_table_get(expr, "key").as.borp_table, "inferredType")) == ANY)) {
key = borp_concat(borp_concat("(int)borp_to_num(", key), ")");
} else {
key = borp_concat(borp_concat("(int)(", key), ")");
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("borp_array_set(", target), ", "), key), ", "), wrappedVal), ")");
}
if ((borp_to_num(borp_table_get(expr, "type")) == TABLE_EXPR)) {
if ((borp_length(borp_table_get(expr, "fields")) == 0)) {
return "borp_table_create(0)";
} else if ((borp_length(borp_table_get(expr, "fields")) > 0)) {
const char* keys = "";
const char* vals = "";
for (double i = 0; i <= (borp_length(borp_table_get(expr, "fields")) - 1); i += 1) {
BorpValue field = borp_array_get(borp_table_get(expr, "fields").as.borp_array, (int)(i));
const char* wrappedVal = boxValue(borp_to_table(borp_table_get(field.as.borp_table, "value")));
if ((i < (borp_length(borp_table_get(expr, "fields")) - 1))) {
keys = borp_concat(borp_concat(borp_concat(keys, "\""), borp_to_text(borp_table_get(field.as.borp_table, "key"))), "\", ");
vals = borp_concat(borp_concat(vals, wrappedVal), ", ");
} else {
keys = borp_concat(borp_concat(borp_concat(keys, "\""), borp_to_text(borp_table_get(field.as.borp_table, "key"))), "\"");
vals = borp_concat(vals, wrappedVal);
}
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("borp_table_from_entries(", borp_stringify(borp_val_num(borp_length(borp_table_get(expr, "fields"))))), ", (const char*[]){"), keys), "}, (BorpValue[]){"), vals), "})");
}
}
if ((borp_to_num(borp_table_get(expr, "type")) == ARRAY_EXPR)) {
if ((borp_length(borp_table_get(expr, "elements")) == 0)) {
return "borp_array_create(0)";
} else if ((borp_length(borp_table_get(expr, "elements")) > 0)) {
const char* elems = "";
for (double i = 0; i <= (borp_length(borp_table_get(expr, "elements")) - 1); i += 1) {
const char* wrappedVal = boxValue(borp_to_table(borp_array_get(borp_table_get(expr, "elements").as.borp_array, (int)(i))));
if ((i < (borp_length(borp_table_get(expr, "elements")) - 1))) {
elems = borp_concat(borp_concat(elems, wrappedVal), ", ");
} else {
elems = borp_concat(elems, wrappedVal);
}
}
return borp_concat(borp_concat(borp_concat(borp_concat("borp_array_from_elements(", borp_stringify(borp_val_num(borp_length(borp_table_get(expr, "elements"))))), ", (BorpValue[]){"), elems), "})");
}
}
return "NULL";
}

static const char* genStatementList(BorpArray* statements, double fnReturnType) {
const char* out = "";
if ((borp_length(borp_val_array(statements)) > 0)) {
for (double i = 0; i <= (borp_length(borp_val_array(statements)) - 1); i += 1) {
const char* stmtCode = genStatement(borp_to_table(borp_array_get(statements, (int)(i))), fnReturnType);
if (stmtCode != NULL) {
out = borp_concat(out, stmtCode);
}
}
}
return out;
}

static const char* genStatement(BorpTable* statement, double fnReturnType) {
if ((borp_to_num(borp_table_get(statement, "type")) == EXPRESSION_STATEMENT)) {
BorpValue e = borp_table_get(statement, "expression");
if (((borp_to_num(borp_table_get(e.as.borp_table, "type")) == ASSIGN_EXPR) && borp_table_get(e.as.borp_table, "isDeclaration").type != VAL_NULL)) {
const char* valCode = genExpr(borp_to_table(borp_table_get(e.as.borp_table, "value")));
if ((borp_to_num(borp_table_get(e.as.borp_table, "inferredType")) == ANY)) {
valCode = boxValue(borp_to_table(borp_table_get(e.as.borp_table, "value")));
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(cType(borp_to_num(borp_table_get(e.as.borp_table, "inferredType"))), " "), borp_to_text(borp_table_get(e.as.borp_table, "name"))), " = "), valCode), ";\n");
}
return borp_concat(genExpr(borp_to_table(e)), ";\n");
}
if ((borp_to_num(borp_table_get(statement, "type")) == VARDECLARE_STATEMENT)) {
const char* typeStr = cType(borp_to_num(borp_table_get(statement, "typeAnnotation")));
const char* privStr = "";
if (((borp_to_num(borp_table_get(statement, "typeAnnotation")) == ANY) || (strcmp(typeStr, "void") == 0))) {
typeStr = cType(borp_to_num(borp_table_get(borp_table_get(statement, "initializer").as.borp_table, "inferredType")));
}
if ((borp_to_num(borp_table_get(borp_table_get(statement, "initializer").as.borp_table, "type")) == ENUM_EXPR)) {
const char* enumMembers = "";
for (double i = 0; i <= (borp_length(borp_table_get(borp_table_get(statement, "initializer").as.borp_table, "members")) - 1); i += 1) {
BorpTable* member = borp_to_table(borp_array_get(borp_table_get(borp_table_get(statement, "initializer").as.borp_table, "members").as.borp_array, (int)(i)));
if ((i < (borp_length(borp_table_get(borp_table_get(statement, "initializer").as.borp_table, "members")) - 1))) {
enumMembers = borp_concat(borp_concat(borp_concat(borp_concat(enumMembers, borp_to_text(borp_table_get(member, "name"))), " = "), borp_to_text(borp_table_get(member, "value"))), ",\n");
} else {
enumMembers = borp_concat(borp_concat(borp_concat(enumMembers, borp_to_text(borp_table_get(member, "name"))), " = "), borp_to_text(borp_table_get(member, "value")));
}
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(typeStr, " "), borp_to_text(borp_table_get(statement, "name"))), " {\n"), enumMembers), "};\n");
}
if (borp_to_bool(borp_table_get(statement, "isPrivate"))) {
privStr = "static ";
}
const char* valCode = "";
if ((borp_to_num(borp_table_get(statement, "typeAnnotation")) == ANY)) {
valCode = boxValue(borp_to_table(borp_table_get(statement, "initializer")));
} else {
valCode = coerceTo(borp_to_table(borp_table_get(statement, "initializer")), borp_to_num(borp_table_get(statement, "typeAnnotation")));
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(privStr, typeStr), " "), borp_to_text(borp_table_get(statement, "name"))), " = "), valCode), ";\n");
}
if ((borp_to_num(borp_table_get(statement, "type")) == RETURN_STATEMENT)) {
if (borp_table_get(statement, "value").type != VAL_NULL) {
const char* valCode = "";
if ((fnReturnType == ANY)) {
valCode = boxValue(borp_to_table(borp_table_get(statement, "value")));
} else {
valCode = coerceTo(borp_to_table(borp_table_get(statement, "value")), fnReturnType);
}
return borp_concat(borp_concat("return ", valCode), ";\n");
} else {
return "return;\n";
}
}
if ((borp_to_num(borp_table_get(statement, "type")) == BREAK_STATEMENT)) {
return "break;\n";
}
if ((borp_to_num(borp_table_get(statement, "type")) == BLOCK_STATEMENT)) {
return borp_concat(borp_concat("{\n ", genStatementList(borp_to_array(borp_table_get(statement, "statements")), fnReturnType)), "}\n");
}
if ((borp_to_num(borp_table_get(statement, "type")) == WHILE_STATEMENT)) {
const char* condCode = coerceTo(borp_to_table(borp_table_get(statement, "condition")), BOOLEAN);
return borp_concat(borp_concat(borp_concat(borp_concat("while (", condCode), ") {\n"), genStatementList(borp_to_array(borp_table_get(statement, "body")), fnReturnType)), "}\n");
}
if ((borp_to_num(borp_table_get(statement, "type")) == IF_STATEMENT)) {
const char* condCode = coerceTo(borp_to_table(borp_table_get(statement, "condition")), BOOLEAN);
if ((borp_length(borp_table_get(statement, "elseBranch")) > 0)) {
const char* out = borp_concat(borp_concat(borp_concat(borp_concat("if (", condCode), ") {\n"), genStatementList(borp_to_array(borp_table_get(statement, "thenBranch")), fnReturnType)), "}");
if ((borp_length(borp_table_get(statement, "elseBranch")) == 0)) {
out = borp_concat(out, "\n");
return out;
} else if (((borp_length(borp_table_get(statement, "elseBranch")) == 1) && (borp_to_num(borp_table_get(borp_array_get(borp_table_get(statement, "elseBranch").as.borp_array, (int)(0)).as.borp_table, "type")) == IF_STATEMENT))) {
out = borp_concat(borp_concat(out, " else "), genStatement(borp_to_table(borp_array_get(borp_table_get(statement, "elseBranch").as.borp_array, (int)(0))), fnReturnType));
return out;
} else {
out = borp_concat(borp_concat(borp_concat(out, " else {\n"), genStatementList(borp_to_array(borp_table_get(statement, "elseBranch")), fnReturnType)), "}\n");
return out;
}
} else {
return borp_concat(borp_concat(borp_concat(borp_concat("if (", condCode), ") {\n"), genStatementList(borp_to_array(borp_table_get(statement, "thenBranch")), fnReturnType)), "}\n");
}
}
if ((borp_to_num(borp_table_get(statement, "type")) == FOR_STATEMENT)) {
if (borp_to_bool(borp_table_get(statement, "isCStyle"))) {
const char* c1 = genExpr(borp_to_table(borp_table_get(statement, "clause1")));
const char* c2 = genExpr(borp_to_table(borp_table_get(statement, "clause2")));
const char* c3 = genExpr(borp_to_table(borp_table_get(statement, "clause3")));
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("for (", c1), "; "), c2), "; "), c3), ") {\n"), genStatementList(borp_to_array(borp_table_get(statement, "body")), fnReturnType)), "}\n");
} else {
const char* varName = borp_to_str(borp_table_get(borp_table_get(statement, "clause1").as.borp_table, "name"));
const char* startVal = genExpr(borp_to_table(borp_table_get(borp_table_get(statement, "clause1").as.borp_table, "value")));
const char* stopVal = genExpr(borp_to_table(borp_table_get(statement, "clause2")));
const char* stepVal = "1";
if (borp_table_get(statement, "clause3").type != VAL_NULL) {
stepVal = genExpr(borp_to_table(borp_table_get(statement, "clause3")));
}
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat("for (double ", varName), " = "), startVal), "; "), varName), " <= "), stopVal), "; "), varName), " += "), stepVal), ") {\n"), genStatementList(borp_to_array(borp_table_get(statement, "body")), fnReturnType)), "}\n");
}
}
if ((borp_to_num(borp_table_get(statement, "type")) == IMPORT_STATEMENT)) {
return "";
}
return "";
}

static const char* genFunction(BorpTable* statement) {
const char* retType = cType(borp_to_num(borp_table_get(statement, "returnType")));
const char* params = "";
if ((borp_length(borp_table_get(statement, "parameters")) == 0)) {
params = "";
} else {
for (double i = 0; i <= (borp_length(borp_table_get(statement, "parameters")) - 1); i += 1) {
BorpValue p = borp_array_get(borp_table_get(statement, "parameters").as.borp_array, (int)(i));
if ((i < (borp_length(borp_table_get(statement, "parameters")) - 1))) {
params = borp_concat(borp_concat(borp_concat(borp_concat(params, cType(borp_to_num(borp_table_get(p.as.borp_table, "type")))), " "), borp_to_text(borp_table_get(p.as.borp_table, "name"))), ", ");
} else {
params = borp_concat(borp_concat(borp_concat(params, cType(borp_to_num(borp_table_get(p.as.borp_table, "type")))), " "), borp_to_text(borp_table_get(p.as.borp_table, "name")));
}
}
}
if ((! borp_to_bool(borp_table_get(statement, "hasReturn")))) {
retType = "void";
}
if ((borp_length(borp_table_get(statement, "parameters")) == 0)) {
params = "void";
}
if (borp_to_bool(borp_table_get(statement, "isPrivate"))) {
retType = borp_concat("static ", retType);
}
const char* body = genStatementList(borp_to_array(borp_table_get(statement, "body")), borp_to_num(borp_table_get(statement, "returnType")));
return borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(retType, " "), borp_to_text(borp_table_get(statement, "name"))), "("), params), ") {\n"), body), "}\n");
}

static const char* generateProgram(BorpArray* statements) {
const char* headers = "";
const char* functionsCode = "";
const char* mainCode = "";
const char* enumsCode = "";
const char* globalsCode = "";
for (double i = 0; i <= (borp_length(borp_val_array(statements)) - 1); i += 1) {
BorpValue stmt = borp_array_get(statements, (int)(i));
if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == FUNCTION_STATEMENT)) {
functionsCode = borp_concat(borp_concat(functionsCode, genFunction(borp_to_table(stmt))), "\n");
headers = borp_concat(headers, generateHeaders(borp_to_table(stmt)));
} else if ((borp_to_num(borp_table_get(stmt.as.borp_table, "type")) == VARDECLARE_STATEMENT)) {
if ((borp_to_num(borp_table_get(borp_table_get(stmt.as.borp_table, "initializer").as.borp_table, "type")) == ENUM_EXPR)) {
enumsCode = borp_concat(enumsCode, genStatement(borp_to_table(stmt), INTEGER));
} else {
const char* typeStr = cType(borp_to_num(borp_table_get(stmt.as.borp_table, "typeAnnotation")));
if (((borp_to_num(borp_table_get(stmt.as.borp_table, "typeAnnotation")) == ANY) || (strcmp(typeStr, "void") == 0))) {
typeStr = cType(borp_to_num(borp_table_get(borp_table_get(stmt.as.borp_table, "initializer").as.borp_table, "inferredType")));
}
const char* valCode = "";
if ((borp_to_num(borp_table_get(stmt.as.borp_table, "typeAnnotation")) == ANY)) {
valCode = boxValue(borp_to_table(borp_table_get(stmt.as.borp_table, "initializer")));
} else {
valCode = coerceTo(borp_to_table(borp_table_get(stmt.as.borp_table, "initializer")), borp_to_num(borp_table_get(stmt.as.borp_table, "typeAnnotation")));
}
const char* prefix = "";
if (borp_to_bool(borp_table_get(stmt.as.borp_table, "isPrivate"))) {
prefix = "static ";
}
globalsCode = borp_concat(borp_concat(borp_concat(borp_concat(borp_concat(globalsCode, prefix), typeStr), " "), borp_to_text(borp_table_get(stmt.as.borp_table, "name"))), ";\n");
mainCode = borp_concat(borp_concat(borp_concat(borp_concat(mainCode, borp_to_text(borp_table_get(stmt.as.borp_table, "name"))), " = "), valCode), ";\n");
}
} else {
mainCode = borp_concat(mainCode, genStatement(borp_to_table(stmt), INTEGER));
}
}
const char* out = "#include <stdio.h>\n#include <stdbool.h>\n#include <stdlib.h>\n#include <stdint.h>\n#include \"borp_value.h\"\n#include \"borp_array.h\"\n#include \"borp_table.h\"\n#include \"borp_helpers.h\"\n\n";
out = borp_concat(borp_concat(out, enumsCode), "\n");
out = borp_concat(borp_concat(out, globalsCode), "\n");
out = borp_concat(borp_concat(out, headers), "\n");
out = borp_concat(out, functionsCode);
out = borp_concat(out, "int main(int argc, char** argv) {\n");
out = borp_concat(out, "    borp_argc = argc;\n    borp_argv = argv;\n");
out = borp_concat(out, mainCode);
out = borp_concat(out, "    return 0;\n");
out = borp_concat(out, "}\n");
return out;
}

void testCompiler(void) {
const char* inputPath = "src/borpc.borp";
const char* outputPath = "test2.c";
if ((borp_arg_count() >= 1)) {
inputPath = borp_get_arg(0);
}
if ((borp_arg_count() >= 2)) {
outputPath = borp_get_arg(1);
}
const char* source = borp_read_file(inputPath);
BorpArray* tokens = tokenize(borp_val_table(lexerState(source)));
BorpValue ast = parseProgram(parseState(tokens));
BorpTable* visited = borp_table_create(0);
borp_table_set(visited, inputPath, borp_val_boolean(true));
ast = borp_val_array(expandImports(borp_to_array(ast), dirOf(inputPath), visited));
resolveProgram(borp_to_array(ast));
borp_write_file(outputPath, borp_val_string(generateProgram(borp_to_array(ast))));
}

int main(int argc, char** argv) {
    borp_argc = argc;
    borp_argv = argv;
keywords = borp_table_create(0);
borp_table_set(keywords, "if", borp_val_num(TOKEN_IF));
borp_table_set(keywords, "then", borp_val_num(TOKEN_THEN));
borp_table_set(keywords, "else", borp_val_num(TOKEN_ELSE));
borp_table_set(keywords, "elseif", borp_val_num(TOKEN_ELSEIF));
borp_table_set(keywords, "end", borp_val_num(TOKEN_END));
borp_table_set(keywords, "and", borp_val_num(TOKEN_AND));
borp_table_set(keywords, "or", borp_val_num(TOKEN_OR));
borp_table_set(keywords, "fn", borp_val_num(TOKEN_FUNCTION));
borp_table_set(keywords, "function", borp_val_num(TOKEN_FUNCTION));
borp_table_set(keywords, "func", borp_val_num(TOKEN_FUNCTION));
borp_table_set(keywords, "return", borp_val_num(TOKEN_RETURN));
borp_table_set(keywords, "lazy", borp_val_num(TOKEN_LAZY));
borp_table_set(keywords, "while", borp_val_num(TOKEN_WHILE));
borp_table_set(keywords, "do", borp_val_num(TOKEN_DO));
borp_table_set(keywords, "break", borp_val_num(TOKEN_BREAK));
borp_table_set(keywords, "true", borp_val_num(TOKEN_TRUE));
borp_table_set(keywords, "false", borp_val_num(TOKEN_FALSE));
borp_table_set(keywords, "for", borp_val_num(TOKEN_FOR));
borp_table_set(keywords, "import", borp_val_num(TOKEN_IMPORT));
borp_table_set(keywords, "local", borp_val_num(TOKEN_LOCAL));
borp_table_set(keywords, "private", borp_val_num(TOKEN_PRIVATE));
borp_table_set(keywords, "not", borp_val_num(TOKEN_OPPOSE));
borp_table_set(keywords, "null", borp_val_num(TOKEN_NULL));
testCompiler();
    return 0;
}
