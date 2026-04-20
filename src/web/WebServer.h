#pragma once
#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
  #endif
  #ifndef IPV6_V6ONLY
    #define IPV6_V6ONLY 27
  #endif
  #include <strings.h>
#endif
#include "LibraryService.h"
#include "SearchService.h"
#include "httplib.h"

// ============================================================
//  WebServer — sets up all REST routes and starts listening.
//  This is the ONLY layer that knows about HTTP.
// ============================================================
void startServer(LibraryService& libraryService,
                 SearchService&  searchService,
                 int port = 8080);
