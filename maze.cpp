#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
using namespace std;


// Maze Class - Handles maze grid, carving, and path checking
// ----------------------------------------------------------
class Maze {
private:
    int width;                      // Maze width
    int height;                     // Maze height
    vector<vector<char>> grid;      // 2D grid representing the maze

public:
    char path_symbol = ' ';         // Character used for open path

    // Constructor: initialize maze filled with walls ('#')
    Maze(int w, int h) : width(w), height(h) {
        grid.resize(height, vector<char>(width, '#'));
    }

    // Display the maze grid in console
    void display() const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cout << grid[y][x];
            }
            cout << endl;
        }
    }

    // Safely set a cell value in the grid
    void setCell(int x, int y, char value) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = value;
    }

    // Carve a path two cells away in the chosen direction
    void Carve(int x, int y, int direction) {
        switch(direction) {
            case 1: // Up
                setCell(x, y - 1, path_symbol);
                setCell(x, y - 2, path_symbol);
                break;
            case 2: // Right
                setCell(x + 1, y, path_symbol);
                setCell(x + 2, y, path_symbol);  
                break;
            case 3: // Down
                setCell(x, y + 1, path_symbol);
                setCell(x, y + 2, path_symbol);  
                break;
            case 4: // Left
                setCell(x - 1, y, path_symbol);
                setCell(x - 2, y, path_symbol);  
                break;
        }
    }

    // Check which directions are available to carve from current position
    vector<int> Availability(int x, int y) {
        vector<int> avaliable_directions = {};

        // Up
        if (y >= 2 && grid[y - 2][x] == '#')
            avaliable_directions.push_back(1);
        // Right
        if (x <= width - 3 && grid[y][x + 2] == '#')
            avaliable_directions.push_back(2);
        // Down
        if (y <= height - 3 && grid[y + 2][x] == '#')
            avaliable_directions.push_back(3);
        // Left
        if (x >= 2 && grid[y][x - 2] == '#')
            avaliable_directions.push_back(4);
        
        return avaliable_directions;
    }
};


// Main Function
// -------------
int main()
{
    // Counters and control variables
    int count = 0;
    int final_count = 0;

    // Maze dimensions
    int width = 71;   
    int height = 41; 

    // User input and control parameters
    char answer;
    int carve_count = 150;      // Minimum number of carvings before maze qualifies
    int attempts = 0;           // Number of maze generation attempts

    // Seed random number generator
    srand(time(NULL)); 

    // Initial carving coordinates (start position)
    int initial_x = 1;
    int initial_y = 1;
    
    // Maze Generation Loop (up to 500 attempts)
    // -----------------------------------------
    while (final_count < carve_count && attempts < 500)
    {
        // Storage for positions
        vector<vector<int>> saved_coords;
        vector<vector<int>> solved_coords;

        // Create new maze instance
        Maze maze(width, height);
        int count = 0;

        // Mark start point
        maze.setCell(initial_x, initial_y, '$');

        // Current coordinates
        int x_coord = initial_x;
        int y_coord = initial_y;

        // Store starting coordinate
        saved_coords.push_back({x_coord, y_coord});
        solved_coords.push_back({x_coord, y_coord});

        // Destination coordinates
        int x_final;
        int y_final;
        
        // State flags
        bool inside = 0;
        bool end = 0;
        bool turn_off = 0;

        // Maze Carving Loop
        // ----------------
        while (turn_off == 0) {
            // Get possible directions from current cell
            vector<int> avaliable_directions = maze.Availability(x_coord, y_coord);
            size_t length = avaliable_directions.size();
            
            // No available paths -> backtrack
            if (length == 0) {
                if (!end) {
                    // Record the endpoint (dead end)
                    x_final = x_coord;
                    y_final = y_coord;
                    end = 1;
                    final_count = count;
                }

                // Backtrack to last saved position
                saved_coords.pop_back();
                x_coord = saved_coords.back()[0];
                y_coord = saved_coords.back()[1];
            }

            // Choose random direction to carve
            else {
                int number = rand() % length;
                int direction = avaliable_directions[number];

                // Save current position
                saved_coords.push_back({x_coord, y_coord});

                // If not finished, record this position for solution path
                if (!end) {
                    solved_coords.push_back({x_coord, y_coord});
                }

                // Carve maze two cells in chosen direction
                maze.Carve(x_coord, y_coord, direction);

                // Move to new position
                switch(direction) {
                    case 1: y_coord -= 2; break; // Up
                    case 2: x_coord += 2; break; // Right
                    case 3: y_coord += 2; break; // Down
                    case 4: x_coord -= 2; break; // Left
                }
            }

            count++;

            // Stop if no more saved coordinates (fully backtracked)
            if (saved_coords.size() == 1)
                turn_off = 1;
        }

        // Finalize Maze and Ask to Show Solution
        // -------------------------------------
        maze.setCell(x_final, y_final, '!');

        // Once a maze meets minimum carve count, show it
        if (final_count >= carve_count)
        {
            system("CLS"); // Clear console
            maze.display();

            // Ask user if they want the solution displayed
            cout << "Do you want the solution (y or n): ";
            cin >> answer;

            if (tolower(answer) == 'y')
            {
                // Overlay solution path with dots
                for (int i = 0; i < solved_coords.size(); i++)
                {
                    int x = solved_coords[i][0];
                    int y = solved_coords[i][1];
                    maze.setCell(x, y, '.');
                }

                // Restore start and end symbols
                maze.setCell(initial_x, initial_y, '$');
                maze.setCell(x_final, y_final, '!');

                cout << endl;
                maze.display();
            }
            else
                cout << "Goodbye!" << endl;
        }

        attempts++;
    }

    return 0;
}
