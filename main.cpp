#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;



///////////////////////////////////These variables are commonly used in most of the below functions

ofstream outfile;
int tmp_counter = 1;
unordered_set<string> variables; // all variable names are put here.




// creates a new tmp variable name and returns it
string createTmpVariable(){
  return "%" + to_string(tmp_counter++);
}


////////////////////////////// These functions writes to out file

void llAlloca(string var_name){
  outfile << "    " << var_name << " = alloca i32" << endl;
}

void llStore(string var_name, string value = "0"){
  outfile << "    store i32 " << value << ", i32* " << var_name << endl << endl;
}

// var_to is probably the temporary variable
void llLoad(string var_from, string var_to){
  outfile << "    " << var_to << " = load i32* " << var_from << endl;
}


// Alert!: look there is no "%"  before values
void llAdd(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = add i32 " << value1 << ", " << value2 << endl;
}

// result_var = value1 - value2
void llSub(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = sub i32 " << value1 << ", " << value2 << endl;
}

void llMul(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = mul i32 " << value1 << ", " << value2 << endl;
}

// there are udiv and sdiv. I write udiv belov but we should consider their differences!!
void llDiv(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = udiv i32 " << value1 << ", " << value2 << endl;
}

/////////////////////////////////////////////////

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



///////////////////////////These functions handles assigments, expressions, mathematical operations,...

string factor(string text){
  int value;
  if(stringstream(text) >> value){
    // it is a decimal number.
    return text;
  }
  else{
    // Here I assumed that variable exists and it is already declared. We should improve this part later!!!
    string tmp = createTmpVariable();
    llLoad("%" + text, tmp);
    return tmp;
  }
}


string term(string text){
  int pos = text.find_last_of("/*");

    if(pos == string::npos){
      return factor(text);
    }

    string value1 = term(text.substr(0, pos));
    string value2 = factor(text.substr(pos+1, string::npos));
    string result = createTmpVariable();

    if(text[pos] == '*'){
      llMul(result, value1, value2);
    }
    else if(text[pos] == '/'){
      llDiv(result, value1, value2);
    }
    return result;
}


string expr(string text){
    int pos = text.find_last_of("+-");

    if(pos == string::npos){
      return term(text);
    }

    string value1 = expr(text.substr(0, pos));
    string value2 = term(text.substr(pos+1, string::npos));
    string result = createTmpVariable();

    if(text[pos] == '+'){
      llAdd(result, value1, value2);
    }
    else if(text[pos] == '-'){
      llSub(result, value1, value2);
    }
    return result;
}

// var_name is without %. Returns false if there is a syntax error
bool assignment(string var_name, string text){
  if (variables.find(var_name) == variables.end()){
    // this variable hasn't declared before
    llAlloca("%" + var_name);
    variables.insert(var_name);
  }

  string value = expr(text);
  llStore("%" + var_name, value);

  return true;
}




int main(){
	      
  // checksyntax();


  //This is the input file.
  ifstream infile;
  infile.open("main.my");

   //This will be the output file.
  outfile.open("main.ll");

  outfile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n";
  outfile << "define i32 @main()   {\n";


  // all lines are put in the vector.
  vector<string> sentences;
  

   string line;
   while (getline(infile, line)){
         deleteSpaces(line);
         sentences.push_back(line);

         if(line[0] == '#'){
            //do nothing
         }
         else if(isassignment(line) != -1){
             int operator_pos = isassignment(line);
             string var_name = line.substr(0, operator_pos);
             string expression = line.substr(operator_pos+1, string::npos);

             assignment(var_name, expression);
         }

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