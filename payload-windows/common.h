/*
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Program Tracing and Instrumentation Toolkit.

The Initial Developer of the Original Code is Mozilla Corporation.

Original Author: Kevin Gadd (kevin.gadd@gmail.com)
*/

#ifdef PAYLOAD_IMPL
    #define PAYLOAD_EXPORT_2(return_type, callconv) extern "C" __declspec(dllexport) return_type callconv
#else
    #define PAYLOAD_EXPORT_2(return_type, callconv) extern "C" __declspec(dllimport) return_type callconv
#endif

#define PAYLOAD_EXPORT(return_type) PAYLOAD_EXPORT_2(return_type, __stdcall)
