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
int while_if_counter = 1; // Used for creation of while/if bodies.
bool is_in_while = false; //Used for discriminating between while and if bodies, keeps this information in memory.
unordered_set<string> variables; // all variable names are put here.




// creates a new tmp variable name and returns it
string createTmpVariable(){
	return "%_" + to_string(tmp_counter++);
}


////////////////////////////// These functions writes to out file

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

int isCurlyBraces(const string& s){

  	if(s.find("}") != string::npos)
    	return s.find("}");

  	return -1;
}

//Removes the white spaces of the string
// Credits to https://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
void deleteSpaces(string& s){
	s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
}

//Removes the part where there are comments.
void deleteComment(string& s){
	for(int i = 0 ; i < s.length(); i++){
      	if(s[i] == '#')   s = s.substr(0, i);
	}
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
  	int value;
  	if(stringstream(text) >> value){
    	// it is a decimal number.
    	return text;
	}
	//It is something with parantheses. Handles the parantheses part.
    else if(text[0] == '(' && text[text.size() - 1] == ')'){
		return expr(text.substr(1,text.size() - 2)); // (expr) -> expr
    }
  	else{
    // Here I assumed that variable exists and it is already declared. We should improve this part later!!!
    // I now improved i think.
    	if (variables.find(text) == variables.end()){
         	llAlloca("%" + text);
         	llStore("%" + text, "0");
         	variables.insert(text);
    	}
		string tmp = createTmpVariable();
		llLoad("%" + text, tmp);
		return tmp;
  }
}


string term(string text){
  //int pos = text.find_last_of("/*");
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
    //int pos = text.find_last_of("+-");
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
bool assignment(string var_name, string text){
	if (variables.find(var_name) == variables.end()){
		// this variable hasn't been declared before
		llAlloca("%" + var_name);
		variables.insert(var_name);
	}

	string value = expr(text);
	llStore("%" + var_name, value);

	return true;
}

bool print(string text){
	string value = expr(text);
	llPrint(value);

	return true;
}

bool handleWhileCondition(string text){
	string value = expr(text); // %t1
	string result = createTmpVariable();
	outfile << "    " << result << " = icmp ne i32 " << value << ", 0" << endl;
	outfile << "    br i1 "<< result << ", label %body" << while_if_counter <<", label %end" << while_if_counter << endl << endl;
}


bool condition(string text){
	int bracepst1 = text.find("(");
	int bracepst2 = text.find_last_of(")");
	string expression = text.substr(bracepst1 + 1, bracepst2 - bracepst1 - 1);

	outfile << "    br label %cond" << while_if_counter << endl << endl;
	outfile << "cond" << while_if_counter << ":" << endl;
	handleWhileCondition(expression);
	outfile << "body" << while_if_counter << ":" << endl;
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
  

   	string line; //First get all lines.
   	while (getline(infile, line)){
         sentences.push_back(line);
   	}

	for(int i = 0; i < sentences.size(); i++){
	  deleteComment(sentences[i]); // First, delete comments
	  deleteSpaces(sentences[i]); //Second, delete white spaces.
	}

	for(int i=0;i< sentences.size();i++){
		string line = sentences[i]; // Line to be processed.

		if(isassignment(line) != -1){
			int operator_pos = isassignment(line);
			string var_name = line.substr(0, operator_pos);
			string expression = line.substr(operator_pos+1, string::npos);
			assignment(var_name, expression);
		}
		else if(isprint(line) != -1){
			int bracepst1 = line.find("(");
			int bracepst2 = line.find_last_of(")");
			string expression = line.substr(bracepst1 + 1, bracepst2 - bracepst1 - 1);
			print(expression);
		}

		else if(isif(line) != -1){
			is_in_while = false;
			condition(line);
		}

		else if(iswhile(line) != -1){
			is_in_while = true;
			condition(line);
		}

         // End of an if or while statement.
		else if(isCurlyBraces(line) != -1){

          	if(is_in_while){
               	outfile << "    br label %cond" << while_if_counter << endl;
        	}

          	else{
          		outfile<< "    br label %end" << while_if_counter << endl;
        	}
        	outfile << "\n\nend" << while_if_counter++ << ":" << endl;
		}
	}

  	outfile << "\n ret i32 0\n}" ;

  	infile.close();
  	outfile.close();

	return 0;
}