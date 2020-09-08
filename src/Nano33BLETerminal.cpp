#include "Nano33BLETerminal.h"
#include "Arduino.h"
#define TERMINAL_COMMANDS_LIST_SIZE         10
#define TERMINAL_READ_BUFFER_SIZE           30

class Nano33BLETerminalCommand
{
    public:
        const char* commandName;
        void (*commandFunction)(void);
};

static Nano33BLETerminalCommand commandList[TERMINAL_COMMANDS_LIST_SIZE];
static int commandsInUse = 0;

static char currentChar;
static char readBuffer[TERMINAL_READ_BUFFER_SIZE];
static int bufferCount = 0;

static const char* helpBanner = 
    "*******************************************************************************\r\n"
    "*                        Nano33BLETerminal Help Menu                          *\r\n"
    "*******************************************************************************\r\n"
    "Below is a list of registered commands:\r\n";

/**
 * @brief Initialises the terminal, registers the help and runtime-ms commands, 
 * and starts the Mbed OS Thread the terminal uses.
 */
void Nano33BLETerminal::begin(void)
{
    /* Make sure serial is initialised otherwise this ain't gonna work. */
    while(!Serial);
    Serial.println("Starting Nano33BLETerminal...");
    this->registerCommand("help", Nano33BLETerminal::helpCommand);
    this->registerCommand("runtime-ms", Nano33BLETerminal::runtimeCommand);
    this->terminalThread.start(mbed::callback(Nano33BLETerminal::pollFunction));
}

/**
 * @brief Registers a command with associated callback function that will be 
 * able to be executed using the terminal.
 * @param name String name of the command that will be typed in the terminal.
 * @param commandFunction Callback function associated with command name.
 */
void Nano33BLETerminal::registerCommand(const char* name, void (*commandFunction)(void))
{
    if(commandsInUse < TERMINAL_COMMANDS_LIST_SIZE)
    {
        commandList[commandsInUse].commandName = name;
        commandList[commandsInUse].commandFunction = commandFunction;
        commandsInUse++;
    }
    else
    {
        if(Serial)
        {
            Serial.printf(
                "Maximum number of commands already registered. Could not register %s!", 
                name);
        }
    }
};

/**
 * @brief Function that should be called periodically to process input.
 */
void Nano33BLETerminal::pollFunction(void)
{
    while(1)
    {
        /* Check if when input has been received */
        if(Serial.available() > 0) 
        {
            /* Read one of the available input characters */
            currentChar = Serial.read();
            /* Check for backspace */
            if(currentChar == '\b')
            {
                /* 
                 * If so delete the last character in the read buffer and 
                 * print out the line again.
                 */
                if(bufferCount > 0)
                {
                    bufferCount--;
                }
                readBuffer[bufferCount] = '\0';
                Serial.printf("\r%s", readBuffer);
            }
            /* Check for tab */
            else if(currentChar == '\t')
            {
                /* 
                 * If a character has already been entered, try predicting 
                 * the text 
                 */
                if(bufferCount > 0)
                {
                    /*
                     * This is a super simple text predictor, if it can be
                     * called that. It sees if one of the command names 
                     * has a similar substring as that of the string that
                     * is currently in the read buffer. If it does, the 
                     * first command name found with this attribute
                     * is placed in the buffer, and the buffer is printed
                     * out again. This will probably suit most peoples needs.
                     * To work perfectly, similar command names should be 
                     * avoided.
                     */
                    for(int ii = 0; ii < commandsInUse; ii++)
                    {
                        char* ret = strstr(commandList[ii].commandName, readBuffer); 
                        if(ret != NULL) 
                        {
                            memset(readBuffer, 0, 30);
                            memcpy(readBuffer, commandList[ii].commandName, strlen(commandList[ii].commandName));
                            bufferCount = strlen(readBuffer);
                            Serial.printf("\r%s", readBuffer);
                            break;
                        }
                    }
                }
            }
            /* Check for end of line*/
            else if(currentChar != '\r')
            {
                /* If it is not, just print the last character entered */
                readBuffer[bufferCount] = currentChar;
                if(bufferCount < 30)
                {
                    bufferCount++;
                }
                Serial.print(currentChar);
            }
            else
            {
                /* 
                 * If it is end of line, make sure the key pressed contains
                 * characters. If it does, go through each command name and 
                 * try and find a match with the current buffer. If a match
                 * exists, call the relavent callback function.
                 */
                if(bufferCount > 1)
                {
                    bool foundFlag = false;
                    for(int ii = 0; ii < commandsInUse; ii++)
                    {
                        if(!strcmp(readBuffer, commandList[ii].commandName))
                        {
                            commandList[ii].commandFunction();
                            foundFlag = true;
                            break;
                        }
                    }
    
                    /* If no command is found, let the user know */
                    if(!foundFlag )
                    {
                        Serial.printf("%s is not registered as a command!", readBuffer);             
                    }
                }
                Serial.println();
                memset(readBuffer, 0, 30);
                bufferCount = 0;
            }
        }
        delay(10);
    }
};

/** 
 * @brief Prints help banner and list of all registered command names.
 */
void Nano33BLETerminal::helpCommand(void)
{
    Serial.println();
    Serial.println(helpBanner);
    for(int ii = 0; ii < commandsInUse; ii++)
    {
        Serial.println(commandList[ii].commandName);
    }            
}

/** 
 * @brief Prints millis().
 */
void Nano33BLETerminal::runtimeCommand(void)
{
    Serial.printf("\r\n%lu\r\n", millis());
}


Nano33BLETerminal Terminal;
