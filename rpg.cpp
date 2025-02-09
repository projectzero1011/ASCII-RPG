#include <iostream>
#include <limits>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

const int ROWS = 4, COLS = 4;
const string null_cell = "";
const string empty_cell = ".";
const string block_sprite = "█";
const string item_sprite = "♠";

const string player_sprite = "@";
const string enemy_sprite = "%";

//------------------------------------------------------------------------------

struct Position {
public:
    Position(int row, int col);
    int row() const { return r; }
    int col() const { return c; }
private:
    int r;
    int c;
};

//------------------------------------------------------------------------------

bool is_out_of_range(int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return true;
    return false;
}

//------------------------------------------------------------------------------

Position::Position(int row, int col) 
    : r{row}, c{col}
{
    if (is_out_of_range(row,col)) throw runtime_error("Invalid Position");
}

//------------------------------------------------------------------------------

class Entity {
public:
    Entity(Position p) : pos{p} { }

    int row() const { return pos.row(); }
    int col() const { return pos.col(); }

    virtual string sprite() const = 0;
protected:
    Position pos;
};

//------------------------------------------------------------------------------

class World_map {
public:
    void print() const;
    void refresh() const;
    string look(int row, int col) const;
    void place(const Entity&);
    void remove(Entity& e) { grid[e.row()][e.col()] = empty_cell; }
    void erase(Position pos) { grid[pos.row()][pos.col()] = empty_cell; }
private:
    string grid[ROWS][COLS]
    {
        {".", ".", ".", "."},
        {".", ".", ".", "."},
        {".", ".", ".", "."},
        {".", ".", ".", "."}
    };
};

//------------------------------------------------------------------------------

void clear_screen() {
    system("cls");
}

//------------------------------------------------------------------------------

void World_map::print() const {
    cout << "╔═══════════════════╗\n"
         << "║                   ║\n";

    for(int row = 0; row < ROWS; row++) {
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

void World_map::refresh() const {
    clear_screen();
    print();
}

//------------------------------------------------------------------------------

string World_map::look(int row, int col) const {
    if (is_out_of_range(row,col)) return null_cell;
    return grid[row][col];
}

//------------------------------------------------------------------------------

void World_map::place(const Entity& e) {
    if (grid[e.row()][e.col()] != empty_cell) {
        throw runtime_error("Entity Overlap");
    }
    grid[e.row()][e.col()] = e.sprite();
}

//------------------------------------------------------------------------------

class Item : public Entity {
public:
    using Entity::Entity;
    string sprite() const override { return "♠"; }
};

//------------------------------------------------------------------------------

class Block : public Entity {
public:
    using Entity::Entity;
    string sprite() const override { return "█"; }
};

//------------------------------------------------------------------------------

class Actor : public Entity {
public:
    int hp() const { return h; }
    int max_hp() const { return max_h; }
    void decr_hp(int dmg) { h -= dmg; if (h < 0) h = 0; }
    void incr_hp(int amt) { h += amt; if (h > max_h) h = max_h; }

    int mp() const { return m; }
    int max_mp() const { return max_m; }
    void decr_mp(int amt) { m -= amt; if (m < 0) m = 0; }
    void incr_mp(int amt) { m += amt; if (m > max_m) m = max_m; }
protected:
    Actor(Position p, int hp, int mp) 
        : Entity{p}, h{hp}, max_h{hp}, max_m{mp} { }
private:
    int h, max_h;
    int m {0}, max_m;
};

//------------------------------------------------------------------------------

enum class Selection {
    start,
    help,
    invalid
};

//------------------------------------------------------------------------------

Selection title_input(string message) {
    cout << message << "\n"
         << "> ";
    string input;
    cin >> input;
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
    char ch = (input.size() == 1) ? input[0] : ' ';

    switch(ch) {
        case 'z': return Selection::start; break;
        case 'h': return Selection::help; break;
        default: return Selection::invalid; break;
    }
}

//------------------------------------------------------------------------------

enum class Key : char {
    up = 'w',
    left = 'a',
    down = 's',
    right = 'd',
    help = 'h',
    // quit
    invalid
};

//------------------------------------------------------------------------------

Key world_input(string message) {
    cout << message << "\n"
         << "> ";
    string input;
    cin >> input;
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
    char ch = (input.size() == 1) ? input[0] : ' ';

    switch(ch) {
        case 'w': return Key::up;
        case 'a': return Key::left;
        case 's': return Key::down;
        case 'd': return Key::right;
        case 'h': return Key::help;
        default:  return Key::invalid;
    }
}

//------------------------------------------------------------------------------

enum class Option {
    attack = 1,
    parry = 2,
    heal = 3,
    fire = 7,
    ice = 8,
    lightning = 9,
    help,
    invalid
};

//------------------------------------------------------------------------------

bool is_battle_option(Option op) {
    switch(op) {
        case Option::invalid: return false; break;
        case Option::help:    return false; break;
        default: return true; break;
    }
}

//------------------------------------------------------------------------------

Option battle_input(string message) {
    cout << message << "\n"
         << "> ";
    string input;
    cin >> input;
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
    char ch = (input.size() == 1) ? input[0] : ' ';

    switch(ch) {
        case '1': return Option::attack;
        case '2': return Option::parry;
        case '3': return Option::heal;
        case '7': return Option::fire;
        case '8': return Option::ice;
        case '9': return Option::lightning;
        case 'h': return Option::help;
        default:  return Option::invalid;
    }
}

//------------------------------------------------------------------------------

enum class Spell {
    fire,
    ice,
    lightning
};

//------------------------------------------------------------------------------

class Battle_scene;

//------------------------------------------------------------------------------

class Enemy;

//------------------------------------------------------------------------------

class Player : public Actor {
public:
    Player(Position pos, int hp, int mp) : Actor::Actor(pos,hp,mp) { }
    string sprite() const override { return "@"; }

    void move(Key input, World_map& world);
    void attack(Enemy&,Battle_scene&);
    void parry();
    void cast(Spell,Enemy&,Battle_scene&);
    void heal(Battle_scene&);

    bool has_item() const { return item; }
    void set_has_item(bool b) { item = b; }
    bool is_parry() const { return parrying; }
    bool parry_success() const { return parried; }
    void reset_parry() { parrying = false; parried = false; }
private:
    bool parrying {false}, parried {false};
    bool item {false};
};

//------------------------------------------------------------------------------

// add get weakness, set weakness, and private member weakness
class Enemy : public Actor {
public:
    Enemy(Position pos, int hp, int mp) : Actor::Actor(pos,hp,mp) { }
    string sprite() const override { return "%"; }

    void move(const Player&, World_map& world);
    void attack(Player&, Battle_scene&);

    bool in_vicinity(const Player&, const World_map&);
};


// Putting enemy here will fix error of not declaring enemy before 
// refresh_battle() in line 224, do this to implement battle msgs

//------------------------------------------------------------------------------

enum class Frame {
    title,
    help
};

//------------------------------------------------------------------------------

enum class Battle_frame {
    battle,
    player_attack,
    // player_parry?
    player_fire,
    player_ice,
    player_ltng,
    enemy_attack,
};

//------------------------------------------------------------------------------

const vector<string> frame_tbl = {"title", "help"};

//------------------------------------------------------------------------------

const vector<string> battle_frame_tbl = {"battle", "player_attack",
"player_fire", "player_ice", "player_ltng", "enemy_attack"};

//------------------------------------------------------------------------------

// bool find(vector<string> v, string& name)

//------------------------------------------------------------------------------

// declare a constant for the file extension of frames
// declare a constant for the file path
// helper function to class Frame_manager
// needs pre-condition to prevent random string inputs
string load(string& name) {
    ifstream ist {name + ".txt"};
    if (!ist) throw runtime_error("Can't open input file ");

    string frame;
    for (char ch; ist.get(ch);) frame += ch;

    return frame;
}

//------------------------------------------------------------------------------

class Frame_manager {
public:
    Frame_manager();
    void print(Frame f) const { cout << frames[int(f)]; }
    void print(Battle_frame bf) const { cout << battle_frames[int(bf)]; }
private:
    vector<string> frames;
    vector<string> battle_frames;
};

//------------------------------------------------------------------------------

Frame_manager::Frame_manager() {
    for(string fname : frame_tbl) {
        string f = load(fname);
        frames.push_back(f);
    }

    for(string bfname : battle_frame_tbl) {
        string bf = load(bfname);
        battle_frames.push_back(bf);
    }
}

//------------------------------------------------------------------------------

class Battle_scene {
public:
    Battle_scene(Player& pp, Enemy& ee, Frame_manager& fmm) 
        : p{pp}, e{ee}, fm{fmm}, last{Battle_frame::battle} { }
    void print(Battle_frame bf);
    void refresh() { clear_screen(); print(Battle_frame::battle); }
    void print_last() { print(last); }
    void refresh_last() { clear_screen(); print_last(); }
private:
    Player& p;
    Enemy& e;
    Frame_manager& fm;
    Battle_frame last;
};

//------------------------------------------------------------------------------

void Battle_scene::print(Battle_frame bf) {
    last = bf;
    clear_screen();
    cout << "Player" << "\t\t" << setw(8) << "Enemy" << "\n"
         << "HP " << setw(2) << p.hp() << "/" << p.max_hp() << "\t" 
         << "HP " << setw(2) << e.hp() << "/" << e.max_hp() << "\n" 
         << "MP  " << p.mp() << "/ " << p.max_mp() << "\t" << "\n\n";
    fm.print(bf);
}

//------------------------------------------------------------------------------

void prompt_next(string message) {
    cout << message + " [z: next]" << "\n"
         << "> ";
    string s;
    cin >> s;
}

//------------------------------------------------------------------------------

void prompt_next(string message, const World_map& world) {
    char ch = ' ';
    while(ch != 'z') {
        cout << message + " [z: next]" << "\n"
             << "> ";
        string input;
        cin >> input;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
        ch = (input.size() == 1) ? input[0] : ' ';
        world.refresh();
    }
}

//------------------------------------------------------------------------------

void prompt_next(string message, Battle_scene& battle) {
    char ch = ' ';
    while(ch != 'z') {
        cout << message + " [z: next]" << "\n"
             << "> ";
        string input;
        cin >> input;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
        ch = (input.size() == 1) ? input[0] : ' ';
        battle.refresh_last();
    }
}

//------------------------------------------------------------------------------

void prompt_next(string message, Frame f, const Frame_manager& fm) {
    char ch = ' ';
    while(ch != 'z') {
        cout << message + " [z: next]" << "\n"
             << "> ";
        string input;
        cin >> input;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
    
        ch = (input.size() == 1) ? input[0] : ' ';
        clear_screen();
        fm.print(f);
    }
}

//------------------------------------------------------------------------------

void show_help(const Frame_manager& fm) {
    clear_screen();
    fm.print(Frame::help);
    prompt_next("Back",Frame::help,fm);
}

//------------------------------------------------------------------------------

// void show_spell_options() 

//------------------------------------------------------------------------------

// input functions here so Player & Actor definitions aren't separated

//------------------------------------------------------------------------------

void Player::move(Key input, World_map& world) {
    int new_row = row();
    int new_col = col();

    switch(input) {
        case Key::up:    --new_row; break;
        case Key::left:  --new_col; break;
        case Key::down:  ++new_row; break;
        case Key::right: ++new_col; break;
        default: throw runtime_error("Invalid key");
    }

    string new_cell = world.look(new_row,new_col);

    if(new_cell == item_sprite || new_cell == empty_cell) {
        world.erase(Position{new_row,new_col});
        world.remove(*this);
        pos = Position{new_row,new_col};
        world.place(*this);
    }

    if(new_cell == item_sprite) {
        set_has_item(true);
        world.refresh();
        prompt_next("Item Get!",world);
    }
}

//------------------------------------------------------------------------------

// implement message that says "Player dealt [x] DMG!"
void Player::attack(Enemy& enemy, Battle_scene& battle) {
    int roll = rand() % 10;
    bool hit = (roll == 0) ? false : true;
    int dmg = 2 + (rand() % 3);

    if(hit) {
        enemy.decr_hp(dmg);
        incr_mp(1);
        battle.print(Battle_frame::player_attack);
        prompt_next("Player dealt " + to_string(dmg) + " DMG!",battle);
    }
    else {
        battle.print(Battle_frame::player_attack);
        prompt_next("Miss!",battle);
    }
}

//------------------------------------------------------------------------------

void Player::parry() {
    parrying = true;
    int roll = rand() % 10;
    (roll <= 6) ? parried = true : parried = false;
}

//------------------------------------------------------------------------------

// implement messages that say "Player dealt [x] DMG!"
void Player::cast(Spell s, Enemy& enemy, Battle_scene& battle) {
    switch(s) {
        case Spell::fire:
            decr_mp(1);
            enemy.decr_hp(6);
            battle.print(Battle_frame::player_fire);
            prompt_next("Cast fire!",battle);
            // chance of burn
            break;
        case Spell::ice:
            decr_mp(2);
            enemy.decr_hp(8);
            battle.print(Battle_frame::player_ice);
            prompt_next("Cast ice!",battle);
            // chance of freeze
            break;
        case Spell::lightning:
            decr_mp(3);
            enemy.decr_hp(10);
            battle.print(Battle_frame::player_ltng);
            prompt_next("Cast lightning!",battle);
            // chance of slow
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------

void Player::heal(Battle_scene& battle) {
    int amt = max_hp()*(2.0/3.0);
    incr_hp(amt);
    decr_mp(2);
    battle.refresh();
    prompt_next("Player healed " + to_string(amt) + " HP!",battle);
}

//------------------------------------------------------------------------------

// class Enemy was here

//------------------------------------------------------------------------------

// Needs work (particularly in simplifying the conditionals)
// Could use if (player_to_left) or (player_to_right), etc
void Enemy::move(const Player& player, World_map& world) {
    int player_row = player.row();
    int player_col = player.col();
    int new_row = row();
    int new_col = col();

    if (player_row == new_row && player_col < new_col) --new_col;
    else if (player_row == new_row && player_col > new_col) ++new_col;

    if (player_row < new_row) --new_row;
    else if (player_row > new_row) ++new_row;

    string new_cell = world.look(new_row,new_col);

    if(new_cell == empty_cell) {
        world.remove(*this);
        pos = Position{new_row,new_col};
        world.place(*this);
    }
}

//------------------------------------------------------------------------------

// Could use more work
void Enemy::attack(Player& player, Battle_scene& battle) {
    int roll = rand() % 10;
    bool hit = (roll == 0) ? false : true;
    int dmg = 3 + (rand() % 3);

    if(hit) {
        if(player.parry_success()) {
            player.incr_mp(1);
            battle.refresh();
            prompt_next("Parry successful!",battle);
        }
        else if(!player.parry_success() && player.is_parry()) {
            player.decr_hp(dmg);
            battle.print(Battle_frame::enemy_attack);
            prompt_next("Parry fail!",battle);
        }
        else {
            player.decr_hp(dmg);
            battle.print(Battle_frame::enemy_attack);
            prompt_next("Enemy dealt " + to_string(dmg) + " DMG!",battle);
        }
    }
    else {
        battle.print(Battle_frame::enemy_attack);
        prompt_next("Enemy miss!",battle);
    }
}

//------------------------------------------------------------------------------

// getting enemy vicinity cells can be helper functions
// needs to be reworked
bool Enemy::in_vicinity(const Player& p, const World_map& world) {
    string enemy_left = world.look(row(), col() - 1);
    string enemy_right = world.look(row(), col() + 1);
    string enemy_up = world.look(row() - 1, col());
    string enemy_down = world.look(row() + 1, col());

    if (enemy_left == p.sprite()) return true;
    if (enemy_right == p.sprite()) return true;
    if (enemy_up == p.sprite()) return true;
    if (enemy_down == p.sprite()) return true;

    return false;
}

//------------------------------------------------------------------------------

int main() try {
    system("chcp 65001");
    clear_screen();
    
    // generate random seed
    srand(time(nullptr));

    World_map world;
    
    Player player {Position{2,2},30,5};
    Enemy enemy {Position{0,0},99,3};
    Item fire_crystal {Position{1,0}};
    Block block {Position{1,1}};

    world.place(player);
    world.place(enemy);
    world.place(fire_crystal);
    world.place(block);

    Frame_manager fm;

    while(true) {
        /* Title */

        fm.print(Frame::title);
        Selection input = Selection::invalid;

        while(input != Selection::start) {
            input = title_input("Welcome to Aegis");

            switch(input) {
                case Selection::start:   
                    break;
                case Selection::help:    
                    show_help(fm); 
                    break;
                case Selection::invalid: 
                    break;
                default: throw runtime_error("Selection doesn't exist!");
            }

            clear_screen();
            fm.print(Frame::title);
        }


        /* World */

        world.refresh();

        while(!enemy.in_vicinity(player,world)) {
            Key input = Key::invalid;

            while(input == Key::invalid) {
                world.refresh();
                input = world_input("Use [w,a,s,d] to move [h: help]");

                switch(input) {
                    case Key::up: case Key::left: case Key::down: 
                    case Key::right:
                        player.move(input,world);
                        world.refresh();
                        break;
                    case Key::help:
                        show_help(fm);
                        break;
                    //case Key::quit:
                    case Key::invalid:
                        break;
                    default:
                        throw runtime_error("Key doesn't exist!");
                }
            }

            // item is still true here if picked up item

            enemy.move(player,world);
            world.refresh();
        }

        prompt_next("Enemy engaged!",world);
        

        /* Battle */

        /* Player Turn */

        Battle_scene battle {player,enemy,fm};
        battle.refresh();

        while(enemy.hp() > 0 && player.hp() > 0) {
            player.reset_parry();
            Option p_choice = Option::invalid;

            while(!is_battle_option(p_choice)) {
                p_choice = battle_input("What will you do? [h: help]");

                switch(p_choice) {
                    case Option::attack:
                        player.attack(enemy,battle);
                        break;
                    case Option::parry:
                        player.parry();
                        break;
                    case Option::heal:
                        if(player.mp() < 2) { 
                            p_choice = Option::invalid;
                            battle.refresh();
                            prompt_next("Heal requires 2 MP!",battle);
                            battle.refresh();
                        }
                        if(p_choice != Option::invalid) {
                            player.heal(battle);
                        }
                        break;
                    case Option::fire:
                        if(player.mp() < 1) { 
                            p_choice = Option::invalid;
                            battle.refresh();
                            prompt_next("Fire requires 1 MP!",battle);
                            battle.refresh();
                        }
                        if(p_choice != Option::invalid) {
                            player.cast(Spell::fire,enemy,battle);
                        }
                        break;
                    case Option::ice:
                        if(player.mp() < 2) { 
                            p_choice = Option::invalid;
                            battle.refresh();
                            prompt_next("Ice requires 2 MP!",battle);
                            battle.refresh();
                        }
                        if(p_choice != Option::invalid) {
                            player.cast(Spell::ice,enemy,battle);
                        }
                        break;
                    case Option::lightning:
                        if(player.mp() < 3) { 
                            p_choice = Option::invalid;
                            battle.refresh();
                            prompt_next("Ltng requires 3 MP!",battle);
                            battle.refresh();
                        }
                        if(p_choice != Option::invalid) {
                            player.cast(Spell::lightning,enemy,battle);
                        }
                        break;
                    /*
                    case Option::aegis:
                        // if (!is_item_ready(player) { battle.refresh(); }
                        if (!player.has_item()) { battle.refresh(); }
                        if (player.is_item_use()) { battle.refresh(); }
                        // cast aegis
                        player.set_item_use(true);
                        break;
                    */
                    case Option::help:
                        show_help(fm);
                        battle.refresh();
                        break;
                    /*
                    case Option::spell:
                        show_spell_options(fm);
                        break;
                    */
                    case Option::invalid:
                        battle.refresh();
                        break;
                    default:
                        throw runtime_error("Option doesn't exist!");
                        break;
                }
            }

            if (enemy.hp() == 0) break;

            /* Enemy Turn */

            // Enemy_option enemy_choice = enemy_input();

            // switch(enemy_choice) 
            
            enemy.attack(player,battle);

            if (player.hp() == 0) break;

            battle.refresh();
        }

        clear_screen();
        
        if (player.hp() == 0) prompt_next("You lost!");
        if (enemy.hp() == 0) prompt_next("You won!");

        // Answer ans = end_input(); 

        cout << "\nReturn to title screen? (y/n)\n"
             << "> ";
        char answer = ' ';
        cin >> answer;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');

        if (answer != 'y') break;

        clear_screen();
    }
    return 0;
}
catch(exception& e) {
    cerr << "Error: " << e.what() << "\n\n";
    prompt_next("Next");
    return 1;
}
catch(...) {
    cerr << "Unknown Exception!\n\n";
    prompt_next("Next");
    return 2;
}

//------------------------------------------------------------------------------
