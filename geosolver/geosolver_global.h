#ifndef GEOSOLVER_GLOBAL_H
#define GEOSOLVER_GLOBAL_H

#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>

using r32 = float;
using r64 = double;
using s32 = int;
using s64 = long int;
using u32 = unsigned int;
using u64 = unsigned long int;
using s16 = short;
using u16 = unsigned short;
using s8  = char;
using u8  = unsigned char;

#if 0
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#else
#define ANSI_COLOR_RED       
#define ANSI_COLOR_GREEN     
#define ANSI_COLOR_YELLOW    
#define ANSI_COLOR_BLUE      
#define ANSI_COLOR_MAGENTA   
#define ANSI_COLOR_CYAN      
#define ANSI_COLOR_RESET     
#endif

#define THROW(msg) \
            std::stringstream ss;                                           \
            ss << "[" ANSI_COLOR_GREEN;                                     \
            ss << __FUNCTION__;                                             \
            ss << ANSI_COLOR_RESET "(";                                     \
            ss << __FILE__;                                                 \
            ss << ":";                                                      \
            ss << __LINE__;                                                 \
            ss << ")]: ";                                                   \
            ss << ANSI_COLOR_RED " ";                                           \
            ss << msg;                                                      \
            ss << ANSI_COLOR_RESET " ";                                         \
            throw std::runtime_error(ss.str());

#endif // GEOSOLVER_GLOBAL_H
