#ifndef ALLCELLS_H
#define ALLCELLS_H

//include all the types of cell so that they don't have to be added individually in mainwindow
#include "dcell.h"
#include "helpercell.h"
#include "killercell.h"
#include "tcell.h"
#include "grid.h"
#include "fileio.h"
#include <sstream>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <random>
#include <QGraphicsScene>
#include <QPainter>

//Provide functionality for mainwindow to use
class AllCells{
public:
    AllCells(unsigned int rows,
             unsigned int frame_height, string file_name): m_rows(rows), grid(rows, rows),
                                                            frame_height(frame_height),
                                                            file_io(file_name)
    {
        //Initialise starting values
        num_tcells = 0;
        num_hcells = 0;
        num_kcells = 0;
        num_dcells = 0;
        diffusion_cycles = 10;
        num_dead = 0;
        time_elapsed = 0;
        frame_scale = frame_height/m_rows;
        diffuse_signal = true;
        no_flux = true;
    }

    ~AllCells(){
        for(unsigned int i=0; i < num_tcells; i++){
            delete tcells[i];
        }
        for(unsigned int i=0; i < num_dcells; i++){
            delete dcells[i];
        }
    }

    //Helper method for generating random numbers
    unsigned int random(unsigned int range){
        srand(static_cast<unsigned int>(clock()) + unsigned(rand()));
        //srand(time(NULL));
        return unsigned(rand()) % range;
    }

    //Initialise the grid and parameters
    //1st 2 parameters will create randomly spread cells and last 2 parameters will create a square of cells
    void initialise(unsigned int num_h = 0, unsigned int num_k = 0, unsigned int num_d = 0,unsigned int start_pos = 0, unsigned int width = 0){
        starting_nums.push_back(num_h);
        starting_nums.push_back(num_k);
        starting_nums.push_back(num_d);
        //Create and add K cells in a grid
        if(width != 0){
            createAndAddSquare(start_pos, width, KCell);
        }
        if(num_h != 0 || num_k != 0){
            createAndAddRandomly(num_h, HCell);
            createAndAddRandomly(num_k, KCell);
        }
        //Create and add D cells
        createAndAddRandomly(num_d, DCell);
    }

    //Creation/deletion methods

    //Create and add Cell to the grid
    void createAndAddCell(unsigned int row, unsigned int column, unsigned int type){
        unsigned int id = 0;
        unsigned int rad = unsigned(frame_scale/2);
        unsigned int x_coord = column*frame_scale + rad;
        unsigned int y_coord = row*frame_scale - rad;
        switch(type){
            case TCell:
            {
                id = num_tcells;
                Tcell * cell = new Tcell(new Coordinate(row,column,x_coord,y_coord,frame_height),id,rad);
                if(grid.addCell(row,column,type,cell)){
                    tcells.push_back(cell);
                    num_tcells++;
                }
                break;
            }
            case DCell:
            {
                id = num_dcells;
                Dcell * cell = new Dcell(new Coordinate(row,column,x_coord,y_coord,frame_height),id,rad);
                if(grid.addCell(row,column,type,cell)){
                    dcells.push_back(cell);
                    num_dcells++;
                }
                break;
            }
            case HCell:
            {
                id = num_tcells;
                HelperCell *cell = new HelperCell(new Coordinate(row,column,x_coord,y_coord,frame_height),id,rad);
                if(grid.addCell(row,column,type,cell)){
                    tcells.push_back(cell);
                    num_tcells++;
                    num_hcells++;
                }
                break;
            }
            case KCell:
            {
                id = num_tcells;
                KillerCell *cell = new KillerCell(new Coordinate(row,column,x_coord,y_coord,frame_height),id,rad);
                if(grid.addCell(row,column,type,cell)){
                    tcells.push_back(cell);
                    num_tcells++;
                    num_kcells++;
                }
                break;
            }
        }
    }

    void createAndAddRandomly(unsigned int number_of_cells, unsigned int type){
        unsigned int num_added = 0;
        while(num_added < number_of_cells){
            //Generate random position on grid
            unsigned int row = random(m_rows);
            unsigned int column = random(m_rows);
            //Square is already occupied
            if(grid.isOccupied(row,column)){
                continue;
            }
            //Square is not occupied
            createAndAddCell(row,column,type);
            num_added++;
        }
    }

    //Must be called first
    void createAndAddSquare(unsigned int start_pos, unsigned int width, unsigned int type){
        //Create cells and put them in the given places
        for(unsigned int i=start_pos; i < start_pos + width; i++){
            for(unsigned int j=start_pos; j < start_pos + width; j++){
                createAndAddCell(i,j,type);
            }
        }
    }


    //Remove a Tcell from the grid by position
    bool removeTCell(unsigned int position){
        Tcell *tmp = tcells[position];
        unsigned int cell_row = tmp->getRow();
        unsigned int cell_column = tmp->getColumn();
        if(grid.isOccupied(cell_row, cell_column)){
            //Kill the cell
            unsigned int cell_type = tmp->getType();
            grid.addCell(cell_row, cell_column, NoCell,nullptr);
            //qDebug() << "ss2" << "erasing id:" << tmp->getId() << "position: " << position << "size: " << num_tcells;
            vector<Tcell *>::iterator it = tcells.begin();
            tcells.erase(it+position);
            delete tmp;
            num_tcells--;
            switch(cell_type){
            case KCell:
                num_kcells--;
                break;
            case HCell:
                num_hcells--;
                break;
            }
            num_dead++;
            return true;
        }
        return false;
    }

    //Logical checks

    //Given the coordinate and direction to go in, will a wall be hit
    bool willHitWall(unsigned int row, unsigned int column, unsigned int direction){
        switch(direction){
        case Up:
            //cell is near the top
            return (row == 0);
        case Right:
            //cell is near the right wall
            return (column == grid.getColumns() - 1);
        case Down:
            //cell is near the bottom
            return (row == grid.getRows() - 1);
        case Left:
            //cell is near the left wall
            return (column == 0);
        }
        return false;
    }

    //Given a cell's coordinate and direction to go in, will a given Tcell hit another Tcell. CALL THIS.
    bool willCollide(unsigned int row, unsigned int column, unsigned int direction){
        if(!willHitWall(row,column,direction)){
            switch(direction){
            case Up:
                //cell is near the top
                return grid.isOccupied(row-1,column);
            case Right:
                //cell is near the right wall
                return grid.isOccupied(row,column+1);
            case Down:
                //cell is near the bottom
                return grid.isOccupied(row+1,column);
            case Left:
                //cell is near the left wall
                return grid.isOccupied(row,column-1);
            }
            return false;
        }else{
            return true;
        }
    }

    //Update helper methods

    //Returns true only if the cell moves off the grid
    bool moveTcell(Tcell * cell, unsigned int position){
        unsigned int direction = random(4);
        unsigned int random_num = random(100);
        if(random_num >= cell->getPMove() * 100){
            //Do not move
            cell->changeJustMoved(false);
            return false;
        }
        Coordinate cell_coordinate = *cell->getCoordinate();
        unsigned int cell_row = cell_coordinate.getRow();
        unsigned int cell_column = cell_coordinate.getColumn();
        if(!no_flux && willHitWall(cell_row,cell_column,direction)){
            removeTCell(position);
            return true;
        }
        if(willCollide(cell_row, cell_column, direction)){
            //Do not move
            //qDebug() << "Collision!" << cell->getId() << cell_row << cell_column;
            cell->changeJustMoved(false);
            return false;
        }
        switch(direction){
        case Up:
        //update grid
            grid.addCell(cell_row - 1,cell_column,TCell,cell);
            grid.removeCell(cell_row,cell_column);
            //update coordinate
            cell->addPosition(-1,0);
            break;
        case Right: //update grid
            grid.addCell(cell_row,cell_column+1,TCell,cell);
            grid.removeCell(cell_row,cell_column);
            //update coordinate
            cell->addPosition(0,1);
            break;
        case Down: //update grid
            grid.addCell(cell_row + 1,cell_column,TCell,cell);
            grid.removeCell(cell_row,cell_column);
            //update coordinate
            cell->addPosition(1,0);
            break;
        case Left: //update grid
            grid.addCell(cell_row,cell_column-1,TCell,cell);
            grid.removeCell(cell_row,cell_column);
            //update coordinate
            cell->addPosition(0,-1);
            break;
        default:
            ;
        }
        cell->changeJustMoved(true);
        return false;
    }

    double releasePSignal(Tcell * cell){
        if(cell->getType() != HCell){
            return 0;
        }
        Coordinate cell_coordinate = *cell->getCoordinate();
        unsigned int cell_row = cell_coordinate.getRow();
        unsigned int cell_column = cell_coordinate.getColumn();
        //unsigned int num_adjacent = grid.getNumAdjacentSquares(cell_row,cell_column);
        double signal_per_box = double(cell->getPReleaseRate())/4;
        if(signal_per_box <= 0){
            return 0;
        }
        //release up
        if(!willHitWall(cell_row, cell_column, Up)){
            grid.addSignal(cell_row - 1, cell_column, signal_per_box);
        }
        //release right
        if(!willHitWall(cell_row, cell_column, Right)){
            grid.addSignal(cell_row, cell_column + 1, signal_per_box);
        }
        //release left
        if(!willHitWall(cell_row, cell_column, Left)){
            grid.addSignal(cell_row, cell_column - 1, signal_per_box);
        }
        //release down
        if(!willHitWall(cell_row, cell_column, Down)){
            grid.addSignal(cell_row + 1, cell_column, signal_per_box);
        }
        return cell->getPReleaseRate();
    }

    //Tells a cell to absorb the signal in its square
    double absorbPSignal(Tcell * cell){
        Coordinate cell_coordinate = *cell->getCoordinate();
        unsigned int cell_row = cell_coordinate.getRow();
        unsigned int cell_column = cell_coordinate.getColumn();
        //Get the current signal value
        double current_signal = grid.getSignalValue(cell_row,cell_column);
        //Absorb the desired proportion
        double sucked_up = cell->getPAbsorbRate() * current_signal;
        cell->setPSignal(sucked_up);
        grid.addSignal(cell_row,cell_column,-sucked_up);
        return sucked_up;
    }

    //Splitting logic

    double divideProbability(Tcell * cell){
        return (1 - exp(-1*cell->getDivideParam()*cell->getTotalP()));
    }

    void splitCell(Tcell * cell){
        Coordinate cell_coordinate = *cell->getCoordinate();
        unsigned int cell_row = cell_coordinate.getRow();
        unsigned int cell_column = cell_coordinate.getColumn();
        unsigned int num_free = grid.getNumFreeSquares(cell_row, cell_column);
        //There is space for division
        if(num_free == 0){
            return;
        }
        //See if the cell will divide
        unsigned int random_num = random(100);
        bool willDivide = false;
        if(cell->getType() == KCell){
            if(random_num < divideProbability(cell) * 100){
                willDivide = true;
            }
        }else if(cell->getType() == HCell){
            if(random_num < cell->getDivideParam() * 100){
                willDivide = true;
            }
        }
        //Divide the cell
        if(willDivide){
            vector<unsigned int> free_squares = grid.getFreeSquares(cell_row, cell_column);
            //Which square to divide unsigned into
            unsigned int random_2 = random(num_free);
            vector<unsigned int> adjacent = grid.getAdjacentCoords(cell_row, cell_column, free_squares[random_2]);
            createAndAddCell(adjacent[0],adjacent[1],cell->getType());
        }

    }

    //Death logic

    bool cellDeath(Tcell * cell){
        unsigned int random_num = random(100);
        if(random_num < cell->getDeathRate()*100){
            //Kill the cell
            return true;
        }
        //Don't kill the cell
        return false;
    }

    //DCell logic (non-diffusive case)

    void updateDCells(Dcell * cell){
        //Initialise values
        unsigned int cell_row = cell->getRow();
        unsigned int cell_column = cell->getColumn();
        unsigned int num_stuck_k = 0;
        unsigned int num_stuck_h = 0;
        //Loop through adjacent squares
        vector<unsigned int> adjacentToD = grid.getAdjacentToD(cell_row,cell_column);
        vector<Cell *> adjacentKCells;
        //Sticking and unsticking
        for(unsigned int i=0; i < adjacentToD.size(); ++i){
            GridSquare * square = grid.getAdjacent(cell_row,cell_column,adjacentToD[i]);
            //General T cell
            if(square->isOccupiedByTCell()){
                //There is a T cell next to the DC so check if it sticks/unsticks
                Cell * current_cell = square->getCellPtr();
                stickTCell(current_cell);
                //Helper T cell
                if(current_cell->getType() == HCell){
                    //Release signal
                    if(square->getCellPtr()->getIsBound() == true){
                        cell->addPAvailable(square->getCellPtr()->getPReleaseRate());
                        num_stuck_h++;
                    }
                }
                //Killer T cell: count how many of them are stuck to the DCell
                if(current_cell->getType() == KCell){
                    if(square->getCellPtr()->getIsBound() == true){
                        num_stuck_k++;
                    }
                }
            }
        }

        //No signal absorbed
        if((cell->getPAvailable() <= 0) || (num_stuck_k == 0)){
            //Reset p available for next round. Signal is 'lost'
            cell->setPAvailable(0);
            return;
        }

        double signal_per_cell = (cell->getPAvailable())/double(num_stuck_k);

        //Signal is non-zero, so we divide it amongst the stuck cells
        for(unsigned int i=0; i < adjacentToD.size(); ++i){
            GridSquare * square = grid.getAdjacent(cell_row,cell_column,adjacentToD[i]);
            //Killer T cell: Absorb signal and divide
            if(square->isOccupied()){
                Cell * cell_ptr = square->getCellPtr();
                //Absorb signal
                if(cell_ptr->getType() == KCell && cell_ptr->getIsBound() == true){
                    cell_ptr->setPSignal(signal_per_cell);
                    cell->addPAvailable(-signal_per_cell);
                }
            }
        }
    }

    //Sticking logic

    void stickTCell(Cell * current_cell){
        unsigned int random_num = random(100);
        if(current_cell->getIsBound() == false){
            //Binding logic
            double p_bind = current_cell->getPBind();
            if(random_num <= p_bind * 100){
                //Bind by making cell unable to move and changing the boolean flag
                current_cell->setPMove(0);
                current_cell->setIsBound(true);
            }
        }else{
            //Unbinding logic
            double p_unbind = current_cell->getPUnbind();
            if(random_num <= p_unbind * 100){
                //Unbind by making cell able to move and changing the boolean flag
                current_cell->setPMove(current_cell->getPMovePrev());
                current_cell->setIsBound(false);
                if(current_cell->getType() == KCell){
                    current_cell->setPSignal(0);
                }
            }
        }
    }

    //Update methods

    //Call the render function of each cell
    void renderAll(QPainter &painter, unsigned int time){
        //render t cells
        for(unsigned int i=0; i<num_tcells; i++){
            tcells[i]->render(painter,time);
        }
        //render d cells
        for(unsigned int i=0; i<num_dcells; i++){
            dcells[i]->render(painter,time);
        }
    }

    //Do the simulation logic
    bool updateAll(){
        //DCELL START

        if(!diffuse_signal){
            //Loop through DC's
            for(unsigned int i = 0; i < num_dcells; ++i){
                current_dcell = dcells[i];
                //Perform sticking and signalling logic
                updateDCells(current_dcell);
            }
        }
        //DCELL END

        //TCELL START

        //Don't want the newly created tcells to do anything
        unsigned int starting_num = num_tcells;
//        double signal_released = 0.0;
//        double signal_sucked = 0.0;
//        double signal_decayed = 0.0;

        if(num_kcells == 0){
            return false;
        }

//        qDebug() << "Before Changes: " << grid.getTotalSignal();

        //Randomly permute order
        std::random_shuffle(std::begin(tcells), std::end(tcells));

        bool dead = false;

        for(unsigned int i=0; i < starting_num; i++){
            current_tcell = tcells[i];

            //If not true, then the release/absorb will be handled in the DCELL section
            if(diffuse_signal){
                releasePSignal(current_tcell);
                absorbPSignal(current_tcell);
            }

            if(moveTcell(current_tcell,i)){
                i--;
                starting_num--;
                continue;
            }

            splitCell(current_tcell);

            bool dies = cellDeath(current_tcell);
            if(dies){
                dead = removeTCell(i);
                if(dead){
                    i--;
                    starting_num--;
                }
            }
         }

        if(diffuse_signal){
            //Decay the signal
            grid.decayPSignal();
            //Diffuse signal
            for(unsigned int i=0;i < diffusion_cycles; i++){
                grid.diffusePSignal();
            }
        }

        //Add the current state to the vectors for reading and writing
        time_steps.push_back(time_elapsed);
        population.push_back(num_tcells);
        h_population.push_back(num_hcells);
        k_population.push_back(num_kcells);

        //TCELL END

        time_elapsed++;
        return true;
    }

    //Getter methods

    Tcell * getTCell(unsigned int position){
        return tcells[position];
    }

    unsigned int getNumTCells(){
        return num_tcells;
    }

    map<string,double> getParameters(){
        map<string,double> param_dict;
        param_dict.emplace("StartingHCells",starting_nums[0]);
        param_dict.emplace("StartingKCells",starting_nums[1]);
        param_dict.emplace("StartingDCells",starting_nums[2]);
        param_dict.emplace("NumTCells",num_tcells);
        param_dict.emplace("NumHCells",num_hcells);
        param_dict.emplace("NumDCells",num_dcells);
        param_dict.emplace("DiffusionCycles",diffusion_cycles);
        param_dict.emplace("NumDead",num_dead);
        param_dict.emplace("TimeElapsed",time_elapsed);
        param_dict.emplace("DiffuseSignal",diffuse_signal);
        param_dict.emplace("NoFluxBoundary",no_flux);
        return param_dict;
    }

    //File Handling

    //Write current game parameters to the FileIO object
    void saveGame(){
        //Build all the different components and then write to the file.
        ostringstream full_data;
        full_data << buildFileData(TimeSteps);
        full_data << buildFileData(Population);
        full_data << buildFileData(HelperPopulation);
        full_data << buildFileData(KillerPopulation);
        full_data << buildFileData(Parameters);
        file_io.writeFile(full_data.str());
    }

    //Build the QJsonObject and pass in current parameters
    string buildFileData(int data_type){
        vector<unsigned int> data_vec;
        ostringstream data_stream;
        switch(data_type){
        case TimeSteps:
            data_vec = time_steps;
            break;
        case Population:
            data_vec = population;
            break;
        case HelperPopulation:
            data_vec = h_population;
            break;
        case KillerPopulation:
            data_vec = k_population;
            break;
        case Parameters:
            data_stream << copyDictData("Dendritic cell Parameters",D);
            data_stream << copyDictData("Killer cell Parameters",K);
            data_stream << copyDictData("Helper cell Parameters",H);
            data_stream << copyDictData("Grid Parameters",G);
            data_stream << copyDictData("Modelling Parameters",A);
        }
        if (!data_vec.empty() && data_type != Parameters)
          {
            // Convert all but the last element to avoid a trailing "\t"
            copy(data_vec.begin(), data_vec.end()-1,
                std::ostream_iterator<int>(data_stream, "\t"));

            // Now add the last element with no delimiter
            data_stream << data_vec.back() << endl;
          }
        return data_stream.str();
    }

    string copyDictData(string heading, unsigned int dict_type){
        ostringstream data_stream;
        map<string,double> param_data;
        data_stream << heading << endl;
        //There is probably a better way to do this...
        switch(dict_type){
        case D:
            param_data = Dcell(new Coordinate(0,0),0,0).getParameters();
            break;
        case H:
            param_data = HelperCell(new Coordinate(0,0),0,0).getParameters();
            break;
        case K:
            param_data = KillerCell(new Coordinate(0,0),0,0).getParameters();
            break;
        case G:
            param_data = grid.getParameters();
            break;
        case A:
            param_data = getParameters();
            break;
        }
        map<string,double>::iterator it;
        //Copy the names of the parameters
        for(it = param_data.begin(); it != param_data.end(); it++){
            data_stream << it->first << "\t";
        }
        data_stream << endl;
        for(it = param_data.begin(); it != param_data.end(); it++){
            data_stream << it->second << "\t";
        }
        data_stream << endl;
        return data_stream.str();
    }


private:
    //Number of rows and columns in the grid
    unsigned int m_rows;
    vector<unsigned int> starting_nums;
    unsigned int num_tcells;
    unsigned int num_hcells;
    unsigned int num_kcells;
    unsigned int num_dcells;
    unsigned int num_dead;
    unsigned int diffusion_cycles;
    vector<Tcell *> tcells;
    vector<Dcell *> dcells;
    //pointer that keeps track of the current cell
    Tcell * current_tcell;
    Dcell * current_dcell;
    Grid grid;
    //Number of time steps
    unsigned int time_elapsed;
    unsigned int frame_height;
    unsigned int frame_scale;
    //File handler
    FileIO file_io;
    //Data to be written to the file.
    vector<unsigned int> time_steps;
    vector<unsigned int> population;
    vector<unsigned int> h_population;
    vector<unsigned int> k_population;
    //Whether to use a diffusion model or a local interaction model.
    bool diffuse_signal;
    //Whether to use Dirichlet or Neumann BC's
    bool no_flux;
};

#endif // ALLCELLS_H
