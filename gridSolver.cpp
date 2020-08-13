/*
Name of Program: Sudoku Solver
Programmer Name: Kareem Ibrahim
Description: Program made to solve any 9 by 9 sudoku grid as long as it contains at least 17 valid integers.
Date Created: August 7th, 2020
*/

#include <iostream>
#include <vector>
#include <chrono>
#include "grid.hpp"
#include <fstream>
using namespace std;

string greet(); // Function for greeting user
string noFile(ifstream &tStream); // Function for checking if the passed in file is empty and ask user for output file name.
void goodbyeMessage(ostream &out); // Function for the end of the program.
void gridCreator(ifstream &tStream, int pGrid[9][9], ostream &out); // Function to create grid
void addData(string line, vector<int> &lineHolder, ostream &out); // Function that will be called by gridCreator() to add data to the grid.
void validRange(string &userInput, vector<int> &lineHolder, int validStart, int validEnd, ostream &out); // Function that checks if passed in data is in the valid range of 1-9.
int moreSolutions(vector<int> randomRange, ostream &out, int partialGrid[9][9], Grid sudokuGrid); // Function that checks if there are more than one solution to the grid

int main() {
    string fileName =  greet(); // Call greet function, get file name;
    ifstream tStream(fileName);
    ofstream out(noFile(tStream));
    vector<int> randomRange {1, 2, 3, 4, 5, 6, 7, 8, 9}; // Numbers that will try to be added to the grid
    int partialGrid[9][9] = {0};
    int numDiff = 0; // Number of different possible solutions
    
    auto startTime = chrono::high_resolution_clock::now(); // Start tracking time
    gridCreator(tStream, partialGrid, out);
    Grid sudokuGrid(partialGrid);

    auto solBeginningTime = chrono::high_resolution_clock::now(); // Track time to calculate solution
    if (sudokuGrid.solveGrid(randomRange, out, true)) {
        cout << "\n*****PRINTING FINAL GRID*****\n";
        out << "\n*****PRINTING FINAL GRID*****\n";
        sudokuGrid.printGrid(out); // If the grid can be solved, print it out
        numDiff++;
    }

    else {
        cout << "Agh! That’s a bit too tough for me. I couldn’t find a solution\n";
        out << "Agh! That’s a bit too tough for me. I couldn’t find a solution\n";
        return 0;
    }

    auto endSolTime = chrono::high_resolution_clock::now(); // Stop tracking time for solution
    auto startPartialTime = chrono::high_resolution_clock::now(); // Start tracking time

    numDiff = moreSolutions(randomRange, out, partialGrid, sudokuGrid);
    
    auto endTime = chrono::high_resolution_clock::now();
    auto totalTime = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime);
    auto pTime = chrono::duration_cast<chrono::nanoseconds>(endTime - startPartialTime);
    auto solTime = chrono::duration_cast<chrono::nanoseconds>(endSolTime - solBeginningTime);

    sudokuGrid.printStats(numDiff, out, solTime.count(), pTime.count(), totalTime.count());

    goodbyeMessage(out);

    return 0;
}


//Description: Function greets user and asks them to input file name.
//Pre-condition: None
//Post-condition: The user will be greeted and asked to enter the file name.

string greet() {
    string fileName;
    cout << "Welcome to the Sudoku solver. Whatever you can think of, I can solve!\n\n";
    cout << "Please enter the name of your file: ";
    cin >> fileName;
    cout << "\nGive me a second to check your file......\n";
    cout << ".....Checking.......\n\n";
    return fileName;
}

//Description: Function checks for empty file and gets the name of the output file..
//Pre-condition: The name of the file to be checked
//Post-condition: Exit program if the file is empty, otherwise, continue running the program and return the name of the output file.

string noFile(ifstream &tStream) {
    /* Check to see if function's next integer value is equal to the eof.*/

    if (tStream.peek() == ifstream::traits_type::eof()) {
        cout << "UH OH! The file is empty or doesn't exist....." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "ALL CLEAR!\n\n"; // Message to print if the file isn't empty

    string fileName = "output.txt";

    cout << "Please enter the name of the output file in the following format. fileName.txt.\n";
    cout << "File name: ";
    cin >> fileName;
    return fileName;

}

//Description: Function gives goodybe message to user
//Pre-condition: None
//Post-condition: The user will know the program will end.

void goodbyeMessage(ostream &out) {
    cout << "Come on! That was too easy! Give me a better challenge!\n";
    out << "Come on! That was too easy! Give me a better challenge!\n";
}

//Description: Function copies the values from the inputted file and places them into the passed in array
//Pre-condition: The file exists and can be read in. Paramaters passed in are valid and the passed in array is set to 0.
//Post-condition: The passed in array will be filled with 0's or the values passed in from the inputted file.

void gridCreator(ifstream &tStream, int pGrid[9][9], ostream &out) {
    string line;
    int row = 0;
    int col = 0;
    int gridVal = 0;
    vector<int> lineHolder;

    while (getline(tStream, line)) {
        addData(line, lineHolder, out);
        row = lineHolder[0];
        col = lineHolder[1];
        gridVal = lineHolder[2];
        pGrid[row][col] = gridVal; // At the row and col, set the value to the grid val.
        lineHolder.clear(); // Clear the vector so it can be used again.
    }
}

//Description: Function tries to add the inputted value to a vector.
//Pre-condition: The passed in string is only one line from the input file.
//Post-condition: The passed in vector will be filled with 3 values (row, col, and value) if it is valid.

void addData(string line, vector<int> &lineHolder, ostream &out) {
    string newInput = "";
    const int VALID_START_FOR_VAL = 0;
    const int VALID_START_FOR_ELSE = -1;
    const int VALID_END = 10;
    for (int i = 0; i < line.length(); i++) {
        if (line[i] != ' ')
            newInput += line[i]; // While there is no space, add each character to this string

        if (line[i] == ' ') {
            validRange(newInput, lineHolder, VALID_START_FOR_ELSE, VALID_END, out); // Make sure the inputted value is valid
        }

        else if (i == line.length() - 1) {
            validRange(newInput, lineHolder, VALID_START_FOR_VAL, VALID_END, out); // Again, check if the inputted value is valid. Since the last value will be the actual grid value, change the range of valid values.
        }

    }
}

//Description: Function checks if the passed in value is valid.
//Pre-condition: The passed in paramaters for the range are valid, a one line string is passed in.
//Post-condition: The value will be added to the vector if it is valid. Otherwise, an error message will be returned.

void validRange(string &userInput, vector<int> &lineHolder, int validStart, int validEnd, ostream &out) {
    try {
        if (stoi(userInput) > validStart && stoi(userInput) < validEnd) {
            lineHolder.push_back(stoi(userInput)); // Try to add the value to the vector
        }

        else {
            cout << "\'" << stoi(userInput) << "\' is not in the valid range of numbers!\n";
            cout << "WILL NOW TERMINATE THE PROGRAM!\n";
            out << "\'" << stoi(userInput) << "\' is not in the valid range of numbers!\n";
            out << "WILL NOW TERMINATE THE PROGRAM!\n";
            out.flush(); // Flush the ostream so the message gets outputted to the output file
            exit(EXIT_FAILURE);
        }

    }

    catch (...) {
        cout << "You entered an invalid character!\n";
        cout << "WILL NOW TERMINATE THE PROGRAM!";
        out << "You entered an invalid character!\n";
        out << "WILL NOW TERMINATE THE PROGRAM!\n";
        out.flush(); // Flush the ostream so the message gets outputted to the output file
        exit(EXIT_FAILURE);

    }

    userInput = "";
}

int moreSolutions(vector<int> randomRange, ostream &out, int partialGrid[9][9], Grid sudokuGrid) {
    int numDiff = 1;
    int tempHold = 0;
    Grid temp(partialGrid); // Create instance of the grid class
    Grid uniqueChecker(partialGrid);
    uniqueChecker = sudokuGrid; // Create another instance to check if the there are repeat solutions for grids
    
    while (tempHold != 9) {
        tempHold = randomRange[0];
        randomRange.erase(randomRange.begin());
        randomRange.push_back(tempHold); // Change the order of the vector so that numbers in different orders will try to be added
        if (temp.solveGrid(randomRange, out, false) && sudokuGrid.notTheSame(sudokuGrid, temp, uniqueChecker)) {
            cout << "\n*****ANOTHER SOLUTION HAS BEEN FOUND*****\n";
            out << "\n*****ANOTHER SOLUTION HAS BEEN FOUND*****\n";
            temp.printGrid(out); // If there is a new unique solution to the grid, print it
            uniqueChecker = temp; // Change the unique checker so that it holds the new unique solution
            numDiff++;
        }

        temp = Grid(partialGrid); // Reset the instance so it can be solved again with numbers in a different order.

    }
    
    return numDiff;
    
}

