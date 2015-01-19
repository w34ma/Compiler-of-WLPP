#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
using namespace std;

//======================================================================
//========= Declarations for the scan() function =======================
//======================================================================

// Each token has one of the following kinds.

enum Kind {
    ID,                 
    NUM,               
    ZERO,               
    null,           
    RETURN,             
    IF,              
    ELSE,             
    WHILE,              
    PRINTLN,            
    WAIN,             
    NEW,              
    DELETE,             
    INT,               
    EQ,            
	 BECOMES,           
    NE,                
    LT,                 
    GT,               
    LE,               
    GE,                
    PLUS,               
    MINUS,              
    STAR,               
    SLASH,              
    PCT,                
    COMMA,              
    SEMI,               
	 AMP,                
    LPAREN,             
    RPAREN,            
    LBRACE,             
    RBRACE,             
    LBRACK,             
    RBRACK,             
    WHITESPACE,
    ERROR       
};

// kindString(k) returns string a representation of kind k
// that is useful for error and debugging messages.
string kindString(Kind k);

// Each token is described by its kind and its lexeme.

struct Token {
    Kind      kind;
    string    lexeme;
};

// scan() separates an input line into a vector of Tokens.
vector<Token> scan(string input);

// =====================================================================
// The implementation of scan() and associated type definitions.
// If you just want to use the scanner, skip to the next ==== separator.

// States for the finite-state automaton that comprises the scanner.

enum State {
    ST_START,
    ST_ID,
    ST_ZERO,
    ST_NUM,
    ST_BECOMES,
    ST_EQ,
    ST_NE,
    ST_EXC, //!
    ST_LT,
    ST_GT,
    ST_LE,
    ST_GE,
    ST_PLUS,
    ST_MINUS,
    ST_STAR,
    ST_SLASH,
    ST_PCT,
    ST_COMMA,
    ST_SEMI,
    ST_LBRACK,
    ST_RBRACK,
	 ST_LPAREN,
    ST_RPAREN,
    ST_LBRACE,
    ST_RBRACE,
    ST_AMP,

	//while
    ST_W,
    ST_WH,
    ST_WHI,
    ST_WHIL,
    ST_WHILE,
	//wain
    ST_WA,
    ST_WAI,
    ST_WAIN,
	//int
    ST_I,
    ST_IN,
    ST_INT,
	//if
    ST_IF,
	//else
    ST_E,
    ST_EL,
    ST_ELS,
    ST_ELSE,
	//println
    ST_P,
    ST_PR,
    ST_PRI,
    ST_PRIN,
    ST_PRINT,
    ST_PRINTL,
    ST_PRINTLN,
	//null
    ST_N,
    ST_NU,
    ST_NUL,
    ST_NULL,
	//new
    ST_Ne,
    ST_NeW,
	//delete
    ST_D,
    ST_DE,
    ST_DEL,
    ST_DELE,
    ST_DELET,
    ST_DELETE,
	//return
    ST_R,
    ST_RE,
    ST_RET,
    ST_RETU,
    ST_RETUR,
    ST_RETURN,
	
    ST_COMMENT,
    ST_WHITESPACE,
	 ST_NO,
	 ST_ERROR
};

// The *kind* of token (see previous enum declaration)
// represented by each state; states that don't represent
// a token have stateKinds == INVALID.

Kind stateKinds[] = {
    ERROR,                
	 ID,			
    ZERO,				
    NUM,				
    BECOMES,		
    EQ,					
    NE,					
    ERROR,             
    LT,			
    GT,			
    LE,					
    GE,				
    PLUS,				
    MINUS,				
    STAR,			
    SLASH,				
    PCT,		
    COMMA,				
    SEMI,			
	 LBRACK,				
    RBRACK,		
	 LPAREN,	    		
    RPAREN, 			
    LBRACE,				
    RBRACE,				
    AMP,             

    ID,     	
    ID,     	
    ID,     			
    ID,     			
    WHILE,				
    ID,     		
    ID,     			
    WAIN,				
    ID,     			
    ID,     
    INT,			
    IF,				
    ID,     		
    ID,     		
    ID,     			
    ELSE,				
    ID,     		
    ID,     		
    ID,     		
    ID,     			
    ID,     
    ID,     			
    PRINTLN,			
    ID,     			
    ID,     	
    ID,     			
    null,				
    ID,     			
    NEW,				
    ID,   
    ID,     		
    ID,     			
    ID,     	
    ID,     		
    DELETE,				
    ID,     			
    ID,     
    ID,     			
    ID,     			
    ID,     		
    RETURN,            
    
    WHITESPACE,			
 	 WHITESPACE,       
	 ERROR,
	 ERROR
};

State delta[ST_WHITESPACE+1][256];

#define whitespace  " \t\n\r"
#define letters     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define firstStart  "ABCDEFGHIJKLMOPQRSTUVWXYZabcfghjklmoqstuvxyz"
#define digits      "0123456789"
#define oneToNine   "123456789"

#define noN "ABCDEFGHIJKLMOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define noU "ABCDEFGHIJKLMNOPQRSTVWXYZabcdefghijklmnopqrstuvwxyz"
#define noL "ABCDEFGHIJKMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define noa "ABCDEFGHIJKLMNOPQRSTUVWXYZbcdefghijklmnopqrstuvwxyz"
#define nod "ABCDEFGHIJKLMNOPQRSTUVWXYZabcefghijklmnopqrstuvwxyz"
#define noe "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdfghijklmnopqrstuvwxyz"
#define nof "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdeghijklmnopqrstuvwxyz"
#define noh "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyz"
#define noi "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghjklmnopqrstuvwxyz"
#define nol "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
#define non "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz"
#define nop "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoqrstuvwxyz"
#define nor "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqstuvwxyz"
#define nos "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrtuvwxyz"
#define no_t "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrsuvwxyz"
#define nou "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstvwxyz"
#define now "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz"
#define noah "ABCDEFGHIJKLMNOPQRSTUVWXYZbcdefgijklmnopqrstuvwxyz"
#define nonf "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdeghijklmopqrstuvwxyz"
#define noUe "ABCDEFGHIJKLMNOPQRSTVWXYZabcdfghijklmnopqrstuvwxyz"

void setT(State from, string chars, State to) {
    for(unsigned i = 0; i < chars.length(); i++ ) delta[from][chars[i]] = to;
}

void initT(){
    int i, j;

    // The default transition is ST_INVALID (i.e., no transition
    // defined for this char).
    for ( i=0; i<=ST_WHITESPACE; i++ ) {
        for ( j=0; j<256; j++ ) {
            delta[i][j] = ST_ERROR;
        }
    }
    // Non-null transitions of the finite state machine.
    // NB: in the third line below, letters digits are macros
    // that are replaced by string literals, which the compiler
    // will concatenate into a single string literal.
    setT( ST_START,      whitespace,     ST_WHITESPACE  );
    setT( ST_WHITESPACE, whitespace,     ST_WHITESPACE  );
    setT( ST_START,      firstStart,     ST_ID          );
    setT( ST_START,      oneToNine,      ST_NUM         );
    setT( ST_START,      "0",            ST_ZERO        );
    setT( ST_ID,         letters digits, ST_ID          );
    setT( ST_NUM,        digits,         ST_NUM         );
    setT( ST_ZERO,       letters digits, ST_NO          );
	
    setT( ST_START,      "=",            ST_BECOMES     );
    setT( ST_BECOMES,    "=",            ST_EQ          );
    setT( ST_START,      "!",            ST_EXC         );
    setT( ST_EXC,        "=",            ST_NE          );
    setT( ST_START,      "<",            ST_LT          );
    setT( ST_LT,         "=",            ST_LE          );
    setT( ST_START,      ">",            ST_GT          );
    setT( ST_GT,         "=",            ST_GE          );
    setT( ST_START,      "+",            ST_PLUS        );
    setT( ST_START,      "-",            ST_MINUS       );
    setT( ST_START,      "*",            ST_STAR        );
    setT( ST_START,      "/",            ST_SLASH       );
    setT( ST_SLASH,      "/",            ST_COMMENT     );
    setT( ST_START,      "%",            ST_PCT         );
    setT( ST_START,      ",",            ST_COMMA       );
    setT( ST_START,      ";",            ST_SEMI        );
    setT( ST_START,      "&",            ST_AMP         );
    setT( ST_START,      "(",            ST_LPAREN      );
    setT( ST_START,      ")",            ST_RPAREN      );
    setT( ST_START,      "{",            ST_LBRACE      );
    setT( ST_START,      "}",            ST_RBRACE      );
    setT( ST_START,      "[",            ST_LBRACK      );
    setT( ST_START,      "]",            ST_RBRACK      );

    setT( ST_START,      "d",            ST_D           );
    setT( ST_D,          "e",            ST_DE          );
    setT( ST_DE,         "l",            ST_DEL         );
    setT( ST_DEL,        "e",            ST_DELE        );
    setT( ST_DELE,       "t",            ST_DELET       );
    setT( ST_DELET,      "e",            ST_DELETE      );
    setT( ST_DELETE,     letters digits, ST_ID          );
	
    setT( ST_START,      "e",            ST_E           );
    setT( ST_E,          "l",            ST_EL          );
    setT( ST_EL,         "s",            ST_ELS         );
    setT( ST_ELS,        "e",            ST_ELSE        );
    setT( ST_ELSE,       letters digits, ST_ID          );
	
    setT( ST_START,      "i",            ST_I           );
    setT( ST_I,          "f",            ST_IF          );
    setT( ST_IF,         letters digits, ST_ID          );
	
    setT( ST_I,          "n",            ST_IN          );
    setT( ST_IN,         "t",            ST_INT         );
    setT( ST_INT,        letters digits, ST_ID          );
	
    setT( ST_START,      "N",            ST_N           );
    setT( ST_N,          "U",            ST_NU       );
    setT( ST_NU,         "L",            ST_NUL       );
    setT( ST_NUL,        "L",            ST_NULL      );
    setT( ST_NULL,       letters digits, ST_ID          );
	
    setT( ST_START,      "n",            ST_N           );
    setT( ST_N,          "e",            ST_Ne          );
    setT( ST_Ne,         "w",            ST_NeW         );
    setT( ST_NeW,        letters digits, ST_ID          );
	
    setT( ST_START,      "w",            ST_W           );
    setT( ST_W,          "a",            ST_WA          );
    setT( ST_WA,         "i",            ST_WAI         );
    setT( ST_WAI,        "n",            ST_WAIN        );
    setT( ST_WAIN,       letters digits, ST_ID          );
	
    setT( ST_W,          "h",            ST_WH          );
    setT( ST_WH,         "i",            ST_WHI         );
    setT( ST_WHI,        "l",            ST_WHIL        );
    setT( ST_WHIL,       "e",            ST_WHILE       );
    setT( ST_WHILE,      letters digits, ST_ID          );
	
	 setT( ST_START,       "p",            ST_P           );
    setT( ST_P,          "r",            ST_PR          );
    setT( ST_PR,         "i",            ST_PRI         );
    setT( ST_PRI,        "n",            ST_PRIN        );
    setT( ST_PRIN,       "t",            ST_PRINT       );
    setT( ST_PRINT,      "l",            ST_PRINTL      );
    setT( ST_PRINTL,     "n",            ST_PRINTLN     );
    setT( ST_PRINTLN,    letters digits, ST_ID          );
	
	 setT( ST_START,       "r",            ST_R           );
    setT( ST_R,          "e",            ST_RE          );
    setT( ST_RE,         "t",            ST_RET         );
    setT( ST_RET,        "u",            ST_RETU        );
    setT( ST_RETU,       "r",            ST_RETUR       );
    setT( ST_RETUR,      "n",            ST_RETURN      );
    setT( ST_RETURN,     letters digits, ST_ID          );

    setT( ST_D,          noe digits,  ST_ID          );
    setT( ST_DE,         nol digits,  ST_ID          );
    setT( ST_DEL,        noe digits,  ST_ID          );
    setT( ST_DELE,       no_t digits,  ST_ID          );
    setT( ST_DELET,      noe digits,  ST_ID          );
	
    setT( ST_W,          noah digits, ST_ID          );
    setT( ST_WA,         noi digits,  ST_ID          );
    setT( ST_WAI,        non digits,  ST_ID          );
	
    setT( ST_I,          nonf digits, ST_ID          );
    setT( ST_IN,         no_t digits,  ST_ID          );
	
    setT( ST_WH,         noi digits,  ST_ID          );
    setT( ST_WHI,        nol digits,  ST_ID          );
    setT( ST_WHIL,       noe digits,  ST_ID          );
	
    setT( ST_E,          nol digits,  ST_ID          );
    setT( ST_EL,         nos digits,  ST_ID          );
    setT( ST_ELS,        noe digits,  ST_ID          ); 
	
    setT( ST_N,          noUe digits,ST_ID          );
    setT( ST_NU,         noL digits,  ST_ID          );
    setT( ST_NUL,        noL digits,  ST_ID          );
	
    setT( ST_Ne,         now digits,  ST_ID          );
	
	 setT( ST_P,          nor digits,  ST_ID          );
    setT( ST_PR,         noi digits,  ST_ID          );
    setT( ST_PRI,        non digits,  ST_ID          );
    setT( ST_PRIN,       no_t digits,  ST_ID          );
    setT( ST_PRINT,      nol digits,  ST_ID          );
    setT( ST_PRINTL,     non digits,  ST_ID          );
	
	 setT( ST_R,          noe digits,  ST_ID          );
    setT( ST_RE,         no_t digits, ST_ID          );
    setT( ST_RET,        nou digits,  ST_ID          );
    setT( ST_RETU,       nor digits,  ST_ID          );
    setT( ST_RETUR,      non digits,  ST_ID          );

    for ( j=0; j<256; j++ ) delta[ST_COMMENT][j] = ST_COMMENT;
}

static int initT_done = 0;

vector<Token> scan(string input){
    // Initialize the transition table when called for the first time.
    if(!initT_done) {
        initT();
        initT_done = 1;
    }

    vector<Token> ret;

    int i = 0;
    int startIndex = 0;
    State state = ST_START;

    if(input.length() > 0) {
        while(true) {
            State nextState = ST_ERROR;
            if(i < input.length())
                nextState = delta[state][(unsigned char) input[i]];
            if(state == ST_NO){
                throw("ERROR ZERO can't be followed by " + input.substr(0, i));
            }
            if(nextState == ST_ERROR){
                if(stateKinds[state] == ERROR){
                    throw("ERROR in lexing after reading " + input.substr(0, i));
                }
                if(stateKinds[state] != WHITESPACE){
                    Token token;
                    token.kind = stateKinds[state];
                    token.lexeme = input.substr(startIndex, i-startIndex);
                    ret.push_back(token);
                }
                startIndex = i;
                state = ST_START;
                if(i >= input.length()) break;
            } else {
                state = nextState;
                i++;
            }
        }
    }

    return ret;
}

// kindString maps each kind to a string for use in error messages.

string kS[] = {
    "ID",
    "NUM",
    "NUM",
    "NULL",
    "RETURN",
    "IF",
    "ELSE",
    "WHILE",
    "PRINTLN",
    "WAIN",
    "NEW",
    "DELETE",
    "INT",
    "EQ",
	 "BECOMES",
    "NE",
    "LT",
    "GT",
    "LE",
    "GE",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PCT",
    "COMMA",
    "SEMI",
	 "AMP",
	 "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACK",
    "RBRACK",
    "WHITESPACE",
    "NUL"
};

string kindString( Kind k ){
    if ( k < ID || k > ERROR ) return "INVALID";
    return kS[k];
}

//======================================================================
//======= A sample program demonstrating the use of the scanner. =======
//======================================================================

int main() {

    try {
        vector<string> srcLines;

        // Read the entire input file, storing each line as a
        // single string in the array srcLines.
        while(true) {
            string line;
            getline(cin, line);
            if(cin.fail()) break;
            srcLines.push_back(line);
        }

        // Tokenize each line, clestoring the results in tokLines.
        vector<vector<Token> > tokLines;

        for(int line = 0; line < srcLines.size(); line++) {
            tokLines.push_back(scan(srcLines[line]));
        }

        // Now we process the tokens.
        // Sample usage: print the tokens of each line.
        for(int line=0; line < tokLines.size(); line++ ) {
            for(int j=0; j < tokLines[line].size(); j++ ) {
                Token token = tokLines[line][j];
                cout << kindString(token.kind) << " "<< token.lexeme << endl;
            }
        }
    } catch(string msg) {
        cerr << msg << endl;
    }

    return 0;
}
