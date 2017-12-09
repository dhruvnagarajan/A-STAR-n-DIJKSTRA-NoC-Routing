#include <bits/stdc++.h>

using namespace std;

const int SIZE = 4;

// struct used to easily break the code. Hard to do, with class members

struct Point {

    int x;
    int y;
};

struct Flit {

    int payload;
    struct Point source, destination;
};

bool deliver (struct Flit, int [][SIZE], string, int);
struct Point deliver_direction (struct Flit, string, int);
bool deliver_check_path_fail (string, int [][SIZE]);

string genetic_new_path (struct Flit, int [][SIZE]);
void genetic_new_path_dfs (struct Flit, bool [][SIZE], int [][SIZE], string, vector <string>, char);
int genetic_new_path_fitness (string, string);
string genetic_new_path_fitness_dfs (struct Flit, int [][SIZE]);
vector <string> genetic_new_path_crossover (string, string);

void grid_init (int [][SIZE]);
void grid_node_failure (int [][SIZE]);

int main () {

    cout << ":: Main ()\n";

    // grid setup
    int grid[SIZE][SIZE];

    grid_init (grid);
    
    int t = 10; // number of test cases for flit delivery

    while (t --) {

        // simulating 'flit' generation by the network
        // for every 'flit' that has to be delivered from source 's' to destination 'd'...
        struct Flit flit;

        // todo: set flit source and destination, and payload using rand ()
        flit.source.x = flit.source.y = flit.destination.x = flit.destination.y = 0;

        printf ("Delivering new random flit\n");

        string path = genetic_new_path (flit, grid);
        
        int hasBeenDelivered = deliver (flit, grid, path, 1); // path[0] contains 'S' as in 'start': for debugging purposes

        if (hasBeenDelivered) {

            printf ("Flit delivered!\n");
        } else {

            printf ("Flit delivery failed!\n");
        }
    }

    return 0;
}

void genetic_new_path_dfs (struct Flit flit, bool visited[][SIZE], int grid[][SIZE], string curr_path, vector <string> paths, char direction) {
    
    cout << ":: DFS ()\n";

    curr_path += direction;

    // store all paths that end at flit.destination
    if (flit.source.x == flit.destination.x && flit.source.y == flit.destination.y) {
    
        cout << ":: DFS path found:\t" << curr_path << endl;
        paths.push_back (curr_path);
    }

    // mark current node as visited
    visited[flit.source.x][flit.source.y] = true;

    for (int i = flit.source.x - 1; i <= flit.source.x + 1; i ++) {

        for (int j = flit.source.y - 1; j <= flit.source.y + 1; j ++) {

            // currently loop running for all 8 directions

            // check for only 4 directions, and discard diagonal advancements
            if (i >= 0 && j >= 0 && i < SIZE && j < SIZE) { // DFS movement to be strictly inside the grid

                // copy flit so as to not alter the original flit, for next DFS iteration
                struct Flit flit_copy = flit;
                // next DFS iteration would start from this grid position, for next node in grid
                flit_copy.source.x = i, flit_copy.source.y = j;

                if (i == flit.source.x - 1 && j == flit.source.y) {
                    
                    // if left
                    genetic_new_path_dfs (flit_copy, visited, grid, curr_path, paths, 'L');
                } else if (i == flit.source.x + 1 && j == flit.source.y) {
                    
                    // if right
                    genetic_new_path_dfs (flit_copy, visited, grid, curr_path, paths, 'R');
                } else if (i == flit.source.x && j == flit.source.y - 1) {
                    
                    // if up
                    genetic_new_path_dfs (flit_copy, visited, grid, curr_path, paths, 'U');
                } else if (i == flit.source.x && j == flit.source.y + 1) {
                    
                    // if down
                    genetic_new_path_dfs (flit_copy, visited, grid, curr_path, paths, 'D');
                }
            }
        }
    }

    // erase all paths for new set of paths in recurrsion
    curr_path = "";

    // marks curr node unvisited, for some other DFS path to go through this node
    visited[flit.source.x][flit.source.y] = false;
}

vector <string> genetic_new_path_crossover (string one, string two) {

    cout << ":: Crossover ()\n";

    // todo: upgrade

    vector <string> paths;

    // perform custom crossover
    paths.push_back (one);
    paths.push_back (two);

    return paths;
}

int genetic_new_path_fitness (string path, string fit_path) {

    cout << ":: Fitness ()\n";

    int diff = 0;

    int path_len = path.length (), fit_path_len = fit_path.length (); // todo: remove 2? path includes 'S' and 'E'

    diff += fit_path_len - path_len; // assuming fit_path is the shortest path

    for (int i = 0; i < path_len - 1 && i < fit_path_len; i ++) { // todo: remove i < path_len -1? '-1' for 'E' in path

        if (path[i + 1] != fit_path[i]) diff ++;
    }

    return diff;
}

string genetic_new_path_best (struct Flit flit, int grid[][SIZE]) {

    cout << ":: Fitness () - BestPath ()\n";

    string fit_path = "";

    return fit_path;
}

string genetic_new_path (struct Flit flit, int grid[][SIZE]) {

    cout << ":: New Path ()\n";

    // calculate best path using fitness_dfs ()

    string fit_path = genetic_new_path_best (flit, grid);

    // start a new DFS from current flit node
    bool visited[SIZE][SIZE] = {{ false }};
    
    vector <string> paths;
    string curr_path = "";

    // todo: pass paths matrix as reference
    genetic_new_path_dfs (flit, visited, grid, curr_path, paths, 'S'); // todo: receive paths matrix from dfs

    vector < pair <int, string> > fit_paths;

    // find top 2 best off springs

    for (int i = 0; i < paths.size (); i ++) {

        fit_paths.push_back (make_pair (genetic_new_path_fitness (paths[i], fit_path), paths[i]));
    }

    // most fit paths are towards the end of the list
    sort (fit_paths.begin (), fit_paths .end ());

    int number_of_offsprings = fit_paths.size ();

    // currently 'offsprings' will contain only one best path
    vector <string> offsprings = genetic_new_path_crossover (fit_paths[number_of_offsprings - 1].second, fit_paths[number_of_offsprings - 2].second);

    return offsprings[0];
}

struct Point deliver_direction (struct Flit flit, string path, int curr_node_in_path) {

    cout << ":: GetDirection ()\n";

    // calculates flit movement direction at every node in transmission
    // U: up, D: down, L: left, R: right

    char direction = path[curr_node_in_path];

    struct Point point;
    point.x = flit.source.x, point.y = flit.source.y;

    if (direction == 'U') {

        point.y += 1;
    } else if (direction == 'D') {

        point.y -= 1;
    } else if (direction == 'L') {

        point.x -= 1;
    } else if (direction == 'R') {

        point.x += 1;
    }

    return point;
}

bool deliver_check_path_fail (struct Flit flit, string path, int grid[][SIZE]) {

    cout << ":: Check node failure ()\n";

    for (int i = 0; i < path.size (); i ++) {

        int curr_x = flit.source.x, curr_y = flit.source.y;

        // check if node at next hop is okay. Return false if it is not
        bool doesNodeFailFallInOurPath =    (path[i] == 'U' && grid[curr_x + 1][curr_y]) ||

    // todo: best parents?
    // todo: cross over?
                                            (path[i] == 'D' && grid[curr_x - 1][curr_y]) ||
                                            (path[i] == 'L' && grid[curr_x][curr_y - 1]) ||
                                            (path[i] == 'R' && grid[curr_x][curr_y + 1]);
        if (doesNodeFailFallInOurPath) return false;
    }

    return true;
}

bool deliver (struct Flit flit, int grid[][SIZE], string path, int curr_node_in_path) {

    cout << ":: Deliver ()\n";

    if (flit.source.x == flit.destination.x && flit.source.y == flit.destination.y) {

        return true; // delivered
    } else if (0/*todo: no more paths*/) {

        return false; // delivery failed
    }

    // next hop is determined by deliver_direction ()
    struct Point direction = deliver_direction (flit, path, curr_node_in_path); // 4 directions

    grid_node_failure (grid); // randomly switches off nodes in the grid, one at a time

    // check if next hop is okay or failed
    if (deliver_check_path_fail (flit, path, grid)) {

        // check if any node has failed in our original path
        // call genetic_new_path () ?

        path = genetic_new_path (flit, grid);

        // check if (i, j) == 0
    } else {

        // otherwise: continue original path
        
        flit.source.x = direction.x, flit.source.y = direction.y; // flit has virtually been moved to new node
        
        deliver (flit, grid, path, curr_node_in_path + 1); // do the same thing for next hop
    }

    // update filt.source to curr grid(i, j)
}

void grid_node_failure (int grid[][SIZE]) {

    cout << ":: CreateNodeFailure()\n";

    // failed nodes do not become functional later

    int point_x = rand (), point_y = rand ();

    // todo: comporess bw 0 - SIZE. Convert into 'int'

    grid[point_x][point_y] = 0; // 1: node functional, 0: node failed
}

void grid_init (int grid[][SIZE]) {

    cout << ":: GridInit ()\n";

    for (int i = 0; i < SIZE; i ++) {

        for (int j = 0; j < SIZE; j ++) {

            grid[i][j] = 1; // 1: node functional, 0: node failed (controlled by grid_node_failure ())
        }
    }
}