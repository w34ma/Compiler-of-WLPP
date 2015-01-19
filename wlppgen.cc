// Starter code for CS241 assignments 9-11
//
// C++ translation by Simon Parent (Winter 2011),
// based on Java code by Ondrej Lhotak,
// which was based on Scheme code by Gord Cormack.
// Modified July 3, 2012 by Gareth Davies
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

map<string,string> table;
map<string,int> offsettable;
// The set of terminal symbols in the WLPP grammar.
const char *terminals[] = {
  "BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID",
  "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM",
  "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI",
  "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW",
  "DELETE", "NULL"
};
int isTerminal(const string &sym) {
  int idx;
  for(idx=0; idx<sizeof(terminals)/sizeof(char*); idx++)
    if(terminals[idx] == sym) return 1;
  return 0;
}

// Data structure for storing the parse tree.
class tree {
  public:
    string rule;
    vector<string> tokens;
    vector<tree*> children;
    ~tree() { for(int i=0; i<children.size(); i++) delete children[i]; }
};

// Call this to display an error message and exit the program.
void bail(const string &msg) {
  // You can also simply throw a string instead of using this function.
  throw string(msg);
}

// Read and return wlppi parse tree.
tree *readParse(const string &lhs) {
  // Read a line from standard input.
  string line;
  getline(cin, line);
  if(cin.fail())
    bail("ERROR: Unexpected end of file.");
  tree *ret = new tree();
  // Tokenize the line.
  stringstream ss;
  ss << line;
  while(!ss.eof()) {
    string token;
    ss >> token;
    if(token == "") continue;
    ret->tokens.push_back(token);
  }
  // Ensure that the rule is separated by single spaces.
  for(int idx=0; idx<ret->tokens.size(); idx++) {
    if(idx>0) ret->rule += " ";
    ret->rule += ret->tokens[idx];
  }
  // Recurse if lhs is a nonterminal.
  if(!isTerminal(lhs)) {
    for(int idx=1/*skip the lhs*/; idx<ret->tokens.size(); idx++) {
      ret->children.push_back(readParse(ret->tokens[idx]));
    }
  }
  return ret;
}

tree *parseTree;
int off = -4;
// Compute symbols defined in t.
void genSymbols(tree *t) {
  if(t->tokens[0].compare("dcl")==0){
    string type;
    if(t->children[0]->children.size() == 2){
        type = "int*";
    }else{
        type = "int";
    }
    string sym = t->children[1]->tokens[1];
    if(table.count(sym)>0){
       bail("ERROR");
       exit(1);
       //cerr << "ERROR" << endl;//duplicate symbol
    }else{
       table.insert(std::pair<string,string>(sym,type));
       offsettable.insert(std::pair<string,int>(sym,off));
       off = off -4;
    }
  }else if(t->tokens[0].compare("ID")==0){
    string name = t->tokens[1];
    if(table.count(name)<=0){//not in table
       bail("ERROR");
       exit(1);
       //cerr << "ERROR" << endl;//use before defined
    }
  }else{
      for(int i = 0; i < t->children.size();i++){
          genSymbols(t->children[i]);
      }
  }
}
string findtype(tree *t){
  if(t->tokens[0].compare("ID") == 0){
        string type = table[t->tokens[1]];
        //cout << type << endl;
  	return type;
  }else if(t->tokens[0].compare("NUM") == 0){
        return "int";
  }else if(t->tokens[0].compare("NULL") == 0){
        return "int*";
  }else if(t->rule.compare("factor LPAREN expr RPAREN") == 0){
        return findtype(t->children[1]);
  }else if(t->rule.compare("factor AMP lvalue") == 0){
      if(findtype(t->children[1]).compare("int")==0){
            return "int*";
      }else{
            bail("ERROR a");
      }
  }else if(t->rule.compare("factor STAR factor") == 0||t->rule.compare("lvalue STAR factor") == 0){
      if(findtype(t->children[1]).compare("int*")==0){
            return "int";
      }else{
            bail("ERROR b");
      }
  }else if(t->rule.compare("factor NEW INT LBRACK expr RBRACK") == 0){
      if(findtype(t->children[3]).compare("int")==0){
            return "int*";
      }else{
            bail("ERROR c");
      }
  }else if(t->rule.compare("lvalue STAR factor") == 0){
      if(findtype(t->children[1]).compare("int*")){
            return "int";
      }else{
            bail("ERROR d");
      }
  }else if(t->rule.compare("lvalue LPAREN lvalue RPAREN") == 0){
        return findtype(t->children[1]);
  }else if(t->tokens.size() == 2){
//cerr << "ooo" <<endl;
        return findtype(t->children[0]);
  }else if(t->rule.compare("expr expr PLUS term") == 0){
      if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int") == 0){
            return "int";
      }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int") == 0){
            return "int*";
      }else if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int*") == 0){
            return "int*";
      }else{
         bail("ERROR e");
      }
  }else if(t->rule.compare("expr expr MINUS term") == 0){
      if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int") == 0){
            return "int";
      }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int") == 0){
            return "int*";
      }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*") == 0){
            return "int";
      }else{
         bail("ERROR f");
      }
  }else if(t->rule.compare("term term STAR factor") == 0){
        //cout <<findtype(t->children[2]).compare("int") <<endl;
      if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int")==0){
            // cout << t->children[0]<<endl;
             //cout << t->children[2]<<endl;
            return "int";
      }else{
          // cout <<findtype(t->children[0]) <<endl;
          // cout << findtype(t->children[2]) <<endl;
          //  cerr << "opps!" << endl;
            bail("ERROR g");   
      }
  }else if(t->rule.compare("term term SLASH factor") == 0){
      if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int")==0){
            return "int";
      }else{
            bail("ERROR h");   
      }
  }else if(t->rule.compare("term term PCT factor") == 0){
      if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int")==0){
            return "int";
      }else{
            bail("ERROR i");   
      }
  }else if(t->rule.compare("dcl type ID") == 0){
    //  cerr << t->tokens[1]<< endl;
      return findtype(t->children[1]);
  }else{
        //cerr << "lalal" << endl;
        bail("ERROR j");
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int check(tree *t){
  //cerr <<t->tokens.size() <<endl;
      if(t->tokens[0].compare("expr")==0 || t->tokens[0].compare("lvalue")==0){
            findtype(t); 
            return 1;
      }else if(t->rule.compare("statement lvalue BECOMES expr SEMI")==0){//statement
         if(findtype(t->children[0])==findtype(t->children[2])){
            return 1;
         }else{
            //cerr << "1" <<endl;
            bail("ERROR");
            exit(1);
         }
      }else if(t->rule.compare("statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE")==0){
         if(check(t->children[2]) && check(t->children[5]) && check(t->children[9])){
            return 1;
         }else{
//cerr << "2" <<endl;
            bail("ERROR");
            exit(1);      
         }            
      }else if(t->rule.compare("statement WHILE LPAREN test RPAREN LBRACE statements RBRACE")==0){
         if(check(t->children[2]) && check(t->children[5])){
            return 1;
         }else{
//cerr << "3" <<endl;
            bail("ERROR");
            exit(1);      
         }
      }else if(t->rule.compare("statement PRINTLN LPAREN expr RPAREN SEMI")==0){
         if(findtype(t->children[2]).compare("int")==0){
            return 1;
         }else{
//cerr << "4" <<endl;
            bail("ERROR");
            exit(1);      
         }
      }else if(t->rule.compare("statement DELETE LBRACK RBRACK expr SEMI")==0){
         if(findtype(t->children[3]).compare("int*")==0){
            return 1;
         }else{
//cerr << "5" <<endl;
            bail("ERROR");
            exit(1);      
         }
      }else if(t->rule.compare("statements")==0){//statements
//cerr <<"here" <<endl;
           return 1;
      }else if(t->rule.compare("statements statements statement")==0){
         if(check(t->children[0]) && check(t->children[1])){
            return 1;
         }else{
//cerr << "6" <<endl;
            bail("ERROR");
            exit(1);      
         }
      }else if(t->rule.compare("test expr EQ expr")==0 ||//test
               t->rule.compare("test expr NE expr")==0 ||            			
	       t->rule.compare("test expr LT expr")==0 ||
 	       t->rule.compare("test expr LE expr")==0 ||
               t->rule.compare("test expr GE expr")==0 ||
               t->rule.compare("test expr GT expr")==0){
         if(findtype(t->children[0])==findtype(t->children[2])){
            return 1;
         }else{
//cerr << "7" <<endl;
            bail("ERROR");
            exit(1);
         }              
      }else if(t->rule.compare("dcls")==0){//dcls
// cerr << "here" << endl;
           return 1; 
      }else if(t->rule.compare("dcls dcls dcl BECOMES NUM SEMI")==0){
         if(check(t->children[0]) && findtype(t->children[1]).compare("int")==0){
            return 1;
         }else{
//cerr << "8" <<endl;
            bail("ERROR");
            exit(1);
         }           
      }else if(t->rule.compare("dcls dcls dcl BECOMES NULL SEMI")==0){
         //cerr << "aaaa" << endl;
         if(check(t->children[0]) && findtype(t->children[1]).compare("int*")==0){
            return 1;
         }else{
//cerr << "9" <<endl;
            bail("ERROR");
            exit(1);
         }     
      }else if(t->rule.compare("procedure INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE")==0){//procedure
         //cerr <<"aaaaaaaa"<<endl;
        //cerr << t->tokens[6]<< endl;
        // cerr << t->children[8]->rule << endl;
        //cerr << t->children[9]->rule<< endl;
         if(findtype(t->children[5]).compare("int")==0 &&
            check(t->children[8])&&                
	    check(t->children[9])&&
            findtype(t->children[11]).compare("int")==0){
               return 1;
        }else{
//cerr << "10" <<endl;
           bail("ERROR");
           exit(1);
        }             
     }
}

/*void checktype(tree *t){
    for(int i = 0; i<t->children.size();i++){
      if((t->children[i])->tokens[0].compare("expr")==0 || (t->children[i])->tokens[0].compare("lvalue")==0){
          findtype(t->children[i]);         
      }else{ 
          checktype(t->children[i]);
      }
    }
}*/

// Generate the code for the parse tree t.
string sym;
int n=0;
void genCode(tree *t) {
  if(t->rule.compare("procedure INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE")==0){
     cout << ".import print" <<endl;
     cout << ".import init" <<endl;
     cout << ".import new" <<endl;
     cout << ".import delete" <<endl;
     cout << "lis $5" <<endl;
     cout << ".word print" <<endl;
     cout << "lis $4" <<endl;
     cout << ".word 4"<<endl;
     cout << "lis $11" <<endl;
     cout << ".word 1"<<endl;
     cout << "lis $12" <<endl;
     cout << ".word new"<<endl;
     cout << "lis $13" <<endl;
     cout << ".word delete"<<endl;
     cout << "lis $14" <<endl;
     cout << ".word init"<<endl;
     cout << "sub $29,$30,$0"<<endl;
     cout << "sw $1,-4($30)"<<endl;
     cout << "sub $30,$30,$4"<<endl;
     cout << "sw $2,-4($30)"<<endl;
     cout << "sub $30,$30,$4"<<endl;
     if(findtype(t->children[3]).compare("int")==0){
       cout << "add $2,$0,$0" << endl;
     }
     cout << "sw $31,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     cout << "jalr $14" << endl;   
     cout << "add $30,$30,$4" << endl;
     cout << "lw $31,-4($30)" << endl;
     genCode(t->children[8]);
     genCode(t->children[9]);
     genCode(t->children[11]);
     //cout<< "add $30,$29,$0" <<endl;
     cout << "jr $31" <<endl;
  }else if(t->rule.compare("dcls")==0){
     return;
  }else if(t->rule.compare("dcls dcls dcl BECOMES NUM SEMI")==0){
     genCode(t->children[0]);
     genCode(t->children[1]);
     genCode(t->children[3]); 
     cout << "sw $3,-4($30)" <<endl;
     cout << "sub $30,$30,$4" <<endl;
  }else if(t->rule.compare("dcls dcls dcl BECOMES NULL SEMI")==0){
     genCode(t->children[0]);
     genCode(t->children[1]);
     genCode(t->children[3]); 
     cout << "sw $3,-4($30)" <<endl;
     cout << "sub $30,$30,$4" <<endl;
  }else if(t->rule.compare("dcl type ID")==0){
     return;
  }else if(t->rule.compare("statements")==0){
     return;
  }else if(t->rule.compare("statements statements statement")==0){
     genCode(t->children[0]);
     genCode(t->children[1]);
  }else if(t->rule.compare("statement PRINTLN LPAREN expr RPAREN SEMI")==0){
     genCode(t->children[2]);
     cout << "add $1,$3,$0" << endl;
     cout << "sw $31,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     cout << "jalr $5" << endl;   
     cout << "add $30,$30,$4" << endl;
     cout << "lw $31,-4($30)" << endl;
  }else if(t->rule.compare("statement lvalue BECOMES expr SEMI")==0){
    if(t->children[0]->tokens.size() == 2){//ID
     genCode(t->children[0]);
     genCode(t->children[2]);
     cout << "sw $3," << offsettable[sym] << "($29)" <<endl;
    }else{//SATR FACTOR
     genCode(t->children[2]);
     cout << "sw $3,-4($30)" <<endl;
     cout << "sub $30,$30,$4" <<endl;     
     genCode(t->children[0]);
     cout << "add $30,$30,$4" << endl;
     cout << "lw $8,-4($30)" << endl;
     cout<< "sw $8,0($3)"<<endl;
    }
  }else if(t->rule.compare("statement WHILE LPAREN test RPAREN LBRACE statements RBRACE")==0){
     int labelcount = n;
     n++;
     cout << "loop"<<labelcount<<":"<<endl;
     genCode(t->children[2]);
     cout<<"beq $3,$0,endloop"<<labelcount<<endl;
     genCode(t->children[5]);
     cout<<"beq $0,$0,loop"<<labelcount<<endl;
     cout << "endloop"<<labelcount<<":"<<endl;
  }else if(t->rule.compare("statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE")==0){
     int labelcount = n;
     n++;
     genCode(t->children[2]);
     cout<<"beq $3,$0,else"<<labelcount <<endl;
     genCode(t->children[5]);
     cout<<"beq $0,$0,end"<<labelcount<<endl;
     cout<<"else"<<labelcount<<":"<<endl;
     genCode(t->children[9]);
     cout<<"end"<<labelcount<<":"<<endl;
  }else if(t->rule.compare("expr term")==0){
     genCode(t->children[0]);
  }else if(t->rule.compare("expr expr PLUS term")==0){
    if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" <<endl;
     cout << "sub $30,$30,$4" <<endl;
     genCode(t->children[2]);    
     cout << "add $30,$30,$4" <<endl;
     cout << "lw $8,-4($30)" <<endl;
     cout << "add $3,$8,$3" <<endl; 
    }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "mult $3,$4" <<endl;
     cout << "mflo $3"<<endl;
     cout << "add $30,$30,$4" <<endl;
     cout << "lw $8,-4($30)" <<endl;
     cout << "add $3,$8,$3" <<endl; 
    }else if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int*")==0){
     genCode(t->children[2]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[0]);
     cout << "mult $3,$4" <<endl;
     cout << "mflo $3"<<endl;
     cout << "add $30,$30,$4" <<endl;
     cout << "lw $8,-4($30)" <<endl;
     cout << "add $3,$8,$3" <<endl;     
    } 
  }else if(t->rule.compare("expr expr MINUS term")==0){
    if(findtype(t->children[0]).compare("int")==0 && findtype(t->children[2]).compare("int")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);    
     cout << "add $30,$30,$4" << endl;
     cout << "lw $8,-4($30)" << endl;
     cout << "sub $3,$8,$3" << endl; 
    }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "add $30,$30,$4" <<endl;
     cout << "lw $8,-4($30)" <<endl;
     cout << "sub $3,$8,$3" <<endl;
     cout << "div $3,$4"<<endl;
     cout << "mflo $3" <<endl; 
    }else if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "mult $3,$4" <<endl;
     cout << "mflo $3"<<endl;
     cout << "add $30,$30,$4" <<endl;
     cout << "lw $8,-4($30)" <<endl;
     cout << "sub $3,$8,$3" <<endl; 
    }
  }else if(t->rule.compare("term factor")==0){
     genCode(t->children[0]);
  }else if(t->rule.compare("term term STAR factor")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "add $30,$30,$4" << endl;
     cout << "lw $8,-4($30)" << endl;
     cout << "mult $3,$8" << endl;
     cout << "mflo $3" << endl;
  }else if(t->rule.compare("term term SLASH factor")==0){ 
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "add $30,$30,$4" << endl;
     cout << "lw $8,-4($30)" << endl;
     cout << "div $8,$3" << endl;
     cout << "mflo $3" << endl;
  }else if(t->rule.compare("term term PCT factor")==0){
     genCode(t->children[0]);
     cout << "sw $3,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     genCode(t->children[2]);
     cout << "add $30,$30,$4" << endl;
     cout << "lw $8,-4($30)" << endl;
     cout << "div $8,$3" << endl;
     cout << "mfhi $3" << endl;
  }else if(t->rule.compare("factor ID")==0){
     genCode(t->children[0]);
  }else if(t->rule.compare("factor NUM")==0){
     genCode(t->children[0]);
  }else if(t->rule.compare("factor NULL")==0){
     genCode(t->children[0]);
  }else if(t->rule.compare("factor LPAREN expr RPAREN")==0){
     genCode(t->children[1]);
  }else if(t->rule.compare("factor AMP lvalue")==0){
     if(t->children[1]->tokens.size() == 2){//AMP ID
         cout << "lis $3" << endl;
         cout << ".word "<< offsettable[t->children[1]->children[0]->tokens[1]]<<endl;
         cout << "add $3,$3,$29"<<endl;
     }else{//AMP STAR FACTOR
         genCode(t->children[1]);
     }
  }else if(t->rule.compare("factor STAR factor")==0){
     genCode(t->children[1]);
     cout<<"lw $3,0($3)"<<endl;
  }else if(t->tokens[0]=="NUM"){
     cout << "lis $3" << endl;
     cout << ".word " << t->tokens[1] <<endl;
  }else if(t->tokens[0]=="ID"){
     cout << "lw $3," << offsettable[t->tokens[1]] << "($29)" <<endl;
  }else if(t->tokens[0]=="NULL"){
     cout << "add $3,$0,$0" << endl;
  }else if(t->rule.compare("lvalue ID")==0){
     //genCode(t->children[0]);
     sym = t->children[0]->tokens[1];
     return;
  }else if(t->rule.compare("lvalue STAR factor")==0){
     genCode(t->children[1]);
  }else if(t->rule.compare("lvalue LPAREN lvalue RPAREN")==0){
     genCode(t->children[1]);
  }else if(t->rule.compare("test expr LT expr")==0){
     genCode(t->children[0]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[2]);  
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){
       cout << "sltu $3,$6,$3"<<endl;     
     }else{
       cout << "slt $3,$6,$3"<<endl;  
     }
  }else if(t->rule.compare("test expr GT expr")==0){
     genCode(t->children[2]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[0]);  
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){ 
       cout << "sltu $3,$6,$3"<<endl;     
     }else{
       cout << "slt $3,$6,$3"<<endl;  
     }
  }else if(t->rule.compare("test expr GE expr")==0){
     genCode(t->children[0]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[2]); 
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){ 
       cout << "sltu $3,$6,$3"<<endl;     
     }else{  
       cout << "slt $3,$6,$3"<<endl;
     } 
     cout << "sub $3,$11,$3"<<endl;    
  }else if(t->rule.compare("test expr LE expr")==0){
     genCode(t->children[2]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[0]);  
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){ 
       cout << "sltu $3,$6,$3"<<endl;     
     }else{  
       cout << "slt $3,$6,$3"<<endl;
     }  
     cout << "sub $3,$11,$3"<<endl; 
  }else if(t->rule.compare("test expr NE expr")==0){
     genCode(t->children[0]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[2]); 
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){ 
       cout << "sltu $7,$6,$3"<<endl;
       cout << "sltu $9,$3,$6"<<endl;    
     }else{  
       cout << "slt $7,$6,$3"<<endl;
       cout << "slt $9,$3,$6"<<endl; 
     } 
     cout << "add $3,$7,$9"<<endl;
  }else if(t->rule.compare("test expr EQ expr")==0){
     genCode(t->children[0]);
     cout<<"add $6,$3,$0"<<endl;
     genCode(t->children[2]); 
     if(findtype(t->children[0]).compare("int*")==0 && findtype(t->children[2]).compare("int*")==0){ 
       cout << "sltu $7,$6,$3"<<endl;
       cout << "sltu $9,$3,$6"<<endl;    
     }else{  
       cout << "slt $7,$6,$3"<<endl;
       cout << "slt $9,$3,$6"<<endl; 
     }     
     cout << "add $3,$7,$9"<<endl;
     cout << "sub $3,$11,$3 "<<endl;
  }else if(t->rule.compare("type INT STAR")==0){
     return;
  }else if(t->rule.compare("factor NEW INT LBRACK expr RBRACK")==0){
     genCode(t->children[3]);
     cout << "add $1,$3,$0" <<endl;
     cout << "sw $31,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     cout << "jalr $12" << endl;   
     cout << "add $30,$30,$4" << endl;
     cout << "lw $31,-4($30)" << endl;
  }else if(t->rule.compare("statement DELETE LBRACK RBRACK expr SEMI")==0){
     genCode(t->children[3]);
     cout << "add $1,$3,$0" <<endl;
     cout << "sw $31,-4($30)" << endl;
     cout << "sub $30,$30,$4" << endl;
     cout << "jalr $13" << endl;   
     cout << "add $30,$30,$4" << endl;
     cout << "lw $31,-4($30)" << endl;
  }
}

int main() {
  // Main program.
  try {
    parseTree = readParse("S");
    genSymbols(parseTree);
    //cerr << "lalala"<<endl;
    parseTree = parseTree->children[1];
    check(parseTree);
    genCode(parseTree);
  } catch(string msg) {
    cerr << msg << endl;
  }
  if (parseTree) delete parseTree;
  return 0;
}
