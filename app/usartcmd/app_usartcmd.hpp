/**
 * @file app_usartcmd.cpp
 * @brief usartcmd 应用级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-8-29
 * @copyright
 * @par 日志:
 *   V1.0 针对串口接受控制命令解析
 *
 */

#ifndef __APP_USARTCMD_HPP__
#define __APP_USARTCMD_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "dep.hpp"
#include <string.h>
#include "usart/bsp_usart.hpp"
#include "memory_cpp/bsp_memory.hpp"

/* 通过移植 adurino serialdubgger 库实现 */
/* 类型定义 ------------------------------------------------------------------*/

// callback function pointer definiton
// 对串口回调函数类型声明
typedef void (*CommandCallback)(char *); //!< command callback function pointer

// UsartCMD verbose display to the user type
enum VerboseMode
{
	nothing = 0,  // display nothing - good for monitoring
	on_request,	  // display only on user request
	user_friendly // display textual messages to the user
};

/* 宏定义 --------------------------------------------------------------------*/

#define MAX_COMMAND_LENGTH 20 // 一帧数据的最大长度

// commander configuration
/* CMD_SCAN 输出所有回调函数 */
#define CMD_SCAN '?' //!< command scaning the network - only for commander
/* CMD_VERBOSE 控制回发变量，目前无用 */
#define CMD_VERBOSE '@' //!< command setting output mode - only for commander
#define CMD_DECIMAL '#' //!< command setting decimal places - only for commander

/* encoder 类函数声明 ----------------------------------------------------------------*/

/**
 * UsartCMD class implementing string communication protocol based on IDvalue (ex AB5.321 - command id `A`, sub-command id `B`,value `5.321`)
 *
 *  - This class can be used in combination with HardwareSerial instance which it would read and write
 *    or it can be used to parse strings that have been received from the user outside this library
 *  - UsartCMD class implements command protocol for few standard components of the SimpleFOC library
 *     - FOCMotor
 *     - PIDController
 *     - LowPassFilter
 *  - UsartCMD also provides a very simple command > callback interface that enables user to
 *    attach a callback function to certain command id - see function add()
 */
class UsartCMD
{
public:
	/**
	 * Default constructor receiving a serial interface that it uses to output the values to
	 * Also if the function run() is used it uses this serial instance to read the serial for user commands
	 *
	 * @param serial - Serial com port instance
	 * @param eol - the end of line sentinel character
	 * @param echo - echo last typed character (for command line feedback)
	 */
	UsartCMD(UART_HandleTypeDef *huart, char eol = '\n', bool echo = false);
	UsartCMD(char eol = '\n', bool echo = false);

	/**
	 * Function reading the serial port and firing callbacks that have been added to the UsartCMD
	 * once the user has requested them - when he sends the command
	 *
	 *  - It has default commands (the letters can be changed in the commands.h file)
	 *    '@' - Verbose mode
	 *    '#' - Number of decimal places
	 *    '?' - Scan command - displays all the labels of attached nodes
	 */
	void run();
	/**
	 * Function reading the string of user input and firing callbacks that have been added to the UsartCMD
	 * once the user has requested them - when he sends the command
	 *
	 *  - It has default commands (the letters can be changed in the commands.h file)
	 *    '@' - Verbose mode
	 *    '#' - Number of decimal places
	 *    '?' - Scan command - displays all the labels of attached nodes
	 *
	 * @param reader - temporary stream to read user input
	 * @param eol - temporary end of line sentinel
	 */
	void run(UART_HandleTypeDef *huart, char eol = '\n');
	/**
	 * Function reading the string of user input and firing callbacks that have been added to the UsartCMD
	 * once the user has requested them - when he sends the command
	 *
	 *  - It has default commands (the letters can be changed in the commands.h file)
	 *    '@' - Verbose mode
	 *    '#' - Number of decimal places
	 *    '?' - Scan command - displays all the labels of attached nodes
	 *
	 * @param user_input - string of user inputs
	 */
	void run(char *user_input);

	/**
	 *  Function adding a callback to the coomander withe the command id
	 * @param id         - char command letter
	 * @param onCommand  - function pointer void function(char*)
	 * @param label      - string label to be displayed when scan command sent
	 */
	void add(char id, CommandCallback onCommand, char *label = nullptr);

	// printing variables
	VerboseMode verbose = VerboseMode::user_friendly; //!< flag signaling that the commands should output user understanable text
	/* decimal_places 用于回发数据的小苏位数控制，这个功能全部集成在了 serialstudio 了 */
	// uint8_t decimal_places = 3;						  //!< number of decimal places to be used when displaying numbers

	// monitoring functions
	UART_HandleTypeDef *com_port = nullptr; //!< Serial terminal variable if provided
	// UART_HandleTypeDef* com_port; //!< Serial terminal variable if provided
	char eol = '\n';   //!< end of line sentinel character
	bool echo = false; //!< echo last typed character (for command line feedback)

	/**
	 * Float variable scalar command interface
	 *  - It only has one property - one float value
	 *  - It can be get by sending an empty string '\n'
	 *  - It can be set by sending 'value' - (ex. 0.01 for settin *value=0.01)
	 */
	void scalar(float *value, char *user_cmd);

private:
	// Subscribed command callback variables
	CommandCallback call_list[20]; //!< array of command callback pointers - 20 is an arbitrary number
	char call_ids[20];			   //!< added callback commands
	char *call_label[20];		   //!< added callback labels
	int call_count = 0;			   //!< number callbacks that are subscribed

	// helping variable for serial communication reading
	char received_chars[MAX_COMMAND_LENGTH] = {0}; //!< so far received user message - waiting for newline
	int rec_cnt = 0;							   //!< number of characters receives

	// // serial printing functions
	// /**
	//  *  print the string message only if verbose mode on
	//  *  @param message - message to be printed
	//  */
	// void printVerbose(const char *message);
	// /**
	//  *  Print the string message only if verbose mode on
	//  *  - Function handling the case for strings defined by F macro
	//  *  @param message - message to be printed
	//  */
	// void printVerbose(const __FlashStringHelper *message);
	// /**
	//  *  print the numbers to the serial with desired decimal point number
	//  *  @param message - number to be printed
	//  *  @param newline - if needs lewline (1) otherwise (0)
	//  */

	// void print(const float number);
	// void print(const int number);
	// void print(const char *message);
	// void print(const __FlashStringHelper *message);
	// void print(const char message);
	// void println(const float number);
	// void println(const int number);
	// void println(const char *message);
	// void println(const __FlashStringHelper *message);
	// void println(const char message);

	// void printError();
	bool isSentinel(char ch);
};

/* 外部声明变量 */
extern UsartCMD command_usart1;
void command_usart1_init(void);

#endif
