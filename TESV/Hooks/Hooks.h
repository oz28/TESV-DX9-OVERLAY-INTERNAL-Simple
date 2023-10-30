#pragma once


extern int ix;
extern DWORD EntlistJmpBack;
extern DWORD EntityObjStart;
extern bool alreadyThere;

extern playerent* ents[255];
extern playerent* entsptr;

__declspec() void entityhook();