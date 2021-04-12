#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;


// it returns the positin of the string. -1 if not found.
int isprint(const string& s){

  if(s.find("print") != string::npos)
  	return s.find("print");

  return -1;
}
int isif(const string& s){

  if(s.find("if") != string::npos)
  	return s.find("if");

  return -1;
}
int isassignment(const string& s){

  if(s.find("=") != string::npos)
  	return s.find("=");

  return -1;
}
int iswhile(const string& s){

  if(s.find("while") != string::npos)
  	return s.find("while");

  return -1;
}

//Removes the white spaces of the string
// Credits to https://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
 void deleteSpaces(string& s){
	s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
}

int main(){
	      
          // checksyntax();


	      //This is the input file.
          ifstream infile;
          infile.open("main.my");

           //This will be the output file.
          ofstream outfile;
          outfile.open("main.ll");
           
          outfile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\0A\00\"\n\n";
          outfile << "define i32 @main()   {";
          
          
          // all lines are put in the vector.
          vector<string> sentences;
          // all variable names are put here.
          unordered_set<string> variables; 
           
           string line;
           while (getline(infile, line)){
           	   
           	     sentences.push_back(line);
           	     if(isassignment(line) != -1){
           	     	line = line.substr(0, line.find("="));
           	     	deleteSpaces(line);

           	     	variables.insert(line);
           	     }
    	     	
           }




           for(string s: variables){
           	 outfile << "    %" << s << " = alloca i32" << endl;
           }

               outfile << "\n\n";

           for(string s: variables){
           	 outfile << "    store i32 0, i32* %" << s << endl;
           }



          outfile << "\n ret i32 0\n}" ;

          infile.close();
          outfile.close();

                  // Just a utility function currently.
           for(int i = 0;i<sentences.size();i++){
           	cout << sentences[i] << " " << isassignment(sentences[i]) << " " << isprint(sentences[i]) << " " << iswhile(sentences[i]) << " " << isif(sentences[i]) << endl;
           }

	return 0;
}