// This code handles parantheses as well. Also initializes the variable as zero in the expression if it is seen for the first time.

#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string expr(string text);

///////////////////////////////////These variables are commonly used in most of the below functions

ofstream outfile;
int tmp_counter = 1;  //Used for creation of temporary variables.
int cnd_counter = 1;  //Used for creation of conditional variables.
int while_if_counter = 1; // Used for creation of while/if bodies.
bool is_in_while = false; //Used for discriminating between while and if bodies, keeps this information in memory.
bool is_in_if = false; //Used for discriminating between while and if bodies, keeps this information in memory.
bool should_terminate = false; // Used for determining syntax errors.
unordered_set<string> variables; // all variable names are put here.
unordered_set<string> cond_variables; // all conditional variable names are put here.



// creates a new tmp variable name and returns it
string createTmpVariable(){
  return "%_" + to_string(tmp_counter++);
}

string createCndVariable(){
  return "v_" + to_string(cnd_counter++); // This will not be a temporary variable.
}

////////////////////////////// These functions writes to out file
      
void llComp(string result, string var1){
    outfile << "    " << result <<  " = icmp eq i32 " <<  var1 << ", 0" << endl;
}

// reutrns (var1 > var2)
void llGreater(string result, string var1){
    outfile << "    " << result <<  " = icmp sgt i32 " <<  var1 << ", 0" << endl;
}

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
  outfile << "    " << result_var << " = sdiv i32 " << value1 << ", " << value2 << endl;
}

/////////////////////////////////////////////////
bool isDigit(char c){
 int ASCIcode = int(c);
 return (ASCIcode <= 57 && ASCIcode >= 48);
}

bool isNum(string text){
if(text.empty())
  return false;

for(int i=0;i<text.length();i++){
    if(!isDigit(text.at(i)))
      return false;
}
return true;
}

bool isLetter(char c){
int ASCIcode = int(c);

return (ASCIcode <= 90 && ASCIcode >= 65) || (ASCIcode <= 122 && ASCIcode >= 97);
}

bool isAlphanumeric(char c){
int ASCIcode = int(c);

return (ASCIcode <= 57 && ASCIcode >= 48) || (ASCIcode <= 90 && ASCIcode >= 65) || (ASCIcode <= 122 && ASCIcode >= 97);
}

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
// it returns the positin of the string. -1 if not found.
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
     //We cannot have a "=" sign anywhere else, hence no problem!!
int isassignment(const string& s){

    if(s.find("=") != string::npos)
      return s.find("=");

  return -1;
}

void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
}

// trim from right
void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
}

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

//Returns the expressions in the choose function. expr1,expr2,expr3 and expr4.
//Nested choose fonksiyonlarını yine parantezle kontrol edebiliyoruz.
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
       //Returns the index of the first available sign.
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
      //Returns the index of the first available sign. 
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

///////////////////////////These functions handles assigments, expressions, mathematical operations,...

string factor(string text){  

    if(text.empty()){
      cout << "found sth empty";
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
      
      //Check clearly that its syntax has this form.
    else if(isChoose(text)){
      
      vector<string> expressions = findExpressions(text); // Su anda dogru calisiyor.
      string expr0 = expressions[0];
      string expr1 = expressions[1];
      string expr2 = expressions[2];
      string expr3 = expressions[3];



       string s = expr(expr0); // s is the name of the variable which keeps the result of the expr1.
      string cond1 = createCndVariable(); // They will be the variables v1, v2 etc..
      string cond2 = createCndVariable(); // Which keeps the boolean information as a i1 variable.
      string cond3 = createCndVariable();
      string num1 = createCndVariable(); // They will be the variables v1, v2 etc..
      string num2 = createCndVariable(); // Which keeps the boolean information as a i32 variable.
      string num3 = createCndVariable();
      
      //variables.insert(cond1); v1
      //variables.insert(cond2); v2
      //variables.insert(cond3); v3
      variables.insert(num1); //v4
      variables.insert(num2); //v5
      variables.insert(num3); //v6
      cond_variables.insert(num1);
      cond_variables.insert(num2);
      cond_variables.insert(num3);


      llComp("%" + cond1, s);  // result1 is the boolean variable which keeps the information.
      llGreater("%" + cond2, s);
      llLess("%" + cond3, s);  
      
      llTypeCast("%" + num1, "%" + cond1);
      llTypeCast("%" + num2, "%" + cond2);
      llTypeCast("%" + num3, "%" + cond3);
                 //v_1*b + v_2*c + v_3*d
      string result = "" + num1 + "*(" + expr1 + ")+" + num2 + "*(" + expr2 + ")+" + num3 + "*(" + expr3 + ")";
      return expr(result);
    }
    else{
    // Here I assumed that variable exists and it is already declared. We should improve this part later!!!
    // I now improved i think.

      if(!isValidVariableName(text) && cond_variables.find(text) == cond_variables.end()){
        outfile << "candidate var name " << text << endl;
        cout << text <<  endl;
         should_terminate = true;
       }


      if (variables.find(text) == variables.end()){
          llAlloca("%" + text);
          llStore("%" + text, "0");
          variables.insert(text);
      }
      // It means it is not a conditional variable.
      if(cond_variables.find(text) == cond_variables.end()){
    string tmp = createTmpVariable();
    llLoad("%" + text, tmp);
    return tmp;
  }
  return "%" + text; //It returns if it is a conditional variable.
  }

}


string term(string text){

   deleteSpaces(text);

  int pos = findLastAvailableMultDiv(text);
   
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


string expr(string text){

     deleteSpaces(text); //Deletes only the beginning and the end.  ( (a  c + )) +     
     //outfile << "expr after deletion" << endl;
     //outfile << text << endl;

  int pos = findLastAvailableAddSub(text);
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

// var_name is without %. Returns false if there is a syntax error
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

void print(string text){
  string value = expr(text);
  llPrint(value);

}

 void handleWhileCondition(string text){
  string value = expr(text); // %t1
  string result = createTmpVariable();
  outfile << "    " << result << " = icmp ne i32 " << value << ", 0" << endl;
  outfile << "    br i1 "<< result << ", label %body" << while_if_counter <<", label %end" << while_if_counter << endl << endl;

}


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

  //This will be the output file.
  outfile.open(argv[2]);

  outfile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n";
  outfile << "define i32 @main()   {\n";


  // all lines are put in the vector.
  vector<string> sentences;
  

    string line; //First get all lines.
    while (getline(infile, line)){
         sentences.push_back(line);
    }

  for(int i = 0; i < sentences.size(); i++){
    deleteComment(sentences[i]); // First, delete comments
    deleteSpaces(sentences[i]); //Second, delete white spaces. at the beginning and the end.
  }

  for(int i=0;i< sentences.size();i++){
    string line = sentences[i]; // Line to be processed.

      // Empty line. Works correctly.
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

      if(should_terminate){
        outfile << "Line " << i <<": syntax error " << endl;
        cout << "Line " << i <<": syntax error " << endl;
        return 0;
      }
    }
    //Print line.
    else if(isprint(line)){
      int bracepst1 = line.find("(");
      int bracepst2 = line.find_last_of(")");
      string expression = line.substr(bracepst1 + 1, bracepst2 - bracepst1 - 1);
      print(expression);
      if(should_terminate){
        outfile << "Line " << i <<": syntax error " << endl;
        cout << "Line " << i <<": syntax error " << endl;
        return 0;
      }
    }

    else if(isif(line)){

          //Nested if/while.
      if(is_in_while || is_in_if)
        should_terminate = true;


      is_in_while = false;
      is_in_if = true;
      condition(line);
      if(should_terminate){
        outfile << "Line " << i <<": syntax error " << endl;
        cout <<  "Line " << i <<": syntax error " << endl;
        return 0;
      }
    }

    else if(iswhile(line)){

         //Nested if/while.
       if(is_in_while || is_in_if)
        should_terminate = true;

      is_in_while = true;
      is_in_if = false;
      condition(line);
      if(should_terminate){
        outfile << "Line " << i <<": syntax error "  << endl;
        cout <<  "Line " << i <<": syntax error "  << endl;
        return 0;
      }
    }

         /// The only possibility is that "}" End of an if/while block.
    else {
               
               //Not a curly braces line.
           if(line != "}")
            should_terminate = true;
                
                //Not in a if/while block.
            if(!(is_in_while || is_in_if))
                 should_terminate = true;


            if(should_terminate){
              outfile << "Line " << i <<": syntax error " << endl;
              cout <<  "Line " << i <<": syntax error " << endl;
              return 0;
             }



            if(is_in_while){
                outfile << "    br label %cond" << while_if_counter << endl;
          }

            else{
              outfile<< "    br label %end" << while_if_counter << endl;
          }
          outfile << "\n\nend" << while_if_counter++ << ":" << endl;

                is_in_while = false;
                is_in_if = false;
    }
            //For unclosed parantheses.
        

  }
    
if(is_in_while || is_in_if){
      outfile << "Line " << sentences.size() - 1 <<": syntax error " << endl;
              cout <<  "Line " << sentences.size() - 1 <<": syntax error " << endl;
    }
      
    outfile << "\n ret i32 0\n}" ;


   // -------This part is for rewriting. -------



    vector<string> normalSentences;
    vector<string> allocateSentences;
    ifstream infile2;
    ofstream outfile2; 
    infile2.open(argv[2]);   
     
     string sentence;

     while (getline(infile2, sentence)){
      if(sentence.find("alloca") == string::npos)
         normalSentences.push_back(sentence);
       else
        allocateSentences.push_back(sentence);
    }
    
    outfile2.open(argv[2]);

    for(int i = 0; i < normalSentences.size(); i++){
      if(i == 5){
        for(int j = 0 ; j < allocateSentences.size(); j++){
              outfile2 << allocateSentences[j] << endl;
              
        }
      }

      outfile2 << normalSentences[i] << endl;
    }
    

    infile.close();
    outfile.close();
    infile2.close();
    outfile2.close();

    // -------This part is for rewriting. -------
    

  return 0;
}