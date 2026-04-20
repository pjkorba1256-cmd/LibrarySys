#pragma once
#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0600
#endif
#ifndef IPV6_V6ONLY
  #define IPV6_V6ONLY 27
#endif
#include <strings.h>
#include "LibraryService.h"
#include "httplib.h"

void registerTransactionRoutes(httplib::Server& svr, LibraryService& service);
