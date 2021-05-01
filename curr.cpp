#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string expr(string text);



////////// There are a bunch of global variables we used in the project. ///////////////
/////////          GLOBAL VARIABLES      ////////////


ofstream outfile;  // It is the output file we use.
int tmp_counter = 1;  //We use them to create temporary variables. This variable is for indexing each number with different value.
int cnd_counter = 1;  //Very similar to above, keeps the number of conditional variables in the program. Conditional variables are the variables we create in our program for choose functions.
int while_if_counter = 1; // Keeps the number of while and if bodies. Increases each time we see an if or while statement.
bool is_in_while = false; // Keeps the information whether we are currently in a while body or not.
bool is_in_if = false; //Keeps the information whether we are currently in an if body or not.
bool should_terminate = false; // This is undoubtedly the most important variable we have. If we see a syntax error, we set its value to true and stop the program as soon as possible.
unordered_set<string> variables; // All variable names in the program except for internal variables(e.g temporary variables etc.) are put in this set.
unordered_set<string> cond_variables; // We create some new variables for our program and keep them in this set. We use them when we see a choose function.

//////////     GLOBAL VARIABLES ///////////////////////      




// creates a new temporary variable name and returns it.
string createTmpVariable(){
  return "%_" + to_string(tmp_counter++);
}

//creates a new conditional variable name and returns it.
string createCndVariable(){
  return "v_" + to_string(cnd_counter++); // This will not be a temporary variable.
}

/////////////////// ************     ////////////////////////////////////

/////     Below are the utility functions we use for writing to outfile. They are very important for readibility of the code. 
////      Otherwise code can be get unscalable quite easily. Their job is quite obvious from their bodies.
      
void llComp(string result, string var1){
    outfile << "    " << result <<  " = icmp eq i32 " <<  var1 << ", 0" << endl;
}

// Whether (var1 > 0) or not.
void llGreater(string result, string var1){
    outfile << "    " << result <<  " = icmp sgt i32 " <<  var1 << ", 0" << endl;
}

// Whether (var1 < 0) or not
void llLess(string result, string var1){
    outfile << "    " << result <<  " = icmp slt i32 " <<  var1 << ", 0" << endl;
}

// Does type casting to var_bool.
void llTypeCast(string result, string var_bool){
    outfile << "    " << result << " = zext i1 " << var_bool << " to i32" << endl;
}
void llAlloca(string var_name){
  outfile << "    " << var_name << " = alloca i32" << endl;
}

void llPrint(string value){
  outfile << "    call i32 (i8*, ...)* @printf(i8* getelementptr ([4x i8]* @print.str, i32 0, i32 0), i32 " << value << ")" << endl;
}

void llStore(string var_name, string value = "0"){
  outfile << "    store i32 " << value << ", i32* " << var_name << endl << endl;
}

void llLoad(string var_from, string var_to){
  outfile << "    " << var_to << " = load i32* " << var_from << endl;
}


void llAdd(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = add i32 " << value1 << ", " << value2 << endl;
}

void llSub(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = sub i32 " << value1 << ", " << value2 << endl;
}

void llMul(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = mul i32 " << value1 << ", " << value2 << endl;
}


void llDiv(string result_var, string value1, string value2){
  outfile << "    " << result_var << " = sdiv i32 " << value1 << ", " << value2 << endl;
}


// Creates an llvm code that is printing "Line <linenum>: sytax error" (<linenum> is the given integer)
//This function is called only if we detect a syntax error. Prints the syntax error and its line.
void llPrintError(string filename, int linenum){
  outfile.close();
  outfile.open(filename);

  outfile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n";
  outfile << "@print.str2 = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"\n\n";
  outfile << "define i32 @main()   {\n";
  outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str2, i32 0, i32 0),  i32 " << linenum << ")\n";
  outfile << "ret i32 0\n}";

}

///////////////////           ***************************     ////////////////////////////////////

/////////////////////         ***************************                     ///////////////////////////////////

/////// Below, there are a lot of boolean functions. They are quite simple functions, however crucial for readibility.
////// Moreover, we understand what we should expect from a string thanks to these functions.


// If c is one of those:   0 1 2 3 4 5 6 7 8 9, the function returns true.
bool isDigit(char c){
 int ASCIcode = int(c);
 return (ASCIcode <= 57 && ASCIcode >= 48);
}

// If the given string can be read as an integer number then it returns true.
bool isNum(string text){
if(text.empty())
  return false;

for(int i=0;i<text.length();i++){
    if(!isDigit(text.at(i)))
      return false;
}
return true;
}

// Whether given letter is in English alphabet or not. [a-zA-Z]
bool isLetter(char c){
int ASCIcode = int(c);

return (ASCIcode <= 90 && ASCIcode >= 65) || (ASCIcode <= 122 && ASCIcode >= 97);
}

// Wheter the given char is alphanumeric. We accept that '_' is not alphanumeric.
bool isAlphanumeric(char c){
int ASCIcode = int(c);

return (ASCIcode <= 57 && ASCIcode >= 48) || (ASCIcode <= 90 && ASCIcode >= 65) || (ASCIcode <= 122 && ASCIcode >= 97);
}

// A valid variable name consists of alphanumeric characters and must start with a letter.
// It can not be empty or any other keywords of mylang ("if", "while", "print", "choose").
bool isValidVariableName(string var_name){
   if(var_name.empty())
    return false;

  if(var_name == "if" || var_name == "while" || var_name == "print" || var_name == "choose" )
    return false;

   if(!isLetter(var_name.at(0)))
    return false;

  for(int i = 1 ; i < var_name.length(); i++){
    if(!isAlphanumeric(var_name.at(i)))
      return false;
  }

   return true; 
}

// Whether the given expr is a valid choose expression. But this function doesn't check validity of expressions in for.
bool isChoose(string expr){
    if(expr.length() < 15) //choose () , , , expr1 expr2 expr3 expr4
    return false;
  
    if(expr.at(0) != 'c' || expr.at(1) != 'h' || expr.at(2) != 'o' || expr.at(3) != 'o'|| expr.at(4) != 's' || expr.at(5) != 'e')
    return false;

    if(expr.at(expr.length() - 1) != ')')
    return false;

    if(expr.find('(') == string::npos || expr.find(')') == string::npos)
    return false;

        //Checks whether there is any character between "choose" and "(" like choosecscd(
    int i = 6;
    while(true){

      if(expr.at(i) == ' ' || expr.at(i) == '\t'){
        i++;
        continue;
      }
      else if(expr.at(i) == '(')
        break;
      else
        return false;
    }

    if(expr.find('(') > expr.find(')') || expr.find_last_of(')') < expr.find_last_of('('))
    return false;


   return true;
}

// Whether the given expr is a valid print expression. But this function doesn't check validity of expression in print.
bool isprint(const string& s){
    if(s.length() < 8) // print ( ) expr 
    return false;

    if(s.at(0) != 'p' || s.at(1) != 'r' || s.at(2) != 'i' || s.at(3) != 'n' || s.at(4) != 't')
        return false;

    if(s.at(s.length() - 1) != ')')
    return false;

    if(s.find('(') == string::npos || s.find(')') == string::npos)
    return false;


      //Checks whether there is any character between "print" and "(" like printacscd(
    int i = 5;
    while(true){

      if(s.at(i) == ' ' || s.at(i) == '\t'){
        i++;
        continue;
      }
      else if(s.at(i) == '(')
        break;
      else
        return false;
    }

    if(s.find('(') > s.find(')') || s.find_last_of(')') < s.find_last_of('('))
    return false;

    return true;
}

// Whether the given expr is a valid if expression. But this function doesn't check validity of expression in if.
bool isif(const string& s){

  if(s.length() < 6) // if ( ) expr {
    return false;

  if(s.at(0) != 'i' || s.at(1) != 'f')
        return false;

  if(s.at(s.length() - 1) != '{')
    return false;

  if(s.find('(') == string::npos || s.find(')') == string::npos)
    return false;

        //Checks whether there is any character between "if" and "(" like ifacscd(
    int i = 2;
    while(true){

      if(s.at(i) == ' ' || s.at(i) == '\t'){
        i++;
        continue;
      }
      else if(s.at(i) == '(')
        break;
      else
        return false;
    }

    if(s.find('(') > s.find(')') || s.find_last_of(')') < s.find_last_of('('))
         return false;

    return true;
}


// Whether the given expr is a valid while expression. But this function doesn't check validity of expression in while.
bool iswhile(const string& s){
  if(s.length() < 9) // while ( ) expr {
    return false;

  if(s.at(0) != 'w' || s.at(1) != 'h' || s.at(2) != 'i' || s.at(3) != 'l' || s.at(4) != 'e')
        return false;

  if(s.at(s.length() - 1) != '{')
    return false;

  if(s.find('(') == string::npos || s.find(')') == string::npos)
    return false;

        //Checks whether there is any character between "while" and "(" like whileacscd(
    int i = 5;
    while(true){

      if(s.at(i) == ' ' || s.at(i) == '\t'){
        i++;
        continue;
      }
      else if(s.at(i) == '(')
        break;
      else
        return false;
    }

    if(s.find('(') > s.find(')') || s.find_last_of(')') < s.find_last_of('('))
         return false;

    return true;
}

// Whether the given expr is expected to be an assignment expression. (contains an '=')
int isassignment(const string& s){
  if(s.find("=") != string::npos)
    return s.find("=");

  return -1;
}



/////////////////////         ***************************                     ///////////////////////////////////







// trims spaces from left of the given string. ex: "   hello  world   " -> "hello  world   "
void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
  s.erase(0, s.find_first_not_of(t));
}

// trims spaces from right of the given string. ex: "   hello  world   " -> "   hello  world"
void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
   s.erase(s.find_last_not_of(t) + 1);
}

// trims spaces from left and right of the given string. ex: "   hello  world   " -> "hello  world"
void deleteSpaces(std::string& s, const char* t = " \t\n\r\f\v")
{
  ltrim(s,t);
  rtrim(s,t);
}
  

//Removes the part where there are comments.
void deleteComment(string& s){
  for(int i = 0 ; i < s.length(); i++){
    if(s[i] == '#')   s = s.substr(0, i);
  }
}

// returns a llvm code line which stores 0 in a variable. parameter "alloca" should be in this format: "<any_string>%<var_name>=<any_string>"
string printStore(string alloca){
  int index1 = alloca.find('%');
  int index2 = alloca.find('=');
  string var_name = alloca.substr(index1 + 1, index2 - index1 - 1);
  deleteSpaces(var_name);

  string result;
  result += "store i32 0, i32* %";
  result += var_name;
  return result;  
}

//Returns a vector of 4 expressions which are exptexted to be in a choose function. returns [expr1, expr2, expr3, expr4].
//If there is a sytax error then expr4 is definitly an empty string. Others may change.
vector<string> findExpressions(string text){
  
  int brace1 = text.find("(");
  int brace2 = text.find_last_of(")");
  vector<int> commaIndexes; //Index of the three commas.

  int bracenum1 = 0;
  int bracenum2 = 0;
  for(int i=brace1+1; i<text.size(); i++){         
    if(text[i] == '(')                             
      bracenum1++;
    if(text[i] == ')')
      bracenum2++;
    if(text[i] == ',' && bracenum1 == bracenum2)
      commaIndexes.push_back(i);
  }
      
  if(commaIndexes.size() < 3){
    should_terminate = true;
    vector<string> vector;
    vector.push_back("");
    vector.push_back("");
    vector.push_back("");
    vector.push_back("");

    return vector;
  }
  vector<string> result;
  result.push_back(text.substr(brace1 + 1, commaIndexes[0] - brace1 - 1));
  result.push_back(text.substr(commaIndexes[0] + 1, commaIndexes[1] - commaIndexes[0] - 1));
  result.push_back(text.substr(commaIndexes[1] + 1, commaIndexes[2] - commaIndexes[1] - 1));
  result.push_back(text.substr(commaIndexes[2] + 1, brace2 - commaIndexes[2] - 1));

  return result;
 }

//Parantheses should match, hence we need such a function. We cannot arbitrarily choose the last sign.
//Returns the index of the first available sign. Here available means a '+' or '-' sign which is not in a pharanthesized expression.
int findLastAvailableAddSub(string s){
  int currBraces1 = 0; // For "("
  int currBraces2 = 0; // For ")"

  for(int i = s.length() -1; i >= 0; i--){
    if(s[i] == '(')
    currBraces1++;
    if(s[i] == ')')
    currBraces2++;
    if((s[i] == '+' || s[i] == '-') && currBraces1 == currBraces2){
    return i;
    }
  }

  return -1; //Couldn't find a proper match.
}


//Parantheses should match, hence we need such a function. We cannot arbitrarily choose the last sign.
//Returns the index of the first available sign.  Here available means a '*' or '/' sign which is not in a pharanthesized expression.
int findLastAvailableMultDiv(string s){
  int currBraces1 = 0; // For "("
  int currBraces2 = 0; // For ")"

  for(int i = s.length() -1; i >= 0; i--){
      if(s[i] == '(')
        currBraces1++;
      if(s[i] == ')')
        currBraces2++;
      if((s[i] == '*' || s[i] == '/') && currBraces1 == currBraces2)
        return i;
  }

  return -1; // Couldn't find a proper match.
}



///////////////////////////These functions handles assignments, expressions, mathematical operations,...

// This is an abstraction of a factor. <factor> -> <integer> | <choose> | <var> | (<expression>)
// Returns the variable name of the factor or an integer.
string factor(string text){  
  if(text.empty()){
     should_terminate = true;
     return text;
  }
    
  deleteSpaces(text);
      
  if(isNum(text)){
    return text;
  }
  //It is something with parantheses. Handles the parantheses part.
  else if(text[0] == '(' && text[text.size() - 1] == ')'){
    return expr(text.substr(1,text.size() - 2)); // (expr) -> expr
  }
      
  //Checks whether it is a choose expression.
  else if(isChoose(text)){
    
    vector<string> expressions = findExpressions(text); // expressions of choose. choose(expr0, expr1, expr2, expr3)
    string expr0 = expressions[0];
    string expr1 = expressions[1];
    string expr2 = expressions[2];
    string expr3 = expressions[3];



    string s = expr(expr0); // s is the name of the variable which keeps the result of the expr1.
    string cond1 = createCndVariable(); // They will be the variables v_1, v_2 etc..
    string cond2 = createCndVariable(); // Which keeps the boolean information as a i1 variable.
    string cond3 = createCndVariable();
    string num1 = createCndVariable(); // They will be the variables v_1, v_2 etc..
    string num2 = createCndVariable(); // Which keeps the boolean information as a i32 variable.
    string num3 = createCndVariable();
    
    variables.insert(num1); //v_4
    variables.insert(num2); //v_5
    variables.insert(num3); //v_6
    cond_variables.insert(num1);
    cond_variables.insert(num2);
    cond_variables.insert(num3);



    llComp("%" + cond1, s); // cond1 is an boolean variable in llvm code which is true if expr==0 (again the comparison is made in llvm code).
    llGreater("%" + cond2, s); // similar to cond1. But cond2 is true if expr>0
    llLess("%" + cond3, s);  // similar to cond1. But cond3 is true if expr<0
    
    // These creates llvm code to typecast bool values to integers.
    llTypeCast("%" + num1, "%" + cond1);  //num1 is an integer having the same value with a bool value cond1
    llTypeCast("%" + num2, "%" + cond2);  //similar
    llTypeCast("%" + num3, "%" + cond3);  //similar
    
    //The idea is that: The value choose returns can be calculated as following:
    //bool1 = (expr0==0)    bool2 = (expr0>0)   bool3 = (expr0<0)
    //choose(expr0, expr1, expr2, expr3) = bool1*expr1 + bool2*expr2 + bool3*expr3   //Here actually a type casting is needed from bool to int
    // We create a mylang expression to calculate this operation. Then we create the llvm code using this mylang line.

    //Here is the maylang code
    string result = "" + num1 + "*(" + expr1 + ")+" + num2 + "*(" + expr2 + ")+" + num3 + "*(" + expr3 + ")";
    return expr(result); //This function creates llvm code of this mylang code and resturns the resultant variable name.
  }
  else{
    if(!isValidVariableName(text) && cond_variables.find(text) == cond_variables.end()){
      //syntax error
       should_terminate = true;
     }

    if (variables.find(text) == variables.end()){
        // if this variable is not allocated yet (it is not in variables set) then allocate it.
        llAlloca("%" + text);
        llStore("%" + text, "0");
        variables.insert(text);
    }
    // It means it is not a conditional variable. (a conditional variable is a specific variable which is crucial for choose function)
    if(cond_variables.find(text) == cond_variables.end()){
      string tmp = createTmpVariable();
      llLoad("%" + text, tmp);
      return tmp;
    }
    return "%" + text; //It returns if it is a conditional variable.
  }

}

// It is an abstraction of term. <term> -> <factor> | <term>*<factor> | <term>/<factor>
// returns a temporary variable which is the result of this term.
string term(string text){
  deleteSpaces(text); //Make sure there is no space at the beginning and end.

  int pos = findLastAvailableMultDiv(text); // position of an appropriate '*' or '/'.
   
  //Not found.
    if(pos == -1){ 
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

// It is an abstraction of expression. <expr> -> <term> | <expr>+<term> | <expr>-<term>
// returns a temporary variable which is the result of this expression.
string expr(string text){

  deleteSpaces(text); //Deletes only the beginning and the end. 

  int pos = findLastAvailableAddSub(text); // position of the nexr appropriate '-' or '+'.
      //Not found
    if(pos == -1){
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

// It is an abstraction of assignment. <asgn> -> <var_name>=<expression>
void assignment(string var_name, string text){

  if(!isValidVariableName(var_name))
    should_terminate = true;


  if (variables.find(var_name) == variables.end()){
    // this variable hasn't been declared before
    llAlloca("%" + var_name);
    variables.insert(var_name);
  }
  string value = expr(text);
  llStore("%" + var_name, value);
}


// It is an aabstraction of print statement. <print> -> print(<epxr>)
void print(string text){
  string value = expr(text);
  llPrint(value);
}

// Creats llvm code to handle condition part of a while loop.
void handleWhileCondition(string text){
  string value = expr(text); // %t1
  string result = createTmpVariable();
  outfile << "    " << result << " = icmp ne i32 " << value << ", 0" << endl;
  outfile << "    br i1 "<< result << ", label %body" << while_if_counter <<", label %end" << while_if_counter << endl << endl;

}

// Creats llvm code for a while loop. Condition and body part are both created. But there is nothing yet in the body part.
void condition(string text){
  int bracepst1 = text.find("(");
  int bracepst2 = text.find_last_of(")");
  string expression = text.substr(bracepst1 + 1, bracepst2 - bracepst1 - 1);

  outfile << "    br label %cond" << while_if_counter << endl << endl;
  outfile << "cond" << while_if_counter << ":" << endl;
  handleWhileCondition(expression);
  outfile << "body" << while_if_counter << ":" << endl;

}



int main(int argc, char const *argv[]){
  //This is the input file.
  ifstream infile;
  infile.open(argv[1]);

  //This is the name of the output file.
  string outfileName = argv[1];
  outfileName = outfileName.substr(0, outfileName.size()-2) + "ll";

  //This is the output file
  outfile.open(outfileName);


  // This part is common for our llvm code.
  outfile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;
  outfile << "define i32 @main()   {" << endl;;


  
  vector<string> sentences; // all lines of mylang are put in this vector.
  
  string line;
  while (getline(infile, line)){
    sentences.push_back(line);
  }

  // Preprocess (remove comments and white spaces at the beginning and the end.)
  for(int i = 0; i < sentences.size(); i++){
    deleteComment(sentences[i]); // First, delete comments
    deleteSpaces(sentences[i]); //Second, delete white spaces. at the beginning and the end.
  }


  //This is the part iterating line by line and creates llvm code.
  for(int i=0;i< sentences.size();i++){
    string line = sentences[i]; // Line to be processed.

    // Empty line.
    if(line.empty()){
      continue;   
      }
    
    //Assignment line.
    else if(isassignment(line) != -1){
      int operator_pos = isassignment(line);   
      string var_name = line.substr(0, operator_pos);
      string expression = line.substr(operator_pos+1, string::npos);
      deleteSpaces(var_name);
      assignment(var_name, expression);
    }

    //Print line.
    else if(isprint(line)){
      int bracepst1 = line.find("(");
      int bracepst2 = line.find_last_of(")");
      string expression = line.substr(bracepst1 + 1, bracepst2 - bracepst1 - 1);
      print(expression);
    }

    //if line
    else if(isif(line)){
      if(is_in_while || is_in_if)   should_terminate = true; //Nested if/while.
      is_in_while = false;
      is_in_if = true;
      condition(line);
    }

    //while line
    else if(iswhile(line)){
      if(is_in_while || is_in_if)    should_terminate = true;  //Nested if/while.
      is_in_while = true;
      is_in_if = false;
      condition(line);
    }

    // The only possibility is that "}" End of an if/while block.
    else {    
      //Not a curly braces line.
      if(line != "}")
        should_terminate = true;
          
      //Not in an if/while block.
      if(!(is_in_while || is_in_if))
        should_terminate = true;

      //There is no error and it is "}" of a while.
      if(is_in_while){
        outfile << "    br label %cond" << while_if_counter << endl;
      }
      else{
        outfile<< "    br label %end" << while_if_counter << endl;
      }
      outfile << endl << endl << "end" << while_if_counter++ << ":" << endl;

      is_in_while = false;
      is_in_if = false;
    }
            //For unclosed parantheses.


    // If the program should terminate (a syntax error is found) then print llvm code which writes the error message
    // and then exit the program.
    if(should_terminate){
      llPrintError(outfileName, i);
      return 0;
    }
  }
   

  // all lines of mylang is read but there is still an '}' missing. Then print llvm code which writes the error message
  // and then exit the program.
  if(is_in_while || is_in_if){
    llPrintError(outfileName, sentences.size() - 1);
    return 0;
  }
  

  // If there is no error in program, these lines are common in all llvm codes which a mylang code would produce.      
  outfile <<  endl << " ret i32 0" << endl;
  outfile << "}" << endl;


  // close the io files.
  infile.close();
  outfile.close();



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // So far in the llvm code which is created, allocation lines may be anywhere of the llvm code. However, all allocation
  // lines should be at the very beginning of the @main function to prevent multiple allocations of the same variables.
  // This part changes places of alloction lines and put them at the very beginning of the @main function.
  vector<string> normalSentences;
  vector<string> allocateSentences;
  ifstream infile2;
  ofstream outfile2; 
  infile2.open(outfileName);   
   
   string sentence;

  while (getline(infile2, sentence)){
    if(sentence.find("alloca") == string::npos)
       normalSentences.push_back(sentence);
    else
      allocateSentences.push_back(sentence);
  }

  infile2.close();

  outfile2.open(outfileName);


     for(int i = 0; i < normalSentences.size(); i++){
       if(i == 5){
        for(int j = 0 ; j < allocateSentences.size(); j++){
               outfile2 << allocateSentences[j]  << endl;
              
         }

        outfile2 << endl;

         for(int j = 0 ; j < allocateSentences.size(); j++){
              outfile2 << printStore(allocateSentences[j]) << endl;
         }
        }

       outfile2 << normalSentences[i] <<  endl;
    }
    
    
    outfile2.close();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

  return 0;
}