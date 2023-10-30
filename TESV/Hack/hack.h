#pragma once

#include <Psapi.h>



extern bool nameesp;
extern bool boneesp;


MODULEINFO GetModuleInfo(char* szModule);
void BoneESP();
void DrawEntNames();
void InitHack();