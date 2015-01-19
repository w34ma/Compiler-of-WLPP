/*
   CFGrl Version 1 -- converts .cfg-r file to equivalent .cfg file

   Author:   Adam Roegiest
   Version:  1.0

   Input:  .cfg-r file with a single derivation
   Output: .cfg file with abbreviated forward leftmost version of the derivation

   Literal translation of CFGrl Version 2 by Ondrej Lhotak. Available (currently) at:

     http://www.student.cs.uwaterloo.ca/~cs241/a08/CFGrl.java

*/


#include <set>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>

using namespace std;

struct Tree {
    string rule;
    list<Tree*> children;

    ~Tree() {
        for(list<Tree*>::iterator it=children.begin(); it != children.end(); it++) {  // delete all subtrees
            delete (*it);
        }
    }
};

struct token{
   string type;
   string lexeme;
};

//----------------------------------------------------------------------------------------------------------------
set<string> nonterms;

string start;
int devlength = 0;
int translength = 0;
vector<string> empty;
vector<token>  input;

vector<string> nextdev(vector<string> array[],string a,string b){
   for(int i = 0;i < devlength;i++){
      string check1 = *(array[i].begin());
      string check2 = *(array[i].begin()+1);
      if(check1.compare(a)==0 && check2.compare(b)==0){
              return array[i];
      }
   }
      return empty;
}

vector<string> startprint(vector<string> array[],string a){
      for(int i = 0;i<translength;i++){
           string check3= *(array[i].begin());
        if(check3.compare(a)==0){
              return array[i];
        }                        
      }
         return empty;
}

void readinput(){
     string in;
     string s1;
     string s2;
     token startinput;
     startinput.type="BOF" ;
     startinput.lexeme = "BOF";
     input.push_back(startinput);
     while(getline(cin,in)){
         istringstream iss(in);
         iss>>s1;
         iss>>s2;
         token midinput;
         midinput.type=s1 ;
         midinput.lexeme = s2;
         input.push_back(midinput);
     }
     token endinput;
     endinput.type="EOF" ;
     endinput.lexeme = "EOF";
     input.push_back(endinput);

}
//leftmore top to bottom
vector<string> finalout;
void traverse(Tree *t, int d) {
    finalout.push_back(t->rule);
    for(list<Tree*>::iterator it=(t->children).begin(); it != (t->children).end(); it++) {  // print all subtrees
        traverse(*it, d+1);
    }
}

int recordterminal=0;
int record = 0;
//print roots and is it is nonterminal print token
void printfinalout(vector<string> v){
     cout << v[record] << endl; 
     string currentline = v[record];
     istringstream str(currentline);
     string left;
     str>>left;
     vector<string> right;
     string ss;
     while(str>>ss){
        right.push_back(ss);
     }
     int i = 0;
     while(i<right.size()){
         if(nonterms.count(right[i]) != 0){//check whether it belongs to the set of nonterminal tokens
              record++;
              printfinalout(v);
              //for print terminal tokens
         }else{
              cout<< input[recordterminal].type<< " "<<input[recordterminal].lexeme<<endl;
              recordterminal++;
              //for print nonterminal tokens
         }
         i++;
     }
}

vector<vector<string> > prints;

vector<vector<string> > question3(vector<token> input1){
       ifstream file;
       file.open("WLPP.lr1");
       string count;
       string s;
      //skip states
       getline(file,count);
       int limit = atoi(count.c_str());
       for(int i = 0; i < limit;i++){
                 getline(file,s);
       }
      //save non terminal states
       getline(file,count);
       int limit2 = atoi(count.c_str());
       for(int j = 0;j<limit2;j++){
           getline(file,s);
           nonterms.insert(s);
       }
       getline(file,s);
       string startstate = s;
       start = startstate;
       getline(file,count);//count stands for number of trans 
       //tokenize each line substitute rules
       //and save it in trans[]
       //array of vector
       translength = atoi(count.c_str());
       vector<string> trans[translength];
       string token;
       for(int n = 0;n<translength;n++){
            getline(file,token);
            char* cstr = new char[token.size()+1];
            strcpy(cstr,token.c_str());
            char* tok;
            tok = strtok(cstr," ");
          while(tok != NULL){
             trans[n].push_back(tok);
             tok = strtok(NULL," ");
          }
       delete[] cstr;
       }
       //skip
       getline(file,count);
       //read and tokenize all derivations
       //and save them in dev[]
       //array of vectors
       getline(file,count);
       devlength = atoi(count.c_str());
       vector<string> dev[devlength];
       for(int p=0;p<devlength;p++){
             getline(file,token);
             char* cstr1 = new char[token.size()+1];
             strcpy(cstr1,token.c_str());
             char* tok1;
             tok1 = strtok(cstr1," ");
           while(tok1 != NULL){
             dev[p].push_back(tok1);
             tok1 = strtok(NULL," ");
           }
           delete[] cstr1;
        }

        //save and tokenize the last line which we looks as input
         stack<string> statestack;
         stack<string> symbolstack;
         //push in state 0 first
         statestack.push("0");
         
         int b = 0;
         //vector<vector<string> > prints;
         for(b = 0;b<input.size();b++){
             vector<string> next = nextdev(dev,statestack.top(),input1[b].type);
             if(next.size() == 0){
                 cerr << "ERROR at " << b <<endl;
                 exit(1);
             }else if(next[2].compare("shift")==0){
                 statestack.push(next[3]);
                 symbolstack.push(next[1]);
             }else{//reduce
                 int pos = atoi(next[3].c_str());
                 vector<string> ss=trans[pos];
                 prints.push_back(ss);
                 int len = ss.size();
                for(int m = 0;m < len - 1;m++){
                      statestack.pop();
                      symbolstack.pop();
                }
                symbolstack.push(ss[0]);
                next = nextdev(dev,statestack.top(),symbolstack.top());
                statestack.push(next[3]);
                b--;
             }
          }
         file.close();
         vector<string> end;
         end = startprint(trans,startstate);
         prints.push_back(end);
         return prints;
      }

void popper(stack<Tree *> &myStack, list<string> &rhs, string rule) {
    Tree *n = new Tree();
    n->rule = rule;
    for(list<string>::iterator it=rhs.begin(); it != rhs.end(); it++){
        Tree *tmp = myStack.top();
        n->children.push_front(tmp);
        myStack.pop();
    }
    myStack.push(n);
}

vector<string> final;
//combine vector of vector of string to vector string
void connect(vector<vector<string> > p){
    int size = p.size();
     for(int i = 0;i< size;i++){
         string s;
         int smallsize = p[i].size();
         int j;
       for(j=0; j< smallsize-1;j++){  
       	     s = s + p[i][j] + " "; 
       }
       s = s + p[i][j];
       final.push_back(s);
    }
     
}

Tree* lrdo(vector<string> v) {
    stack<Tree*> myStack;
    string l; // lhs symbol
    int i = 0;
    do {
        string f;
        f = v[i];
        list<string> r; // rhs symbols
        istringstream iss(f);
        iss >> l; // lhs symbol
        string s;
        while(iss >> s) {
            if(nonterms.count(s) > 0) r.push_back(s); // only non-terminals
        }
        popper(myStack, r, f); // reduce rule
        i++;
    } while(start != l);
    return myStack.top();
}

int main(){
    readinput();
    connect(question3(input));
    traverse(lrdo(final),0);
    printfinalout(finalout);
}
