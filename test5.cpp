#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <algorithm>


// --- Game State Variables ---
int player_life = 5, enemy_life = 5;
int roundIndex = 1;
bool playAgain = true, restartGame = false;
int magniGlassCount = 0, healCount = 0, doubleDamageCount = 0, handcuffCount = 0;
bool playerDD = false, enemyDD = false, enemyHandcuffed = false, itemSelected = false, thisRoundDD = false;
int whoseTurn = 0, selected = 0, bulletNow = 0;
std::vector<int> barrel(6, 0), randBarrel(6, 0);



// --- UI Layout Variables ---
int box_x = 6, box_y = 20, box_width = 90, box_height = 9;
int action_box_width = 21, action_box_height = 3;
int action_y = box_y + box_height;
int action1_x = 24, action2_x = action1_x + action_box_width + 2, action3_x = action2_x + action_box_width + 2;
    
    

// --- Random Generators ---
std::random_device rd;
std::mt19937 g(rd());
std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist6(0,1);



// --- Utility Functions ---
void clearBox();



void set_cursor_position(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void draw_box(int width, int height, int x_offset = 0, int y_offset = 0) {
    set_cursor_position(x_offset, y_offset);
    std::cout << "+" << std::string(width - 2, '-') << "+" << std::endl;
    for (int i = 0; i < height - 2; ++i) {
        set_cursor_position(x_offset, y_offset + 1 + i);
        std::cout << "|" << std::string(width - 2, ' ') << "|" << std::endl;
    }
    set_cursor_position(x_offset, y_offset + height - 1);
    std::cout << "+" << std::string(width - 2, '-') << "+" << std::endl;
}


void draw_box_alt(int width, int height, int x_offset = 0, int y_offset = 0) {
    set_cursor_position(x_offset, y_offset);
    std::cout << "+" << std::string(width - 2, '>') << "+" << std::endl;
    for (int i = 0; i < height - 2; ++i) {
        set_cursor_position(x_offset, y_offset + 1 + i);
        std::cout << "|" << std::string(width - 2, ' ') << "|" << std::endl;
    }
    set_cursor_position(x_offset, y_offset + height - 1);
    std::cout << "+" << std::string(width - 2, '<') << "+" << std::endl;
}


void loading_dots(){
    for(int i = 0; i < 5; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << ".";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}


void print_in_box(const std::string& message, int box_x, int box_y, int box_width, int box_height);


void print_in_box_animated(const std::string& message, int box_x, int box_y, int box_width, int box_height, int delay_ms);
void updateLife(){
    print_in_box("       ", box_x - 36, box_y + 5, box_width, box_height);
    print_in_box("         ", box_x - 36, box_y + 6, box_width, box_height);
    print_in_box("      ", box_x - 36, box_y + 7, box_width, box_height);
    print_in_box("         ", box_x - 36, box_y + 8, box_width, box_height);
    print_in_box("Your life:\n    |  ", box_x - 34, box_y + 5, box_width, box_height);
    for(int i = 0; i < player_life; i++) std::cout << "# ";
    print_in_box("Enemy's life:\n    |  ", box_x - 33, box_y + 7, box_width, box_height);
    for(int i = 0; i < enemy_life; i++) std::cout << "# ";
    set_cursor_position(0, action_y + action_box_height + 4);
}


void print_in_box(const std::string& message, int box_x, int box_y, int box_width, int box_height) {
    int text_x = box_x + (box_width - 2 - message.length()) / 2 + 1; // Center text horizontally
    int text_y = box_y + (box_height - 2) / 2 + 1; // Center text vertically
    set_cursor_position(text_x, text_y);
    std::cout << message << std::flush;
}


void print_in_box_animated(const std::string& message, int box_x, int box_y, int box_width, int box_height, int delay_ms = 40) {
    int text_x = box_x + (box_width - 2 - message.length()) / 2 + 1; // Center text horizontally
    int text_y = box_y + (box_height - 2) / 2 + 1; // Center text vertically
    set_cursor_position(text_x, text_y);
    for (char c : message) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}


void displayBarrel(){
    int lives = 0;
    int blank = 0;
    std::string barrelMessage = "Barrel status: ";
    for(int i = 0; i < 6; i++){
        if(barrel[i] == 1){
            barrelMessage += "[*] ";
            lives++;
        }
        else if(barrel[i] == 0){
            barrelMessage += "[ ] ";
            blank++;
        }
    }
    print_in_box(barrelMessage, box_x, box_y, box_width, box_height);
    print_in_box(std::to_string(lives) + " live bullets and " + std::to_string(blank) + " blanks.", box_x, box_y + 1, box_width, box_height);
    std::cout << std::endl << std::endl;
}


void give_random_items() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3); // 4 item types
    int healC = 0;
    int magniGlassC = 0;
    int doubleDamageC = 0;
    int handcuffC = 0;
    int lineIndex = 0;
    for (int i = 0; i < 3; ++i) {
        int item = dist(gen);
        if (item == 0) {
            healCount++;
            healC++;
        } else if (item == 1) {
            magniGlassCount++;
            magniGlassC++;
        } else if(item == 2){
            doubleDamageCount++;
            doubleDamageC++;
        } else{
            handcuffCount++;
            handcuffC++;
        }
    }
    clearBox();
    print_in_box_animated("You received: ", box_x, box_y - 2, box_width, box_height, 40);
    if(healC > 0){
        print_in_box_animated("- " + std::to_string(healC) + " Heal item(s)", box_x, box_y - 1, box_width, box_height, 40);
        lineIndex++;
    }
    if(magniGlassC > 0){
        print_in_box_animated("- " + std::to_string(magniGlassC) + " Magnifying Glass(s)", box_x, box_y - 1 + lineIndex, box_width, box_height, 40);
        lineIndex++;
    }
    if(doubleDamageC > 0){
        print_in_box_animated("- " + std::to_string(doubleDamageC) + " Double Damage item(s)", box_x, box_y - 1 + lineIndex, box_width, box_height, 40);
        lineIndex++;
    }
    if(handcuffC > 0){
        print_in_box_animated("- " + std::to_string(handcuffC) + " Handcuff(s)", box_x, box_y - 1 + lineIndex, box_width, box_height, 40);
    }
}


void shoot(int which, bool isDoubleDamage){
    std::string message = "";
    if(which == 1 && bulletNow == 1 && enemyDD){
        player_life -= 2;
        whoseTurn = 0;
        enemyDD = false;
        message = "You were shot by a double damage bullet. Your health drops by 2";
    }else if(which == 0 && bulletNow == 1 && playerDD){
        enemy_life -= 2;
        whoseTurn = 1;
        message = "You shot him using Double Damage, causing his health to drop by 2.";
    } else if(which == 0 && bulletNow == 1){
        enemy_life -= 1;
        whoseTurn = 1;
        message = "You shot him with a live bullet";
    }
    else if(which == 0 && bulletNow == 0){
        whoseTurn = 1;
        message = "You shot him with a blank";
    }
    else if(which == 1 && bulletNow == 1){
        player_life -= 1;
        whoseTurn = 0;
        message = "He shot you with a live bullet";
    }
    else if(which == 1 && bulletNow == 0){
        whoseTurn = 0;
        message = "He shot you with a blank";
    }
    clearBox();
    print_in_box_animated(message, box_x, box_y, box_width, box_height, 40);
    updateLife();
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
}


void shootSelf(int which, bool isDoubleDamage){
    std::string message = "";
    if(which == 0 && bulletNow == 1 && playerDD){
        player_life -= 2;
        whoseTurn = 1;
        message = "You shot yourself using Double Damage, causing your health to drop by 2.";
    } else if(which == 0 && bulletNow == 1){
        player_life -= 1;
        whoseTurn = 1;
        message = "You shot yourself with a live bullet";
    }
    else if(which == 0 && bulletNow == 0){
        message = "You shot yourself, but luckily it was a blank";
    }
    else if(which == 1 && bulletNow == 1){
        enemy_life -= 1;
        whoseTurn = 0;
        message = "The enemy shot himself with a live bullet";
    }
    else if(which == 1 && bulletNow == 0){
        message = "The enemy shot himself, but it was a blank";
    }
    clearBox();
    print_in_box_animated(message, box_x, box_y, box_width, box_height, 40);
    updateLife();
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
}


void clearBox(){ // clear the box conveniently, my favourite :)
    print_in_box("                                                                        ", box_x, box_y-2, box_width, box_height);
    print_in_box("                                                                        ", box_x, box_y-1, box_width, box_height);
    print_in_box("                                                                        ", box_x, box_y, box_width, box_height);
    print_in_box("                                                                        ", box_x, box_y+1, box_width, box_height);
    print_in_box("                                                                        ", box_x, box_y + 2, box_width, box_height);
}


void user_input(){
    bool noMessage = true;
                while(whoseTurn == 0){
                    if(noMessage == true){ //Show message when needed
                        clearBox();
                        print_in_box_animated("It's your turn", box_x, box_y, box_width, box_height, 40);
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        print_in_box("press 'a' or 'd' to navigate, and press enter to continue", box_x, box_y + 1, box_width, box_height);
                        noMessage = false;
                    }
                    if(_kbhit()){ //selected refers to which action box is selected
                        char c = _getch();
                        if(c == 'a' && selected > 0) selected--;
                        if(c == 'd' && selected < 2) selected++;
                        if(c == 13){
                            if(selected == 0){
                                shoot(0, playerDD);
                            } else if(selected == 1){
                                shootSelf(0, playerDD);
                                break;
                            } else if(selected == 2){
                                itemSelected = true;
                            }
                            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                        }
                        if(selected == 0){ // Highlight first box
                            draw_box_alt(action_box_width, action_box_height, action1_x, action_y); // Highlight box
                            draw_box(action_box_width, action_box_height, action2_x, action_y);
                            draw_box(action_box_width, action_box_height, action3_x, action_y);
                            print_in_box("SHOOT ENEMY", action1_x + 1, action_y, action_box_width, action_box_height);
                            print_in_box("SHOOT YOURSELF", action2_x, action_y, action_box_width, action_box_height);
                            print_in_box("ITEM", action3_x, action_y, action_box_width, action_box_height);
                            clearBox();
                            print_in_box("You chose to shoot your enemy!", box_x, box_y, box_width, box_height);
                            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                        } else if(selected == 1){ // Highlight second box
                            draw_box(action_box_width, action_box_height, action1_x, action_y);
                            draw_box_alt(action_box_width, action_box_height, action2_x, action_y); // Highlight box
                            draw_box(action_box_width, action_box_height, action3_x, action_y);
                            print_in_box("SHOOT ENEMY", action1_x + 1, action_y, action_box_width, action_box_height);
                            print_in_box("SHOOT YOURSELF", action2_x, action_y, action_box_width, action_box_height);
                            print_in_box("ITEM", action3_x, action_y, action_box_width, action_box_height);
                            clearBox();  
                            print_in_box("You chose to shoot yourself!", box_x, box_y, box_width, box_height);
                            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                        } else if(selected == 2){ // Highlight third box
                            draw_box(action_box_width, action_box_height, action2_x, action_y);
                            draw_box(action_box_width, action_box_height, action1_x, action_y);
                            draw_box_alt(action_box_width, action_box_height, action3_x, action_y); // Highlight box
                            print_in_box("SHOOT ENEMY", action1_x + 1, action_y, action_box_width, action_box_height);
                            print_in_box("SHOOT YOURSELF", action2_x, action_y, action_box_width, action_box_height);
                            print_in_box("ITEM", action3_x, action_y, action_box_width, action_box_height);
                            clearBox();
                            print_in_box("Item list:", box_x, box_y - 2, box_width, box_height);
                            print_in_box("- Heal item(s): " + std::to_string(healCount), box_x, box_y - 1, box_width, box_height);
                            print_in_box("- Magnifying Glass item(s): " + std::to_string(magniGlassCount), box_x, box_y, box_width, box_height);
                            print_in_box("- Double Damage item(s): " + std::to_string(doubleDamageCount), box_x, box_y + 1, box_width, box_height);
                            print_in_box("- Handcuff(s): " + std::to_string(handcuffCount), box_x, box_y + 2, box_width, box_height);
                            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                        }
                    }
                    if(itemSelected){
                        itemSelected = false;
                        clearBox();
                        print_in_box("press '1' to heal yourself", box_x - 22, box_y - 2, box_width, box_height);
                        print_in_box("press '2' to use Magnifying Glass (to see the bullet status this turn)", box_x, box_y - 1, box_width, box_height);
                        print_in_box("press '3' to deal Double Damage on this turn", box_x - 13, box_y, box_width, box_height);
                        print_in_box("press '4' to handcuff your enemy (make him skip his next turn)", box_x - 4, box_y + 1, box_width, box_height);
                        print_in_box("press esc key to go back", box_x - 23, box_y + 2, box_width, box_height);
                        bool inItemMenu = true;
                        while(inItemMenu){
                            if(_kbhit()){
                                char itemC = _getch();
                                if(itemC == '1' && healCount > 0){
                                    healCount--;
                                    if(player_life < 5) player_life++;
                                    updateLife();
                                    clearBox();
                                    print_in_box_animated("You used a Heal item and restored 1 life point.", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '1' && healCount == 0){
                                    clearBox();
                                    print_in_box_animated("You don't have enough heal item left", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                }
                                else if(itemC == '2' && magniGlassCount > 0){
                                    magniGlassCount--;
                                    clearBox();
                                    std::string message;
                                    if(bulletNow == 1){
                                        message = "Live";
                                    }else{
                                        message = "Blank";
                                    }
                                    print_in_box_animated("You used a Magnifying Glass item. The bullet this turn is: " + message, box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(4));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '2' && magniGlassCount == 0){
                                    clearBox();
                                    print_in_box_animated("You don't have enough magnifying glass left!", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '3' && doubleDamageCount > 0){
                                    doubleDamageCount--;
                                    playerDD = true;
                                    thisRoundDD = true;
                                    clearBox();
                                    print_in_box_animated("Your shot this turn will be double in damage", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '3' && doubleDamageCount == 0){
                                    clearBox();
                                    print_in_box_animated("You don't have enough double damage item left", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '4' && handcuffCount > 0){
                                    handcuffCount--;
                                    enemyHandcuffed = true;
                                    clearBox();
                                    print_in_box_animated("You handcuffed your enemy. He will skip his next turn.", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                } else if(itemC == '4' && handcuffCount == 0){
                                    clearBox();
                                    print_in_box_animated("You don't have enough handcuff left", box_x, box_y, box_width, box_height, 10);
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    inItemMenu = false;
                                    noMessage = true;
                                    break;
                                }else if(itemC = 27){ // 27 = enter ASCII
                                    clearBox();
                                    inItemMenu = false;
                                    noMessage = true;
                                }
                            }
                        }
                    }
                }
}


void askPlayer(){
    clearBox();
    print_in_box_animated("Wanna play again?", box_x, box_y, box_width, box_height, 10);
    print_in_box_animated("Press 'y' for yes or 'n' for no", box_x, box_y + 1, box_width, box_height, 10);
    while(true){
        if(_kbhit()){
            char c = _getch();
            if(c == 'y'){
                playAgain = true;
                roundIndex = 1;
                player_life = 5;
                enemy_life = 5;
                updateLife();
                clearBox();
                restartGame = true;
                break;
            } else if(c == 'n'){
                playAgain = false;
                clearBox();
                print_in_box_animated("Thank you for playing!", box_x, box_y, box_width, box_height, 10);
                set_cursor_position(0, action_y + action_box_height + 4);
                break;
            }
        }
    }
}


int main(){
    draw_box(box_width + 4, box_height + 23, box_x - 2, box_y - 18); //biggest box

    draw_box(box_width, box_height, box_x, box_y); //description box

    draw_box_alt(action_box_width, action_box_height, action1_x, action_y); //highlighted box

    draw_box(action_box_width, action_box_height, action2_x, action_y); //other action box

    draw_box(action_box_width, action_box_height, action3_x, action_y); //other action box

    print_in_box("SHOOT ENEMY", action1_x + 1, action_y, action_box_width, action_box_height);  //action box texts
    print_in_box("SHOOT YOURSELF", action2_x, action_y, action_box_width, action_box_height);
    print_in_box("ITEM", action3_x, action_y, action_box_width, action_box_height);
    updateLife();
    set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
    selected = 0;
    while(playAgain){
        if(roundIndex == 1){
            magniGlassCount = 0;
            healCount = 0;
            doubleDamageCount = 0;
            handcuffCount = 0;
            whoseTurn = 0;
        }
        for(int i = 0; i < 6; i++){
            barrel[i] = dist6(rng); // Shuffle the barrel (0 or 1)
        }
        if(player_life < 1){
            clearBox();
            print_in_box_animated("You lost the game!", box_x, box_y, box_width, box_height, 40);
            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor below the boxes
            std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
            askPlayer();
            continue;
            }
        if(enemy_life < 1){
            clearBox();
            print_in_box_animated("You won the game!", box_x, box_y, box_width, box_height, 40);
            set_cursor_position(0, action_y + action_box_height + 4); // Move cursor below the boxes
            std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
            askPlayer();
            continue;
            }
        randBarrel = barrel; // Create a copy to shuffle
        std::shuffle(randBarrel.begin(), randBarrel.end(), g);
        clearBox();



        // Start of the round, here we display the barrel and 3 random items

        print_in_box_animated("Round " + std::to_string(roundIndex), box_x, box_y, box_width, box_height);
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
        clearBox();
        print_in_box_animated("Spinning the barrel", box_x, box_y, box_width, box_height, 40);
        loading_dots();
        displayBarrel();
        std::this_thread::sleep_for(std::chrono::seconds(6)); // Wait for the player to see
        print_in_box_animated("                                       ", box_x, box_y, box_width, box_height, 10);
        print_in_box_animated("                            ", box_x, box_y + 1, box_width, box_height, 10);
        print_in_box_animated("Getting 3 random items", box_x, box_y, box_width, box_height, 40);
        loading_dots();
        print_in_box_animated("                                       ", box_x, box_y, box_width, box_height, 10);
        give_random_items();
        std::this_thread::sleep_for(std::chrono::seconds(6)); // Wait for the player to see
        clearBox();


        // For loop for each round(each round consists of 6 turns)
        for(int i = 0; i < 6; i++){
            thisRoundDD = false;
            playerDD = false;
            bulletNow = randBarrel[i];
            if(player_life < 1){ //Check if player lost or won before each round
                clearBox();
                print_in_box_animated("You lost the game!", box_x, box_y, box_width, box_height, 40);
                set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
                askPlayer();
                break;
            }
            if(enemy_life < 1){
                clearBox();
                print_in_box_animated("You won the game!", box_x, box_y, box_width, box_height, 40);
                set_cursor_position(0, action_y + action_box_height + 4); // Move cursor well below the boxes
                std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the player to see
                askPlayer();
                break;
            }
            if(whoseTurn == 1){ // Check if it's enemy's turn, else it's user's turn
                if(enemyHandcuffed){
                    clearBox();
                    print_in_box_animated("Your enemy is handcuffed and skips his turn!", box_x, box_y, box_width, box_height, 40);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    whoseTurn = 0;
                    enemyHandcuffed = false;
                    user_input();
                } else{
                    clearBox();
                    print_in_box_animated("It's enemy's turn", box_x, box_y, box_width, box_height, 40);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    int rand_item = rand() % 3;
                    if(rand_item == 0 && enemy_life < 5){
                        clearBox();
                        print_in_box_animated("Enemy heals himself", box_x, box_y, box_width, box_height, 40);
                        enemy_life += 1;
                        updateLife();
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    } else if (rand_item == 1){
                        clearBox();
                        print_in_box_animated("Enemy uses double damage", box_x, box_y, box_width, box_height, 40);
                        enemyDD = true;
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }
                    int rand_shoot = rand() % 2;
                    if(enemyDD){
                        shoot(1, enemyDD);
                        enemyDD = false;
                    } else if(rand_shoot == 0){
                        shoot(1, false);
                    } else{
                        shootSelf(1, false);
                    }
                }
            } else{
                user_input(); // user's turn
            }
        }



        if(restartGame == true){ //check if the player had chosen to restart
            restartGame = false;
            continue;
        }
        roundIndex++; // well, increase round index
    }
    return 0;
}
