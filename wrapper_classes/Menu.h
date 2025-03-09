#include "mbed.h"
#include "C12832.h"
#include <vector>
#include <csetjmp>

#ifndef MENU_H
#define MENU_H

class Menu
{
    protected:
        C12832 &lcd;
        std::vector<std::pair<const char*, void (*)()>> menu;
        int selected_option = 1;
        bool option_selected = false;
        bool redraw_menu = true;
        bool exit_requested = false;

    public:
        Menu(C12832 &lcd)
            : lcd(lcd) {}

        void run() {
            while (true) {
                if (redraw_menu) {
                    drawMenu();
                }
                if (option_selected) {
                    runOption();
                }
                thread_sleep_for(100);
            }
        }

        void addMenuItem(const char* name, void (*function)()) {
            menu.emplace_back(name, function);
        }

        void drawMenu() {
            __disable_irq();
            lcd.cls();

            if (selected_option > 0) {
                lcd.locate(0,0);
                lcd.printf("  %s", menu[selected_option-1].first);
            }

            lcd.locate(0,10);
            lcd.printf("> %s", menu[selected_option].first);

            if (selected_option < menu.size() - 1) {
                lcd.locate(0,20);
                lcd.printf("  %s", menu[selected_option+1].first);
            }

            redraw_menu = false;
            __enable_irq();
        }

        void moveUp() {
            if (selected_option > 0) {
                selected_option--;
                redraw_menu = true;
            }
        }

        void moveDown() {
            if (option_selected) {
                exit_requested = true;
            }
            else if (selected_option < menu.size() - 1) {
                selected_option++;
                redraw_menu = true;
            }
        }
        
        void select() {
            option_selected = true;
        }

        void runOption() {
            lcd.cls();
            menu[selected_option].second();
            redraw_menu = true;
            option_selected = false;
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Returning to menu...");
            thread_sleep_for(1000);
        }

        bool const exitRequested() { return exit_requested; }

};

#endif