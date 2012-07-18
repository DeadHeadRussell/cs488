#ifndef __LOGGING_H__
#define __LOGGING_H__

//#define DEBUG

#ifdef DEBUG

#include <iostream>

#define LOG(x) (std::cerr << x)
#define ERROR(x) (std::cerr << x << std::endl)
#define FUNC_ENTER (std::cerr << __FILE__ << ":" << __LINE__ << " - " \
                              << __PRETTY_FUNCTION__ << " Enter" << std::endl)
#define FUNC_LEAVE (std::cerr << __FILE__ << ":" << __LINE__ << " - " \
                              << __PRETTY_FUNCTION__ << " Leave" << std::endl)

#else

#define LOG(x)
#define ERROR(x)
#define FUNC_ENTER
#define FUNC_LEAVE

#endif

#endif

