#include "C:\Users\noaha\Desktop\C++\std_lib_facilities.h"

const int MAX = 200;
const int ROWS = 4, COLS = 4;
const string empty_tile = ".";
const string obstacle_sprite = "█";
const string item_sprite = "♠";

//------------------------------------------------------------------------------

class Position {
public:
    int row;
    int col;
    Position(int r, int c)
        :row(r), col(c) { }
};

//------------------------------------------------------------------------------

class Item {
public:
    Position get_pos() const;
    Item(int row, int col)
        :pos(row,col) { }
private:
    Position pos;
};

//------------------------------------------------------------------------------

Position Item::get_pos() const {
    return pos;
}

//------------------------------------------------------------------------------

class Obstacle {
public:
    Position get_pos() const;
    Obstacle(int row, int col)
        :pos(row,col) { }
private:
    Position pos;
};

//------------------------------------------------------------------------------

Position Obstacle::get_pos() const {
    return pos;
}

//------------------------------------------------------------------------------

const string player_sprite = "@";
const int full = 3;

class Player {
public:
    int get_health() const;
    void set_health(int hp);
    Position get_pos() const;
    void set_pos(int row,int col);
    bool obtained_item() const;
    void set_has_item(bool truth_val);
    // bool inventory_has(int crystal) const;
    // void add_item_to_inventory(int crystal);
    void move(char direction);
    void use_potion();
    void attack();
    Player(int row, int col)
        :pos(row,col), health(full), potions(full), has_item(false) { }
private:
    Position pos;
    int health;
    int potions;
    bool has_item;
    // vector<int> key_inventory;
};

//------------------------------------------------------------------------------

int Player::get_health() const {
    return health;
}

//------------------------------------------------------------------------------

// Error if we set hp to less than 0 or more than full health
// if health < 0, set to 0
void Player::set_health(int hp) {
    health = hp;
}

//------------------------------------------------------------------------------

Position Player::get_pos() const {
    return pos;
}

//------------------------------------------------------------------------------

/*
Error if either row or col is less than 0 or row or col is more than the size of the map; use outside range function
*/

void Player::set_pos(int row, int col) {
    pos.row = row;
    pos.col = col;
}

//------------------------------------------------------------------------------

// Added check to see if player has potions; if not, can't use_potion();
void Player::use_potion() {
    if(potions > 0) {
        health = full;
        --potions;
    }
}

//------------------------------------------------------------------------------

// Define void Player::attack(Enemy& enemy) here, if we make class Enemy
// if health < 0, set to 0

//------------------------------------------------------------------------------

// move() was supposed to go here... but needs to be placed elsewhere to work

//------------------------------------------------------------------------------

bool Player::obtained_item() const {
    return has_item;
}

//------------------------------------------------------------------------------

void Player::set_has_item(bool truth_val) {
    has_item = truth_val;
}

//------------------------------------------------------------------------------

class World_map {
public:
    void print() const;
    const string& look(int row, int col) const;
    bool is_outside_range(int row, int col) const;
    void place(const Player&); 
    void place(const Item&); 
    void place(const Obstacle&);
    void remove(Player&);
    void remove(Item&);
    void remove(Obstacle&);
private:
    string grid[ROWS][COLS] = {
        {".", ".", ".", "."},
        {".", ".", ".", "."},
        {".", ".", ".", "."},
        {".", ".", ".", "."}
    };
};

//------------------------------------------------------------------------------

void World_map::print() const {
    cout << "╔═══════════════════╗\n"
         << "║                   ║\n";

    for (int row = 0; row < ROWS; row++) {
        cout << "║   ";

        for(int col = 0; col < COLS; col++) {
            cout << grid[row][col] << "   ";
        }

        cout << "║\n";
        cout << "║                   ║\n";
    }
    
    cout << "╚═══════════════════╝\n\n";
}

//------------------------------------------------------------------------------

const string& World_map::look(int row, int col) const {
    return grid[row][col];
}

//------------------------------------------------------------------------------

/*
Add pre-condition error-checking:
1) Make sure player pos is within boundaries (rnge chck does it already kinda)
2) Make sure not placing player_sprite on top of non-empty tile (unless item) (we already kind of check for this in move(), the player isn't going to have its position be overlapping with an obstacle/non-empty cell)
*/

void World_map::place(const Player& player) {
    int player_row = player.get_pos().row;
    int player_col = player.get_pos().col;

    grid[player_row][player_col] = player_sprite;
}

//------------------------------------------------------------------------------

void World_map::place(const Item& item) {
    int item_row = item.get_pos().row;
    int item_col = item.get_pos().col;

    grid[item_row][item_col] = item_sprite;
}

//------------------------------------------------------------------------------

void World_map::place(const Obstacle& obstacle) {
    int obstacle_row = obstacle.get_pos().row;
    int obstacle_col = obstacle.get_pos().col;

    grid[obstacle_row][obstacle_col] = obstacle_sprite;
}

//------------------------------------------------------------------------------

void World_map::remove(Player& player) {
    int player_row = player.get_pos().row;
    int player_col = player.get_pos().col;

    grid[player_row][player_col] = empty_tile;
}

//------------------------------------------------------------------------------

void World_map::remove(Item& item) {
    int item_row = item.get_pos().row;
    int item_col = item.get_pos().col;

    grid[item_row][item_col] = empty_tile;
}

//------------------------------------------------------------------------------

void World_map::remove(Obstacle& obstacle) {
    int obstacle_row = obstacle.get_pos().row;
    int obstacle_col = obstacle.get_pos().col;

    grid[obstacle_row][obstacle_col] = empty_tile;
}

//------------------------------------------------------------------------------


// Definitely needs explanation
bool World_map::is_outside_range(int row, int col) const {
    if(row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

World_map overworld;

//------------------------------------------------------------------------------

// Function so I can easily swap clear command if I find a better one
void clear_screen() {
    system("cls");
}

//------------------------------------------------------------------------------

void prompt_next() {
    char c;
    while(cin >> c && c != 'z');
    cin.ignore(MAX,'\n');
}

//------------------------------------------------------------------------------

const char up_key = 'w';
const char left_key = 'a';
const char down_key = 's';
const char right_key = 'd';

void Player::move(char direction) {
    int new_row = get_pos().row;
    int new_col = get_pos().col;

    switch(direction) {
        case up_key:
            --new_row;
            break;
        case left_key:
            --new_col;
            break;
        case down_key:
            ++new_row;
            break;
        case right_key:
            ++new_col;
            break;
        default:
            // Must move with [w,a,s,d] keys
            return;
    }

    if(overworld.is_outside_range(new_row,new_col)) {
        // Can't move out of bounds or into walls
        return;
    }

    string new_cell = overworld.look(new_row,new_col);

    if(new_cell == item_sprite) {
        set_has_item(true);
    }

    if(new_cell == item_sprite || new_cell == empty_tile) {
        overworld.remove(*this);
        set_pos(new_row,new_col);
        overworld.place(*this);
    }
    else {
        // Can't move into full cells
        return;
    }
}

//------------------------------------------------------------------------------

// Maybe put clear_screen() here instead

//------------------------------------------------------------------------------

// Maybe put prompt_next() here instead

//------------------------------------------------------------------------------

const char quit = 'q';
// const char help = 'h';

int main() try {
    system("chcp 65001");
    clear_screen();

    Player player{1,1};
    overworld.place(player);

    Item fire_crystal{2,2};
    overworld.place(fire_crystal);

    Obstacle block{1,2};
    overworld.place(block);

    while(true) {
        cout << "title (Press z to continue)\n"
             << "> ";
        prompt_next();

        clear_screen();
        overworld.print();

        while(true) {
            cout << "Use [w,a,s,d] to move\n"
                 << "> ";
            char direction = ' ';
            cin >> direction;
            cin.ignore(MAX,'\n');
            if(direction == quit) break;

            player.move(direction);
            clear_screen();
            overworld.print();
        
            if(player.obtained_item()) {
                // Add item to inventory or spell_inventory
                player.set_has_item(false);
                cout << "Item Get! (Press z for next)\n"
                     << "> ";
                prompt_next();
                clear_screen();
                overworld.print();
            }

        }

        cout << "\nReturn to title screen? (y/n)\n"
             << "> ";
        char answer = ' ';
        cin >> answer;
        cin.ignore(MAX,'\n');

        if(answer != 'y') {
            break;
        }
        else {
            // Clear the entire map
            clear_screen();
            continue;
        }
    }
    return 0;
}
catch(exception& e) {
    cerr << "Runtime Error: " << e.what() << "\n";
    cout << "Please enter z to exit\n";
    prompt_next();
    return 1;
}
catch(...) {
    cerr << "Unknown Exception!\n";
    cout << "Please enter z to exit\n";
    prompt_next();
    return 2;
}

//------------------------------------------------------------------------------

/*

//
class Enemy will be exactly the same as class Player, but,

1) Will have a different definition for move(); implement Enemy AI
2) iff enemy uses potions, use_potions is defined differently; /w Enemy AI
//


//
1) Destructor for all game entities
2) Check vicinity after player moves or when enemy's turn or move()
//


//
enemy.in_vicinity(Player&)

if(enemy.in_vicinity(player)) {
    // battle_scene();
}
//


//
Fix up game loop in main(), consider having display_map() or update_map() or refresh_screen() instead of 
1) clear_screen();
2) overworld.print();
//


//
Simplify void Player::move(); to improve code layout and reduce size of move() by redefining void World_map::place()

place() be redefined somehow to set player pos and place the player_sprite, but ofc want it to not take a bunch of parameters like I had before, i.e don't do this:

1) overworld.place(player,player.get_pos().row,player.get_pos().col);

or, you could do this in void World_map::place() or call place() this:

display_map(player); instead

MORAL OF THE STORY:
It would be easier to decide on things regarding class design (i.e, how to define a class such that it is simple and follows best practices) if I read up on it in Chapter 9 of P:PP Using C++... (At least to have some more guiding principles beyond the very basics like getters, setters, and "common sense" for lack of a better word)
//


//
player_attack_animation();
enemy_attack_animation();
//


move():
// REMOVE player_sprite
// SET player.pos
// PLACE player_sprite

the idea of place and remove is to handle the actual sprites on the screen, and SET will set the pos; which handles the data relating to the player position that will allow logic related to movement, collision, item get, and player-enemy vicinity
//

*/