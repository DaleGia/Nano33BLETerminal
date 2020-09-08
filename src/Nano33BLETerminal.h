/*
  Nano33BLETerminal.h
  Copyright (c) 2020 Dale Giancono. All rights reserved..

  This class implements a simple terminal using serial. Commands with no 
  arguments can be called and used to trigger functions. Some basic 
  terminal functionality is implemented such as text suggestions when using
  the tab key and backspacing. Functions called using the callback 
  mechanism should not be exessively large, and shoult have very soft
  time constraints. 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*****************************************************************************/
/*INLCUDE GUARD                                                              */
/*****************************************************************************/
#ifndef NANO33BLETERMINAL_H_INCLUDED
#define NANO33BLETERMINAL_H_INCLUDED

/*****************************************************************************/
/*INLCUDES                                                                   */
/*****************************************************************************/
#include <Arduino.h>

/*****************************************************************************/
/*MACROS                                                                     */
/*****************************************************************************/
/**
 * Defines the default thread size that the terminal uses with Mbed OS.
 */
#define TERMINAL_DEFAULT_THREAD_SIZE    2048

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/

/*****************************************************************************/
/*CLASS DECLARATION                                                          */
/*****************************************************************************/
/**
 * @brief This class implements a simple terminal that allows the registering of 
 * simple commands with no arguments. These commands have attached callback 
 * functions that will be called as the commands are entered in the terminal.
 */
class Nano33BLETerminal
{
    public:
        Nano33BLETerminal(
            osPriority threadPriority = osPriorityNormal,
            uint32_t threadSize = TERMINAL_DEFAULT_THREAD_SIZE) : 
            terminalThread(
                threadPriority, 
                threadSize){};
        /**
         * @brief Initialises the terminal, registers the help and runtime-ms commands, 
         * and starts the Mbed OS Thread the terminal uses.
         */
        void begin(void);
        /**
         * @brief Registers a command with associated callback function that will be 
         * able to be executed using the terminal.
         */
        void registerCommand(const char* name, void (*commandFunction)(void));

    private:
        /** Thread the terminal operates with */
        rtos::Thread terminalThread;
        /**
         * @brief Function that should be called periodically to process input.
         */
        static void pollFunction(void);
        /** 
         * @brief Prints help banner and list of all registered command names.
         */
        static void helpCommand(void);
        /** 
         * @brief Prints millis().
         */
        static void runtimeCommand(void);
};


extern Nano33BLETerminal Terminal;
#endif /*NANO33BLETERMINAL_H_INCLUDED*/