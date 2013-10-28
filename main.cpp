#include <iostream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <string>

//Arrays of chars to check expression against
const char INT[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const char NEST[] = {'(', ')'}; 
const char BASIC[] = {'+', '-', '*', '/'};


//Evaluateds individual statements of the type "INT OP INT" (e.g. 2 + 3 or 4 * 5), Invalid statements will casuse both evalDeque() and evaluate() to fail
int evalStmt(int rvalue, char op, int lvalue){

	switch(op){
	case '+':
		return rvalue + lvalue;
	case '-':
		return rvalue - lvalue;
	case '*':
		return rvalue * lvalue;
	case '/':
		return rvalue / lvalue;
	}
}


/** Evaluated expressions within brackets, or whats left after all brackets are resolved.
	Uses two deques to store ints (including multi-digits) and operator chars, then resolves each operator in order of left-to-right though the deque.
	After each operator is resolved, the op char is removed from opDeque, and the result is pushed to the front of intDeque, replacing the previous two ints.
	Returns true if the sequence in d is valid; if not, it will return false, which will casue evaluate() to return false as well.
*/

bool evalDeque(std::deque<char> d, int &result){
	std::deque<int> intDeque;
	std::deque<char> opDeque;

	while(!d.empty()){
		if(std::any_of(&INT[0], &INT[10], [d](char i){return i == d.front();})){
			std::string converter; //Hold INT chars. In case of multi-digit numbers, this will hold the full number.
			int temp;
			while(std::any_of(&INT[0], &INT[10], [d](char i){if(!d.empty()){return i == d.front();}else{return false;}})){ //Keeps parsing until end of number is reached
				converter.push_back(d.front());
				d.pop_front();
			}
			temp = std::stoi(converter); //Converts back to int.
			intDeque.push_back(temp);
		}else if(std::any_of(&BASIC[0], &BASIC[4], [d](char i){return i == d.front();})){
			opDeque.push_back(d.front());
			d.pop_front();
		}
	}

	if(intDeque.size() != opDeque.size() + 1){ //Shoud alwas be n-1 ops to n ints, eg (1, 1+2, 1+2*3, 1+2*3/4, etc.)
		return false;
	}

	while(!opDeque.empty()){ //Resolves the operators in opDeque
		int rvalue = intDeque.front();
		intDeque.pop_front();
		int lvalue = intDeque.front();
		intDeque.pop_front();
		char op = opDeque.front();
		opDeque.pop_front();

		intDeque.push_front(evalStmt(rvalue, op, lvalue)); //Evaluate statemnts, and place resulut in front of intDeque.
	}

	if(intDeque.size() != 1){ //There shoud be only a single int left; the result
		return false;
	}

	result = intDeque.front();
	return true;

}

//Main evalauation prototype

bool evaluate(const char *expression, int &result) {
	std::deque<char> tokenDeque;
	int temp;
	std::ostringstream converter;

	int parsePoint =  0; //Points to the current char being parsed
	int bracketCount = 0; //Keeps track of unresloved brackets, must be zero at end of function

	while(expression[parsePoint] != '\0'){ //Parse until the null char (Standard end of C style strings)
		if(expression[parsePoint] == NEST[0]){ //Push '(' onto the deque
			tokenDeque.push_back(expression[parsePoint]);
			parsePoint++;
			bracketCount++;
		}else if(isspace(expression[parsePoint])){
			parsePoint++;
		}else if(std::any_of(&INT[0], &INT[10], [expression, parsePoint](char i){return i == expression[parsePoint];})){ //Push int chars onto the deque
			tokenDeque.push_back(expression[parsePoint]);
			parsePoint++;
		}else if(std::any_of(&BASIC[0], &BASIC[4], [expression, parsePoint](char i){return i == expression[parsePoint];})){ //Push op chars onto the deque
			tokenDeque.push_back(expression[parsePoint]);
			parsePoint++;
		}else if(expression[parsePoint] == NEST[1]){ //Close brackets and evaluate the expression within the brackets
			bracketCount--;
			std::deque<char> nestDeque; //Will contain the expression within the brakets
			std::stringstream converter;

			while(tokenDeque.back() != NEST[0]){ 
				nestDeque.push_front(tokenDeque.back());
				tokenDeque.pop_back();
			} 
			tokenDeque.pop_back();

			if(evalDeque(nestDeque, temp) == true){ //A failers in evalDeque will casue this function to fail as well
				std::string converter;
				converter = std::to_string(temp);
				for(char c : converter ){
					if(c != '\0'){
						tokenDeque.push_back(c); //Take result of bracket expression and place it on the main deque, replacing the original bracketed expression
					}
				}
			}else {
				return false;
			}
			parsePoint++;
		}
	}

	if(bracketCount == 0){ //If there is a spare bracket, this will fail
		return evalDeque(tokenDeque, result);
	}else{
		return false;
	}
}


//Main function. Asks for input and desplays the answer, or informs the user of improper inputs. Keeps running until the exit code is entered.
int main(){
	int result;
	bool noError;
	std::string expression;

	std::cout << "Type in the expression you want evaluated, or type quit to exit (Limit of 20 chars): ";
	std::getline(std::cin, expression);

	do{
		noError = evaluate(expression.c_str(), result);
		if(noError){
			std::cout << result << '\n';
		}else{
			std::cout << "Invalid input\n";
		}

		std::cout << "Type in the expression you want evaluated, or type quit to exit (Limit of 20 chars): ";
		std::cin >> expression;
	}while(expression != "quit");
}

