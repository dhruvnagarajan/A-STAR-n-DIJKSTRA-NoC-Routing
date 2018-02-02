
/*
 * fortytwo_router.cpp
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Lavina Jain
 *
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file XY_router.cpp
/// \brief Implements XY routing algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "fortytwo_router.h"
#include "../../config/extern.h"
#include <float.h>
#include <vector>
#include <stack>
#include <set>

using namespace std;

vector <char> PATH;
ULL CURR = 0;

struct Point {

    int prev_i, prev_j;
    double f, g, h;
};

const int SIZE = 9;
int grid[SIZE][SIZE];
double h1[SIZE][SIZE], h2[SIZE][SIZE];
vector <char> path1, path2;

#define ROW 9
#define COL 9
#define SIZE 9


typedef pair<int, int> Pair;
typedef pair<double, pair<int, int> > pPair;
 
struct cell {
    
    int parent_i, parent_j;
    double f, g, h;
};

bool isValid(int row, int col) {

    return (row >= 0) && (row < ROW) &&
           (col >= 0) && (col < COL);
}

bool isUnBlocked(int grid[][COL], int row, int col) {
    
    if (grid[row][col] == 1)
        return (true);
    else
        return (false);
}

bool isDestination(int row, int col, Pair dest) {
    
    if (row == dest.first && col == dest.second)
        return (true);
    else
        return (false);
}

void calc_h1 (int, int, int, int);
void calc_h2 (int, int, int, int);
void trace_path (Point [][SIZE], pair <int, int>);
void calc_astar1 (int [][SIZE], pair <int, int>, pair <int, int>);
void calc_astar2 (int [][SIZE], pair <int, int>, pair <int, int>);
vector <char> genetic_crossover ();
vector <char> genetic_crossover_nodeGap (vector <char>);
void grid_init ();
bool grid_isAlive (int, int);

////////////////////////////////////////////////
// Method to set unique id
////////////////////////////////////////////////
void fortytwo_router::setID(UI id_tile)
{
    id = id_tile;
    initialize();
}

////////////////////////////////////////////////
/// Method that implements XY routing
/// inherited from base class router
/// Parameters:
/// - input direction from which flit entered the tile
/// - tileID of source tile
/// - tileID of destination tile
/// returns next hop direction
////////////////////////////////////////////////
UI fortytwo_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id) {

    ULL sx = id / num_cols,
        sy = id % num_cols,
        dx = dest_id / num_cols,
        dy = dest_id % num_cols;
//sx = 5, sy = 8;
//dx = 4, dy = 3;

	cout << "::\tip_dir:" << ip_dir << "  source_id:" << source_id << "  dest_id:" << dest_id;
	cout << "\tsx,sy:" << sx << "," << sy << "    dx,dy:" << dx << "," << dy << endl;

	//if (sx < 1 && sy < 1) return S;
	//else return C;
if (sx == dx && sy == dy) { cout << "RETURNING C\n";return C;}

    grid_init ();
    calc_h1 (sx, sy, dx, dy);
    calc_h2 (sx, sy, dx, dy);
    calc_astar1 (grid, make_pair (sx, sy), make_pair (dx, dy));
    calc_astar2 (grid, make_pair (sx, sy), make_pair (dx, dy));

    genetic_crossover ();

    if (PATH[0] == 'U') {
	cout << "RETURNING N\n";
        return N;
    } else if (PATH[0] == 'D') {
	cout << "RETURNING S\n";
        return S;
    } else if (PATH[0] == 'E') {
	cout << "RETURNING E\n";
        return E;
    } else if (PATH[0] == 'W') {
	cout << "RETURNING W\n";
        return W;
    }

    return 0;
}

void calc_h1 (int sx, int sy, int dx, int dy) {

    for (int i = sx; i < SIZE; i ++) {

        for (int j = sy; j < SIZE; j ++) {
            
            h1[i][j] = abs (i - dx) + abs (j - dy);
        }
    }
}

void calc_h2 (int sx, int sy, int dx, int dy) {

    for (int i = sx; i < SIZE; i ++) {

        for (int j = sy; j < SIZE; j ++) {
            
            h1[i][j] = sqrt (pow (abs (i - dx), 2) + pow (abs (j - dy), 2));
        }
    }
}

void tracePath1 (cell cellDetails[][COL], Pair dest) {
    
    int row = dest.first;
    int col = dest.second;
 
    stack<Pair> Path;
 
    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col )) {
        
        Path.push (make_pair (row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
 
    pair <int, int> old;
    old = make_pair (0, 0);

    path1.clear ();

    Path.push (make_pair (row, col));

    while (!Path.empty()) {
        
        pair<int,int> p = Path.top();
        Path.pop();

        if (p.first == old.first - 1 && p.second == old.second) {

            path1.push_back ('U');
        } else if (p.first == old.first + 1 && p.second == old.second) {

            path1.push_back ('D');
        } else if (p.first == old.first&& p.second == old.second - 1) {

            path1.push_back ('W');
        } else if (p.first == old.first&& p.second == old.second + 1) {

            path1.push_back ('E');
        } else if (p.first == old.first - 1 && p.second == old.second - 1) {

            path1.push_back ('U');
            path1.push_back ('W');
        } else if (p.first == old.first - 1 && p.second == old.second + 1) {

            path1.push_back ('U');
            path1.push_back ('E');
        } else if (p.first == old.first + 1 && p.second == old.second - 1) {

            path1.push_back ('D');
            path1.push_back ('W');
        } else if (p.first == old.first + 1 && p.second == old.second + 1) {

            path1.push_back ('D');
            path1.push_back ('E');
        } else if (p.first == old.first && p.second == old.second) {

            //
        }
        else {

            cout << ":: TracePath1: Undefied behaviour!\n";
		cout<<p.first<<" "<<p.second<<"\n";
		cout<<old.first<<" "<<old.second<<"\n";
        }

        old.first = p.first, old.second = p.second;
    }
 
    return;
}

void tracePath2 (cell cellDetails[][COL], Pair dest) {
    
    int row = dest.first;
    int col = dest.second;
 
    stack<Pair> Path;
 
    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col )) {
        
        Path.push (make_pair (row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
 
    pair <int, int> old;
    old = make_pair (0, 0);

    path2.clear ();

    Path.push (make_pair (row, col));

    while (!Path.empty()) {
        
        pair<int,int> p = Path.top();
        Path.pop();

        if (p.first == old.first - 1 && p.second == old.second) {

            path2.push_back ('U');
        } else if (p.first == old.first + 1 && p.second == old.second) {

            path2.push_back ('D');
        } else if (p.first == old.first&& p.second == old.second - 1) {

            path2.push_back ('W');
        } else if (p.first == old.first&& p.second == old.second + 1) {

            path2.push_back ('E');
        } else if (p.first == old.first - 1 && p.second == old.second - 1) {

            path2.push_back ('U');
            path2.push_back ('W');
        } else if (p.first == old.first - 1 && p.second == old.second + 1) {

            path2.push_back ('U');
            path2.push_back ('E');
        } else if (p.first == old.first + 1 && p.second == old.second - 1) {

            path2.push_back ('D');
            path2.push_back ('W');
        } else if (p.first == old.first + 1 && p.second == old.second + 1) {

            path2.push_back ('D');
            path2.push_back ('E');
        } else if (p.first == old.first && p.second == old.second) {

            //
        }
        else {

            cout << ":: TracePath2: Undefied behaviour!\n";
		cout<<p.first<<" "<<p.second<<"\n";
		cout<<old.first<<" "<<old.second<<"\n";
        }

        old.first = p.first, old.second = p.second;
    }
 
    return;
}

void calc_astar1 (int grid[][COL], Pair src, Pair dest) {
    
    /*if (isValid (src.first, src.second) == false)
    {
        printf ("Source is invalid\n");
        return;
    }
    
    if (isValid (dest.first, dest.second) == false)
    {
        printf ("Destination is invalid\n");
        return;
    }
    
    if (isUnBlocked(grid, src.first, src.second) == false ||
            isUnBlocked(grid, dest.first, dest.second) == false)
    {
        printf ("Source or the destination is blocked\n");
        return;
    }
    
    if (isDestination(src.first, src.second, dest) == true)
    {
        printf ("We are already at the destination\n");
        return;
    }*/
    
    bool closedList[ROW][COL];
    memset(closedList, false, sizeof (closedList));
    
    cell cellDetails[ROW][COL];
 
    int i, j;
 
    for (i=0; i<ROW; i++)
    {
        for (j=0; j<COL; j++)
        {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }
    
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;
    
    set<pPair> openList;
    
    openList.insert(make_pair (0.0, make_pair (i, j)));
    
    bool foundDest = false;
 
    while (!openList.empty())
    {
        pPair p = *openList.begin();
        
        openList.erase(openList.begin());
        
        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

	//cout << "A*1 source:" << i << "," << j << "\tdest:" << dest.first << "," << dest.second << endl;
        
        double gNew, hNew, fNew;
        
        if (isValid(i-1, j) == true)
        {
            
            if (isDestination(i-1, j, dest) == true)
            {
                
                cellDetails[i-1][j].parent_i = i;
                cellDetails[i-1][j].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j] == false &&
                     isUnBlocked(grid, i-1, j) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h1[i - 1][j];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j].f == FLT_MAX ||
                        cellDetails[i-1][j].f > fNew)
                {
                    openList.insert( make_pair(fNew, make_pair(i-1, j)));
                    cellDetails[i-1][j].f = fNew;
                    cellDetails[i-1][j].g = gNew;
                    cellDetails[i-1][j].h = hNew;
                    cellDetails[i-1][j].parent_i = i;
                    cellDetails[i-1][j].parent_j = j;
                }
            }
        }
        
        if (isValid(i+1, j) == true)
        {
            
            if (isDestination(i+1, j, dest) == true)
            {
                
                cellDetails[i+1][j].parent_i = i;
                cellDetails[i+1][j].parent_j = j;
                printf("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j] == false &&
                     isUnBlocked(grid, i+1, j) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h1[i+1][j];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j].f == FLT_MAX ||
                        cellDetails[i+1][j].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i+1, j)));
                    
                    cellDetails[i+1][j].f = fNew;
                    cellDetails[i+1][j].g = gNew;
                    cellDetails[i+1][j].h = hNew;
                    cellDetails[i+1][j].parent_i = i;
                    cellDetails[i+1][j].parent_j = j;
                }
            }
        }
        
        if (isValid (i, j+1) == true)
        {
            
            if (isDestination(i, j+1, dest) == true)
            {
                
                cellDetails[i][j+1].parent_i = i;
                cellDetails[i][j+1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i][j+1] == false &&
                     isUnBlocked (grid, i, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h1[i][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i][j+1].f == FLT_MAX ||
                        cellDetails[i][j+1].f > fNew)
                {
                    openList.insert( make_pair(fNew, make_pair (i, j+1)));

                    cellDetails[i][j+1].f = fNew;
                    cellDetails[i][j+1].g = gNew;
                    cellDetails[i][j+1].h = hNew;
                    cellDetails[i][j+1].parent_i = i;
                    cellDetails[i][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid(i, j-1) == true)
        {
            
            if (isDestination(i, j-1, dest) == true)
            {
                
                cellDetails[i][j-1].parent_i = i;
                cellDetails[i][j-1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i][j-1] == false &&
                     isUnBlocked(grid, i, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h1[i][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i][j-1].f == FLT_MAX ||
                        cellDetails[i][j-1].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i, j-1)));

                    cellDetails[i][j-1].f = fNew;
                    cellDetails[i][j-1].g = gNew;
                    cellDetails[i][j-1].h = hNew;
                    cellDetails[i][j-1].parent_i = i;
                    cellDetails[i][j-1].parent_j = j;
                }
            }
        }
        
        if (isValid(i-1, j+1) == true)
        {
            
            if (isDestination(i-1, j+1, dest) == true)
            {
                
                cellDetails[i-1][j+1].parent_i = i;
                cellDetails[i-1][j+1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j+1] == false &&
                     isUnBlocked(grid, i-1, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h1[i - 1][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j+1].f == FLT_MAX ||
                        cellDetails[i-1][j+1].f > fNew)
                {
                    openList.insert( make_pair (fNew,  make_pair(i-1, j+1)));

                    cellDetails[i-1][j+1].f = fNew;
                    cellDetails[i-1][j+1].g = gNew;
                    cellDetails[i-1][j+1].h = hNew;
                    cellDetails[i-1][j+1].parent_i = i;
                    cellDetails[i-1][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid (i-1, j-1) == true)
        {
            
            if (isDestination (i-1, j-1, dest) == true)
            {
                
                cellDetails[i-1][j-1].parent_i = i;
                cellDetails[i-1][j-1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j-1] == false && isUnBlocked(grid, i-1, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h1[i - 1][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j-1].f == FLT_MAX ||
                        cellDetails[i-1][j-1].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i-1, j-1)));
                    
                    cellDetails[i-1][j-1].f = fNew;
                    cellDetails[i-1][j-1].g = gNew;
                    cellDetails[i-1][j-1].h = hNew;
                    cellDetails[i-1][j-1].parent_i = i;
                    cellDetails[i-1][j-1].parent_j = j;
                }
            }
        }
        
        if (isValid(i+1, j+1) == true)
        {
            
            if (isDestination(i+1, j+1, dest) == true)
            {
                
                cellDetails[i+1][j+1].parent_i = i;
                cellDetails[i+1][j+1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j+1] == false &&
                     isUnBlocked(grid, i+1, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h1[i + 1][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j+1].f == FLT_MAX ||
                        cellDetails[i+1][j+1].f > fNew)
                {
                    openList.insert(make_pair(fNew,  make_pair (i+1, j+1)));
                                        
                    cellDetails[i+1][j+1].f = fNew;
                    cellDetails[i+1][j+1].g = gNew;
                    cellDetails[i+1][j+1].h = hNew;
                    cellDetails[i+1][j+1].parent_i = i;
                    cellDetails[i+1][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid (i+1, j-1) == true)
        {
            
            if (isDestination(i+1, j-1, dest) == true)
            {
                
                cellDetails[i+1][j-1].parent_i = i;
                cellDetails[i+1][j-1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath1 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j-1] == false &&
                     isUnBlocked(grid, i+1, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h1[i + 1][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j-1].f == FLT_MAX ||
                        cellDetails[i+1][j-1].f > fNew)
                {
                    openList.insert(make_pair(fNew,  make_pair(i+1, j-1)));

                    cellDetails[i+1][j-1].f = fNew;
                    cellDetails[i+1][j-1].g = gNew;
                    cellDetails[i+1][j-1].h = hNew;
                    cellDetails[i+1][j-1].parent_i = i;
                    cellDetails[i+1][j-1].parent_j = j;
                }
            }
        }
    }

    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

    return;
}

void calc_astar2 (int grid[][COL], Pair src, Pair dest)
{
    
    /*if (isValid (src.first, src.second) == false)
    {
        printf ("Source is invalid\n");
        return;
    }
    
    if (isValid (dest.first, dest.second) == false)
    {
        printf ("Destination is invalid\n");
        return;
    }
    
    if (isUnBlocked(grid, src.first, src.second) == false ||
            isUnBlocked(grid, dest.first, dest.second) == false)
    {
        printf ("Source or the destination is blocked\n");
        return;
    }
    
    if (isDestination(src.first, src.second, dest) == true)
    {
        printf ("We are already at the destination\n");
        return;
    }*/
    
    bool closedList[ROW][COL];
    memset(closedList, false, sizeof (closedList));
    
    cell cellDetails[ROW][COL];
 
    int i, j;
 
    for (i=0; i<ROW; i++)
    {
        for (j=0; j<COL; j++)
        {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }
    
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;
    
    set<pPair> openList;
    
    openList.insert(make_pair (0.0, make_pair (i, j)));
    
    bool foundDest = false;
 
    while (!openList.empty())
    {
        pPair p = *openList.begin();
        
        openList.erase(openList.begin());
        
        i = p.second.first;
        j = p.second.second;
	//cout << "A*2 source:" << i << "," << j << "\tdest:" << dest.first << "," << dest.second << endl;

        closedList[i][j] = true;
        
        double gNew, hNew, fNew;
        
        if (isValid(i-1, j) == true)
        {
            
            if (isDestination(i-1, j, dest) == true)
            {
                
                cellDetails[i-1][j].parent_i = i;
                cellDetails[i-1][j].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j] == false &&
                     isUnBlocked(grid, i-1, j) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h2[i - 1][j];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j].f == FLT_MAX ||
                        cellDetails[i-1][j].f > fNew)
                {
                    openList.insert( make_pair(fNew, make_pair(i-1, j)));
                                               
                    cellDetails[i-1][j].f = fNew;
                    cellDetails[i-1][j].g = gNew;
                    cellDetails[i-1][j].h = hNew;
                    cellDetails[i-1][j].parent_i = i;
                    cellDetails[i-1][j].parent_j = j;
                }
            }
        }
        
        if (isValid(i+1, j) == true)
        {
            
            if (isDestination(i+1, j, dest) == true)
            {
                
                cellDetails[i+1][j].parent_i = i;
                cellDetails[i+1][j].parent_j = j;
                printf("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j] == false &&
                     isUnBlocked(grid, i+1, j) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h2[i + 1][j];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j].f == FLT_MAX ||
                        cellDetails[i+1][j].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i+1, j)));
                    
                    cellDetails[i+1][j].f = fNew;
                    cellDetails[i+1][j].g = gNew;
                    cellDetails[i+1][j].h = hNew;
                    cellDetails[i+1][j].parent_i = i;
                    cellDetails[i+1][j].parent_j = j;
                }
            }
        }
        
        if (isValid (i, j+1) == true)
        {
            
            if (isDestination(i, j+1, dest) == true)
            {
                
                cellDetails[i][j+1].parent_i = i;
                cellDetails[i][j+1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i][j+1] == false &&
                     isUnBlocked (grid, i, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h2[i][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i][j+1].f == FLT_MAX ||
                        cellDetails[i][j+1].f > fNew)
                {
                    openList.insert( make_pair(fNew, make_pair (i, j+1)));
                                        
                    cellDetails[i][j+1].f = fNew;
                    cellDetails[i][j+1].g = gNew;
                    cellDetails[i][j+1].h = hNew;
                    cellDetails[i][j+1].parent_i = i;
                    cellDetails[i][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid(i, j-1) == true)
        {
            
            if (isDestination(i, j-1, dest) == true)
            {
                
                cellDetails[i][j-1].parent_i = i;
                cellDetails[i][j-1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i][j-1] == false &&
                     isUnBlocked(grid, i, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = h2[i][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i][j-1].f == FLT_MAX ||
                        cellDetails[i][j-1].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i, j-1)));
                                          
                    cellDetails[i][j-1].f = fNew;
                    cellDetails[i][j-1].g = gNew;
                    cellDetails[i][j-1].h = hNew;
                    cellDetails[i][j-1].parent_i = i;
                    cellDetails[i][j-1].parent_j = j;
                }
            }
        }
        
        if (isValid(i-1, j+1) == true)
        {
            
            if (isDestination(i-1, j+1, dest) == true)
            {
                
                cellDetails[i-1][j+1].parent_i = i;
                cellDetails[i-1][j+1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j+1] == false &&
                     isUnBlocked(grid, i-1, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h2[i - 1][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j+1].f == FLT_MAX ||
                        cellDetails[i-1][j+1].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair(i-1, j+1)));
                                    
                    cellDetails[i-1][j+1].f = fNew;
                    cellDetails[i-1][j+1].g = gNew;
                    cellDetails[i-1][j+1].h = hNew;
                    cellDetails[i-1][j+1].parent_i = i;
                    cellDetails[i-1][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid (i-1, j-1) == true)
        {
            
            if (isDestination (i-1, j-1, dest) == true)
            {
                
                cellDetails[i-1][j-1].parent_i = i;
                cellDetails[i-1][j-1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i-1][j-1] == false &&
                     isUnBlocked(grid, i-1, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h2[i - 1][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i-1][j-1].f == FLT_MAX ||
                        cellDetails[i-1][j-1].f > fNew)
                {
                    openList.insert( make_pair (fNew, make_pair (i-1, j-1)));
                    
                    cellDetails[i-1][j-1].f = fNew;
                    cellDetails[i-1][j-1].g = gNew;
                    cellDetails[i-1][j-1].h = hNew;
                    cellDetails[i-1][j-1].parent_i = i;
                    cellDetails[i-1][j-1].parent_j = j;
                }
            }
        }
        
        if (isValid(i+1, j+1) == true)
        {
            
            if (isDestination(i+1, j+1, dest) == true)
            {
                
                cellDetails[i+1][j+1].parent_i = i;
                cellDetails[i+1][j+1].parent_j = j;
                printf ("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j+1] == false &&
                     isUnBlocked(grid, i+1, j+1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h2[i + 1][j + 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j+1].f == FLT_MAX ||
                        cellDetails[i+1][j+1].f > fNew)
                {
                    openList.insert(make_pair(fNew, make_pair (i+1, j+1)));
                                        
                    cellDetails[i+1][j+1].f = fNew;
                    cellDetails[i+1][j+1].g = gNew;
                    cellDetails[i+1][j+1].h = hNew;
                    cellDetails[i+1][j+1].parent_i = i;
                    cellDetails[i+1][j+1].parent_j = j;
                }
            }
        }
        
        if (isValid (i+1, j-1) == true)
        {
            
            if (isDestination(i+1, j-1, dest) == true)
            {
                
                cellDetails[i+1][j-1].parent_i = i;
                cellDetails[i+1][j-1].parent_j = j;
                printf("The destination cell is found\n");
                tracePath2 (cellDetails, dest);
                foundDest = true;
                return;
            }
            
            else if (closedList[i+1][j-1] == false &&
                     isUnBlocked(grid, i+1, j-1) == true)
            {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = h2[i + 1][j - 1];
                fNew = gNew + hNew;
                
                if (cellDetails[i+1][j-1].f == FLT_MAX ||
                        cellDetails[i+1][j-1].f > fNew)
                {
                    openList.insert(make_pair(fNew, make_pair(i+1, j-1)));
                                        
                    cellDetails[i+1][j-1].f = fNew;
                    cellDetails[i+1][j-1].g = gNew;
                    cellDetails[i+1][j-1].h = hNew;
                    cellDetails[i+1][j-1].parent_i = i;
                    cellDetails[i+1][j-1].parent_j = j;
                }
            }
        }
    }
    
    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");
 
    return;
}

vector <char> genetic_crossover () {

    vector <pair<int, char> > c, d;

    int len1 = path1.size (), len2 = path2.size ();

	cout << ":: Cross Over:\n";
for (int i = 0; i < len1; i ++) cout << path1[i]; cout << endl;
for (int i = 0; i < len2; i ++) cout << path2[i]; cout << endl;

    for (int i = 0; i < len1 && i < len2; i ++) {

        if (path1[i] == path2[i]) c.push_back (make_pair (i, path1[1]));
        else d.push_back (make_pair (i, path2[2]));
    }

    vector <char> path3;

    for (int i = 0; i < c.size (); i ++) {

        path3.push_back (c[i].second);
    }

    for (int i = 0; i < d.size (); i ++) {

        path3.push_back (d[i].second);
    }

    return genetic_crossover_nodeGap (path3);
}

vector <char> genetic_crossover_nodeGap (vector <char> path) {

    // use a * again to cover the node gap

    PATH = path;

    return path;
}

void grid_init () {
    
    for (int i = 0; i < SIZE; i ++) {

        for (int j = 0; j < SIZE; j ++) {

            grid[i][j] = true;
            h1[i][j] = 0;
            h2[i][j] = 0;
        }
    }
}

bool grid_isAlive (int i, int j) {

    return grid[i][j];
}

////////////////////////////////////////////////
/// Method containing any initializations
/// inherited from base class router
////////////////////////////////////////////////
// may be empty
// definition must be included even if empty, because this is a virtual function in base class
void fortytwo_router::initialize()
{

}

// for dynamic linking
extern "C"
{
    router *maker()
    {
        return new fortytwo_router;
    }
}
