#ifndef GRID_H
#define GRID_H

#include "gridsquare.h"
#include <vector>

using namespace std;

class Grid{
public:
    Grid(unsigned int rows, unsigned int columns): m_rows(rows),m_columns(columns){
        for(unsigned int i = 0; i < m_rows; i++)
        {
            vector<GridSquare *> current_row;
            for(unsigned int j = 0; j < m_columns; j++)
            {
                current_row.push_back(new GridSquare(0,NoCell));
            }
            gridCells.push_back(current_row);
        }
        diffusion_const = 0.4;
        decay_const = 0.1;
    }

    ~Grid(){
        // Delete the cell grid
        for(unsigned int i = 0; i < m_rows; i++)
        {
            for(unsigned int j = 0; j < m_columns; j++){
                delete gridCells[i][j];
            }
        }
    }

    //Helper method for removing values from vector
    void removeValue(vector<unsigned int>& vec, unsigned int val)
    {
        vec.erase(remove(vec.begin(), vec.end(), val), vec.end());
    }

    //add a cell
    bool addCell(unsigned int row, unsigned int column, unsigned int cellType, Cell * cell){
        bool was_cell_empty = !isOccupied(row,column);
        gridCells[row][column]->addCell(cellType);
        gridCells[row][column]->addCellPtr(cell);
        return was_cell_empty;
    }

    //remove a cell from a square and make it unoccupied. doesn't actually delete the cell
    void removeCell(unsigned int row, unsigned int column){
        gridCells[row][column]->addCell(0);
    }

    //Increase/Decrease the amount of Psignal in the square
    void addSignal(unsigned int row, unsigned int column, double value){
        gridCells[row][column]->addSignal(value);
    }

    //checks whether a cell is occupied
    bool isOccupied(unsigned int row, unsigned int column){
        return (gridCells[row][column]->isOccupied());
    }

    void diffusePSignal(){
        GridSquare * neighbour;
        GridSquare * current;
        double sig_value = 0.0;
        for(unsigned int i=0; i < m_rows; i++){
            for(unsigned int j=0; j < m_columns; j++){
                current = gridCells[i][j];
                vector<unsigned int> adjacent_squares = getAdjacentSquares(i,j);
                //Diffuse signal from this one to adjacent squares and reduce signal in this one
                unsigned int num_adjacent = getNumAdjacentSquares(i,j);
                sig_value = current->getSignalValue();
                double sig_per_square = double(diffusion_const * sig_value)/4;
                for(unsigned int k=0; k < num_adjacent; k++){
                    //This is a neighbouring square
                    neighbour = getAdjacent(i,j,adjacent_squares[k]);
                    neighbour->addSignal(sig_per_square);
                }
                //This is the current square
                current->addSignal(-1.0 * double(diffusion_const * sig_value));
            }
        }
    }

    double decayPSignal(){
        double signal_decayed = 0.0;
        for(unsigned int i=0; i < m_rows; i++){
            for(unsigned int j=0; j < m_columns; j++){
                double sig_value = gridCells[i][j]->getSignalValue();
                if(sig_value > 0){
                    double decay = -1*decay_const*sig_value;
                    gridCells[i][j]->addSignal(decay);
                    signal_decayed += decay;
                }
            }
        }
        return signal_decayed;
    }

    //Getters

    map<string,double> getParameters(){
        map<string,double> param_dict;
        param_dict.emplace("DiffusionConstant",diffusion_const);
        param_dict.emplace("DecayConstant",decay_const);
        return param_dict;
    }

    double getSignalValue(unsigned int row, unsigned int column){
        return gridCells[row][column]->getSignalValue();
    }

    double getTotalSignal(){
        double total = 0.0;
        for(unsigned int i = 0; i < m_rows; i++){
            for(unsigned int j = 0; j < m_columns; j++){
                total += getSignalValue(i,j);
            }
        }
        return total;
    }

    unsigned int getRows(){
        return m_rows;
    }
    unsigned int getColumns(){
        return m_columns;
    }

    //Check if a square exists
    bool squareExists(unsigned int row, unsigned int column, unsigned int direction){
        switch(direction){
        case Up:
            return row > 0;
        case Right:
            return column < m_columns - 1;
        case Down:
            return row < m_rows - 1;
        case Left:
            return column > 0;
        case UpRight:
            return (row > 0) && (column < m_columns -1);
        case DownRight:
            return (row < m_rows - 1) && (column < m_columns -1);
        case DownLeft:
            return (row < m_rows - 1) && (column > 0);
        case UpLeft:
            return (row > 0) && (column > 0);
        }
        //Should not reach here
        return false;
    }

    //Just to check if there is a gridsquare there
    vector<unsigned int> getAdjacentSquares(unsigned int row, unsigned int column){
        vector<unsigned int> squares;
        if(squareExists(row,column,Up)){
            squares.push_back(Up);
        }
        if(squareExists(row,column,Right)){
            squares.push_back(Right);
        }
        if(squareExists(row,column,Down)){
            squares.push_back(Down);
        }
        if(squareExists(row,column,Left)){
            squares.push_back(Left);
        }
        return squares;
    }

    unsigned int getNumAdjacentSquares(unsigned int row, unsigned int column){
        unsigned int num = 0;
        if(row > 0){
            num++;
        }
        if(column < m_columns - 1){
            num++;
        }
        if(row < m_rows - 1){
            num++;
        }
        if(column > 0){
            num++;
        }
        return num;
    }

    //Check if there is a cell as well
    vector<unsigned int> getFreeSquares(unsigned int row, unsigned int column){
        vector<unsigned int> squares;
        if(row > 0 && !(gridCells[row-1][column]->isOccupied())){
            squares.push_back(Up);
        }
        if(column < m_columns - 1 && !(gridCells[row][column+1]->isOccupied())){
            squares.push_back(Right);
        }
        if(row < m_rows - 1 && !(gridCells[row+1][column]->isOccupied())){
            squares.push_back(Down);
        }
        if(column > 0 && !(gridCells[row][column-1]->isOccupied())){
            squares.push_back(Left);
        }
        return squares;
    }

    //Get number of empty adjacent squares
    unsigned int getNumFreeSquares(unsigned int row, unsigned int column){
        unsigned int num = 0;
        if(row > 0 && !(gridCells[row-1][column]->isOccupied())){
            num++;
        }
        if(column < m_columns - 1 && !(gridCells[row][column+1]->isOccupied())){
            num++;
        }
        if(row < m_rows - 1 && !(gridCells[row+1][column]->isOccupied())){
            num++;
        }
        if(column > 0 && !(gridCells[row][column-1]->isOccupied())){
            num++;
        }
        return num;
    }

    //Get the square in a specified direction. SQUARE MUST EXIST
    GridSquare * getAdjacent(unsigned int row, unsigned int column, unsigned int direction){
        if(squareExists(row,column,direction)){
            switch(direction){
            case Up:
                return gridCells[row-1][column];
            case Right:
                return gridCells[row][column+1];
            case Down:
                return gridCells[row+1][column];
            case Left:
                return gridCells[row][column-1];
            case UpRight:
                return gridCells[row-1][column+1];
            case DownRight:
                return gridCells[row+1][column+1];
            case DownLeft:
                return gridCells[row+1][column-1];
            case UpLeft:
                return gridCells[row-1][column-1];
            }
        }
        return nullptr;
    }

    //Returns a vector with a row and column coordinate value
    vector<unsigned int> getAdjacentCoords(unsigned int row, unsigned int column, unsigned int direction){
        vector<unsigned int> coords;
        switch(direction){
        case Up:
            coords.push_back(row-1);
            coords.push_back(column);
            return coords;
        case Right:
            coords.push_back(row);
            coords.push_back(column+1);
            return coords;
        case Down:
            coords.push_back(row+1);
            coords.push_back(column);
            return coords;
        case Left:
            coords.push_back(row);
            coords.push_back(column-1);
            return coords;
        }
        return coords;
    }

    //Get number of Tcells adjacent to a Dcell
    vector<unsigned int> getAdjacentToD(unsigned int row, unsigned int column){
        unsigned int num_directions = 8;
        //The actual vector to return considering diagonal directions as well
        vector<unsigned int> adjacent_box;
        for(unsigned int i=0;i < num_directions; ++i){
            if(squareExists(row,column,i)){
                //Square exists so add to the vector
                adjacent_box.push_back(i);
            }
            //Square doesn't exist so do nothing
        }
        return adjacent_box;
    }

protected:
    const unsigned int m_rows;
    const unsigned int m_columns;
    double diffusion_const;
    double decay_const;
    vector<vector<GridSquare *>> gridCells;
};

#endif // GRID_H
