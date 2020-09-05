#include <iostream>
#include <vector>
#include "grid.hpp"
#include <fstream>
using namespace std;

string greet(); // Function for greeting user
string noFile(ifstream &tStream); // Function for checking if the passed in file is empty and ask user for output file name.
void gridCreator(ifstream &tStream, int pGrid[9][9], ostream &out); // Function to create grid
void addData(string line, vector<int> &lineHolder, ostream &out); // Function that will be called by gridCreator() to add data to the grid.
void validRange(string &userInput, vector<int> &lineHolder, int validStart, int validEnd, ostream &out); // Function that checks if passed in data is in the valid range of 1-9.
int moreSolutions(vector<int> randomRange, ostream &out, int partialGrid[9][9], Grid sudokuGrid); // Function that checks if there are more than one solution to the grid

int main() {
    string fileName =  greet(); // Call greet function, get file name;
    ifstream tStream(fileName);
    string outputFileName = noFile(tStream);
    ofstream out(outputFileName);
    vector<int> randomRange {1, 2, 3, 4, 5, 6, 7, 8, 9}; // Numbers that will try to be added to the grid
    int partialGrid[9][9] = {0};
    int numDiff = 0; // Number of different possible solutions
    
    gridCreator(tStream, partialGrid, out);
    Grid sudokuGrid(partialGrid);

    auto solBeginningTime = chrono::high_resolution_clock::now(); // Track time to calculate solution
    if (sudokuGrid.solveGrid(randomRange, out, true)) {
        out << "\n*****PRINTING FINAL GRID*****\n";
        sudokuGrid.printGrid(out); // If the grid can be solved, print it out
        numDiff++;
    }

    else {
        out << "Agh! That’s a bit too tough for me. I couldn’t find a solution\n";
        return 0;
    }


    numDiff = moreSolutions(randomRange, out, partialGrid, sudokuGrid);

    sudokuGrid.printStats(numDiff, out);

    cout << "Go and check " << outputFileName << " for the solution." << endl;
    cout << "Come on! That was too easy! Give me a better challenge!\n";

    return 0;
}


string greet() {
    string fileName;
    cout << "Welcome to the Sudoku solver. Whatever you can think of, I can solve!\n\n";
    cout << "Please enter the name of your file: ";
    cin >> fileName;
    cout << "\nGive me a second to check your file......\n";
    cout << ".....Checking.......\n\n";
    return fileName;
}

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



void gridCreator(ifstream &tStream, int pGrid[9][9], ostream &out) {
    string line;
    int row = 0;
    int col = 0;
    int gridVal = 0;
    int numHints = 0; // Track the number of hints
    vector<int> lineHolder;
    while (getline(tStream, line)) {
        numHints++;
        if (numHints > 81) {
            out << "Too much data! " << numHints << " is not sufficient!"; // Check immediately if there are more than 81 values. Since only a 9 by 9 array is used, 82 or more values will cause an error.
            out.flush(); // Flush the ostream so the message gets outputted to the output file
            exit(EXIT_FAILURE);
        }
        
        addData(line, lineHolder, out);
        
        row = lineHolder[0];
        col = lineHolder[1];
        gridVal = lineHolder[2];
        pGrid[row][col] = gridVal; // At the row and col, set the value to the grid val.
        lineHolder.clear(); // Clear the vector so it can be used again.
    }
    
    if (numHints < 17) {
        out << "Not enough data! " << numHints << " is not sufficient!";
        out.flush(); // Flush the ostream so the message gets outputted to the output file
        exit(EXIT_FAILURE);
    }
    
}


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

void validRange(string &userInput, vector<int> &lineHolder, int validStart, int validEnd, ostream &out) {
    try {
        if (stoi(userInput) > validStart && stoi(userInput) < validEnd) {
            lineHolder.push_back(stoi(userInput)); // Try to add the value to the vector
        }

        else {
            out << "\'" << stoi(userInput) << "\' is not in the valid range of numbers!\n";
            out << "WILL NOW TERMINATE THE PROGRAM!\n";
            out.flush(); // Flush the ostream so the message gets outputted to the output file
            exit(EXIT_FAILURE);
        }

    }

    catch (...) {
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
            out << "\n*****ANOTHER SOLUTION HAS BEEN FOUND*****\n";
            temp.printGrid(out); // If there is a new unique solution to the grid, print it
            uniqueChecker = temp; // Change the unique checker so that it holds the new unique solution
            numDiff++;
        }

        temp = Grid(partialGrid); // Reset the instance so it can be solved again with numbers in a different order.

    }
    
    return numDiff;
    
}
