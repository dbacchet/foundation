#pragma once
// /////////////////////////////////////////////////////////////////////// //
// Copyright (c) 2013, Davide Bacchet (davide.bacchet@gmail.com)           //
// All rights reserved.                                                    //
//                                                                         //
// The software is released under the Creative Commons Attribution         //
// NonCommercial (CC BY-NC) license. The full license text is available    //
// at http://creativecommons.org/licenses/by-nc/3.0/legalcode              //
//                                                                         //
// You are free to distribute and adapt the work under the conditions:     //
// * Redistributions of source code must retain the above copyright notice //
// * You must attribute the work in the manner specified by the author     //
// * You may not use this work for commercial purposes.                    //
//                                                                         //
// Commercial Usage:                                                       //
// To use this software as part of a commercial product, you must          //
// obtain a written agreement from the author.                             //
// /////////////////////////////////////////////////////////////////////// //

// logging 

#ifdef __cplusplus
extern "C" {
#endif

// log calls
void logTrace(const char* logline, ...);
void logDebug(const char* logline, ...);
void logInfo(const char* logline, ...);
void logMessage(const char* logline, ...);
void logWarning(const char* logline, ...);
void logError(const char* logline, ...);
    



#ifdef __cplusplus
}
#endif
