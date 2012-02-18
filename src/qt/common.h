#ifndef __RETROPIA__QT__COMMON_H__
#define __RETROPIA__QT__COMMON_H__

#include <stdexcept>

#define R_STRINGIFY_(x) #x
#define R_STRINGIFY(x) R_STRINGIFY_(x)

#define R_RUNTIME_ERROR(void) { throw std::runtime_error("The application encountered a serious error (" __FILE__ ":" R_STRINGIFY(__LINE__) "). Please refer to the log for more information about this problem."); }

#endif /* __RETROPIA__QT__COMMON_H__ */
