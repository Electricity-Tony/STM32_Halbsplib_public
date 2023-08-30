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

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_usartcmd.hpp"

/* 私有类型定义 --------------------------------------------------------------*/

////******************************************* UsartCMD 编码器类*************************************************************************////
/**
 * @brief  UsartCMD 编码器类
 * @param [in]   counter_period	完整一圈的计数值
 * @param [in]	get_count_link	获取计数值连接函数
 *
 */
WEAK UsartCMD::UsartCMD(UART_HandleTypeDef *huart, char eol, bool echo)
{
	com_port = huart;
	this->eol = eol;
	this->echo = echo;
}
UsartCMD::UsartCMD(char eol, bool echo)
{
	this->eol = eol;
	this->echo = echo;
}

/**
 * @brief  add 添加命令函数
 * @details 加入回调函数到 UsartCMD
 * @param [in]   id	命令字符
 * @param [in]	onCommand 回调函数名
 * @param [in]	label 对应的字符串名称
 * @retval
 */
void UsartCMD::add(char id, CommandCallback onCommand, char *label)
{
	call_list[call_count] = onCommand;
	call_ids[call_count] = id;
	call_label[call_count] = label;
	call_count++;
}

/**
 * @brief  run 运行函数
 * @details 串口有声明通道进行下一步
 * @param [in]
 * @retval
 */
void UsartCMD::run()
{
	if (com_port == nullptr)
		return;
	run(com_port, eol);
}

/**
 * @brief  run 运行函数
 * @details 接串口fifo中转存，进入后续解析
 * @param [in] 串口通道
 * @param [in] 尾帧
 * @retval
 */
void UsartCMD::run(UART_HandleTypeDef *huart, char eol)
{
	UART_HandleTypeDef *tmp = com_port; // save the serial instance
	char eol_tmp = this->eol;
	this->eol = eol;
	com_port = huart;

	// a string to hold incoming data
	while (RxDebug_fifo.check_state() != FIFO_EMPTY)
	{
		// get the new byte:
		uint8_t ch;
		RxDebug_fifo.pop(&ch);
		received_chars[rec_cnt++] = (char)ch;
		// end of user input
		if (echo)
			printf("%c", (char)ch);
		if (isSentinel(ch))
		{
			// execute the user command
			run(received_chars);

			// reset the command buffer
			received_chars[0] = 0;
			rec_cnt = 0;
		}
		if (rec_cnt >= MAX_COMMAND_LENGTH)
		{ // prevent buffer overrun if message is too long
			received_chars[0] = 0;
			rec_cnt = 0;
		}
	}

	com_port = tmp; // reset the instance to the internal value
	this->eol = eol_tmp;
}

/**
 * @brief  run 运行函数
 * @details 接受内容解析
 * @param [in] 转存后的数据地址
 * @retval
 */
void UsartCMD::run(char *user_input)
{
	// execute the user command
	char id = user_input[0];
	switch (id)
	{
	case CMD_SCAN:
		for (int i = 0; i < call_count; i++)
		{
			printf("%c", call_ids[i]);
			printf(":");
			if (call_label[i])
				printf("%s\r\n", call_label[i]);
			else
				printf("\r\n");
		}
		break;
	case CMD_VERBOSE:
		/* 控制回发的变量，对于 stm32 没什么用 */
		// if (!isSentinel(user_input[1]))
		// 	verbose = (VerboseMode)atoi(&user_input[1]);
		// printVerbose(F("Verb:"));
		// switch (verbose)
		// {
		// case VerboseMode::nothing:
		// 	println(F("off!"));
		// 	break;
		// case VerboseMode::on_request:
		// case VerboseMode::user_friendly:
		// 	println(F("on!"));
		// 	break;
		// }
		break;
	case CMD_DECIMAL:
		/* 控制数据采样的小数位数，这部分功能使用 serialstudio 了，对于 stm32 没什么用 */
		// if (!isSentinel(user_input[1]))
		// 	decimal_places = atoi(&user_input[1]);
		// printVerbose(F("Decimal:"));
		// println(decimal_places);
		// break;
	default:
		for (int i = 0; i < call_count; i++)
		{
			if (id == call_ids[i])
			{
				call_list[i](&user_input[1]);
				break;
			}
		}
		break;
	}
}

/**
 * @brief  scalar 浮点型转换
 * @details 解析提取字符串数据为浮点
 * @param [in] value：放置的浮点数据地址
 * @param [in] user_cmd：接受转存后的数据地址
 * @retval
 */
void UsartCMD::scalar(float *value, char *user_cmd)
{
	bool GET = isSentinel(user_cmd[0]);
	if (!GET)
		*value = atof(user_cmd);
	//   println(*value);
	printf("%.*s\n", rec_cnt, received_chars);
}

/**
 * @brief  isSentinel 尾帧判断
 * @details 对是否检测到尾帧进行判断
 * @param [in] ch：需要检测的字符
 * @retval
 */
bool UsartCMD::isSentinel(char ch)
{
	if (ch == eol)
		return true;
	else if (ch == '\r')
	{
		//   printVerbose(F("Warn: \\r detected! \n"));
		printf("Warn: \\r detected! \n");
	}
	return false;
}

// void Commander::print(const int number){
//   if( !com_port || verbose == VerboseMode::nothing ) return;
//   com_port->print(number);
// }
// void Commander::print(const float number){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->print((float)number,(int)decimal_places);
// }
// void Commander::print(const char* message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->print(message);
// }
// void Commander::print(const __FlashStringHelper *message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->print(message);
// }
// void Commander::print(const char message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->print(message);
// }

// void Commander::println(const int number){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->println(number);
// }
// void Commander::println(const float number){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->println((float)number, (int)decimal_places);
// }
// void Commander::println(const char* message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->println(message);
// }
// void Commander::println(const __FlashStringHelper *message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->println(message);
// }
// void Commander::println(const char message){
//   if(!com_port || verbose == VerboseMode::nothing ) return;
//   com_port->println(message);
// }

// void Commander::printVerbose(const char* message){
//   if(verbose == VerboseMode::user_friendly) print(message);
// }
// void Commander::printVerbose(const __FlashStringHelper *message){
//   if(verbose == VerboseMode::user_friendly) print(message);
// }
// void Commander::printError(){
//  println(F("err"));
// }

/* ***************************以下为声明 UsartCMD 变量****************************** */
UsartCMD command_usart1 = UsartCMD(fusart_Debug);

/* ***************************以下为自写回调功能函数****************************** */
/* callback code start */
/**
 * @brief  打印接受到的数据
 * @details
 * @param ID：A
 * @param cmd：数据地址
 * @retval
 */
void doDebugValue(char *cmd)
{
	static float debugvalue;
	command_usart1.scalar(&debugvalue, cmd);
	printf("%f\n", debugvalue);
}

/**
 * @brief  UsartCMD初始化
 * @details 加入回调函数到 UsartCMD
 * @param ID：A
 * @param function：回调函数名
 * @param label：回调函数标签
 * @retval
 */
void command_usart1_init(void)
{
	command_usart1.add('A', doDebugValue, (char *)"doDebugValue");
}

/* callback code end */
