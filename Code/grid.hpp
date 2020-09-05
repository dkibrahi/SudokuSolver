#ifndef grid_hpp
#define grid_hpp

#include <iostream>
#include <random>
#include <algorithm>
#include <array>
#include <vector>
#include <chrono>
using namespace std;

class Grid {
    public:
        Grid(int gridVal[9][9]);
        bool findEmptyLocation(int &rowVal, int &colVal); // Scans through the sudoku grid to see if there are any empty spots
        bool usedInRow(int rowVal, int numToCheck); // Checks to see if a possible solution has already been used in the current row.
        bool usedInCol(int colVal, int numToCheck); // Checks to see if a possible solution has already been used in the current column.
        bool usedInSubGrid(int firstRow, int firstCol, int numToCheck); // Checks to see if a possible solution has already been used in the current subgrid.
        bool canPlace(int rowVal, int colVal, int numToCheck); // A simple method that calls on the previous boolean functions to see if the possible solution can be placed.
        bool solveGrid(vector<int> numHolder, ostream &out, bool shouldPrint); // Method that uses backtracking to fill up values in the grid
        void printGrid(ostream &out); // Method that prints out values in the sudoku grid.
        bool notTheSame(Grid s1, Grid s2, Grid s3); // Method to check find unique solutions to a grid.
        void printStats(int numUniqueSolutions, ostream &out); // Function to print metrics regarding the efficiency of the program.
    private:
        int sGrid[9][9] = {0};
        int numComparrisons = 0;
        int numBackTrack = 0;
        int numTimes = 0;
};


Grid::Grid(int gridVal[9][9]) {
    for (int rowVal = 0; rowVal < 9; rowVal++)
        for (int colVal = 0; colVal < 9; colVal++) {
            sGrid[rowVal][colVal] = gridVal[rowVal][colVal]; // Assign the array of gridvalues with their respective coordinates in the grid
    }
}



bool Grid::findEmptyLocation(int &rowVal, int &colVal) {
    for (rowVal = 0; rowVal < 9; rowVal++) {
        for (colVal = 0; colVal < 9; colVal++) {
            if (sGrid[rowVal][colVal] == 0) {
                return true;
            }
        }
    }

    return false;
}


bool Grid::usedInRow(int rowVal, int numToCheck) {
    for (int colVal = 0; colVal < 9; colVal++) {
        if (sGrid[rowVal][colVal] == numToCheck) {
            return true; // If the passed in value is equal to a grid value, that means it has been used in the row.
        }
    }

    return false;
}


bool Grid::usedInCol(int colVal, int numToCheck) {
    for (int rowVal = 0; rowVal < 9; rowVal++) {
        if (sGrid[rowVal][colVal] == numToCheck) {
            return true; // If the passed in value is equal to a grid value, that means it has been used in the col.
        }
    }

    return false;
}


bool Grid::usedInSubGrid(int firstRow, int firstCol, int numToCheck) {
    int potentialMatch = 0;
    
    // Search for all values in a three by three area in the subgrid. If the value is found, return true.
    
    for (int rowExtra = 0; rowExtra < 3; rowExtra++) {
        for (int colExtra = 0; colExtra < 3; colExtra++) {
            potentialMatch = sGrid[rowExtra + firstRow][colExtra + firstCol];
            if (potentialMatch == numToCheck)
                return true;
        }
    }

    return false;

}


bool Grid::canPlace(int rowVal, int colVal, int numToCheck) {
    return !usedInRow(rowVal, numToCheck) && !usedInCol(colVal, numToCheck) && !usedInSubGrid(rowVal - rowVal % 3, colVal - colVal % 3, numToCheck) && sGrid[rowVal][colVal] == 0;
}


bool Grid::solveGrid(vector<int> numHolder, ostream &out, bool shouldPrint) {
    int rowVal, colVal;
    if (!findEmptyLocation(rowVal, colVal))
        return true; // If there are no unassigned values, return true
    
    for (int num = 0; num < 9; num++) {
        numComparrisons++;
        if (canPlace(rowVal, colVal, numHolder[num])) {
            sGrid[rowVal][colVal] = numHolder[num]; // If the number is valid to place, add it to the grid.
            if (solveGrid(numHolder, out, shouldPrint))
                return true;
            
            sGrid[rowVal][colVal] = 0; // If the grid cannot be solved, set that value to 0 so it can be reset.
        }
    }
    
    numBackTrack++;
    return false; // This will cause backtracking. Now the function will go back and try to add the number again.
}


void Grid::printGrid(ostream &out) {
    for (int rowVal = 0; rowVal < 9; rowVal++) {
        if (rowVal % 3 == 0) {
            out << "⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯\n";
        }
        
        out << "⎢ ";
        for (int colVal = 0; colVal < 9; colVal++) {
            out << sGrid[rowVal][colVal];
            if ((colVal + 1) % 3 == 0) {
                out << " ⎢ ";
                
            }
            
            else {
                out << "  ";
            }
            
        }

        out << endl;
    }
    

    out << "⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯\n";


}



bool Grid::notTheSame(Grid original, Grid currentGrid, Grid holder) {
    for (int rowVal = 0; rowVal < 9; rowVal++) {
        for (int colVal = 0; colVal < 9; colVal++) {
            if (holder.sGrid[rowVal][colVal] != currentGrid.sGrid[rowVal][colVal] && original.sGrid[rowVal][colVal] != currentGrid.sGrid[rowVal][colVal]) {
                return true; // If the current grid has a different value from the other grids, then that means the grids are not the same and return true.
            }
        }
    }
    
    return false;
}



void Grid::printStats(int numUniqueSolutions, ostream &out) {
    out << "Here are some nice stats about this sudoku puzzle:\n";
    out << "There were exactly " << numUniqueSolutions << " ways to solve this puzzle!\n";
    out << "Backtracking was used exactly " << numBackTrack << " times!\n";
    out << "There were exactly " << numComparrisons << " number of comparisons!\n";
}


#endif /* grid_hpp */
