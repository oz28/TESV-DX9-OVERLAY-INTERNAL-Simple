#include "../Draw/hDraw.h"
#include "../Structs/Structs.h"
#include <Psapi.h>
#include <iostream>
#include <string>
#include "../Hooks/Hooks.h"
#include "../Offsets/Offsets.h"


using namespace std;

ID3DXFont* dx_Font = 0;

char* entityName;
char* localPlayerName;
float Matrix[16];
int rwidth, rhight;
int windowWidth, windowHeight;

bool nameesp = false;
bool boneesp = false; 


Vector3 LocalPlayerCoords;
string playerName = "carlo";

playerent* myents[];

static Vector3 green;
static Vector3 red;

void InitHack(){ 
	green.x = 0;
	green.y = 255;
	green.z = 0;

	red.x = 255;
	red.y = 0;
	red.z = 0;

}


MODULEINFO GetModuleInfo(char* szModule) {
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}



MODULEINFO modInfo = GetModuleInfo((char*)"TESV.exe");
unsigned long Gameresolution = (unsigned long)modInfo.lpBaseOfDll + offsets::dwGameResInfo;


bool WorldToScreen(Vector3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight) {
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords 
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates 
	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

void DrawNumber(Vector3 Bone, char* Bonename, float Matrix[16], int BoneColorR, int BoneColorG, int BoneColorB)
{
	vec2 vScreenBone;
	if (WorldToScreen(Bone, vScreenBone, Matrix, rwidth, rhight))
	{
		DrawString(Bonename, vScreenBone.x, vScreenBone.y, 255, 255, 0, 0);
	}
}

void DrawBone(Vector3 Bone1, Vector3 Bone2, float Matrix[16], Vector3 color)
{
	vec2 vScreenBone1;
	vec2 vScreenBone2;
	if (WorldToScreen(Bone1, vScreenBone1, Matrix, rwidth, rhight))
	{
		if (WorldToScreen(Bone2, vScreenBone2, Matrix, rwidth, rhight))
		{
			DrawLine(vScreenBone1.x, vScreenBone1.y + 20, vScreenBone2.x, vScreenBone2.y + 20, color.x, color.y, color.z, 255);
		}
	}
}


void BoneESP() {

		std::string isValid;
		std::string validationName = "NPC COM [COM ]";

		memcpy(&rwidth, (PBYTE*)Gameresolution, sizeof(rwidth));
		memcpy(&rhight, (PBYTE*)(Gameresolution + 0x4), sizeof(rhight));
		memcpy(&Matrix, (PBYTE*)0x01B3EA10, sizeof(Matrix));

		for (int i = 0; i < 254; i++)
		{
			if (ents[i] != 0)
			{
				isValid = ents[i]->boneptr1->boneptr2->boneptr3->humanoidvalidation;
				if (isValid == validationName)
				{
					//Head
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b26, ents[i]->boneptr1->boneptr2->boneptr3->b25, Matrix, green);

					//Shoulder R
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b25, ents[i]->boneptr1->boneptr2->boneptr3->b50, Matrix, green);
					//Shoulder L
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b25, ents[i]->boneptr1->boneptr2->boneptr3->b54, Matrix, green);

					// Leftarm
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b54, ents[i]->boneptr1->boneptr2->boneptr3->b55, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b55, ents[i]->boneptr1->boneptr2->boneptr3->b56, Matrix, green);

					//rightarm
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b50, ents[i]->boneptr1->boneptr2->boneptr3->b51, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b51, ents[i]->boneptr1->boneptr2->boneptr3->b32, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b31, ents[i]->boneptr1->boneptr2->boneptr3->b49, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b49, ents[i]->boneptr1->boneptr2->boneptr3->b48, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b48, ents[i]->boneptr1->boneptr2->boneptr3->b32, Matrix, green);

					//righthand
					//thumb
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b32, ents[i]->boneptr1->boneptr2->boneptr3->b33, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b33, ents[i]->boneptr1->boneptr2->boneptr3->b34, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b34, ents[i]->boneptr1->boneptr2->boneptr3->b35, Matrix, green);

					//pointerfing
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b32, ents[i]->boneptr1->boneptr2->boneptr3->b36, Matrix,green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b36, ents[i]->boneptr1->boneptr2->boneptr3->b37, Matrix,green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b37, ents[i]->boneptr1->boneptr2->boneptr3->b38, Matrix,green);

					//middle finger
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b32, ents[i]->boneptr1->boneptr2->boneptr3->b39, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b39, ents[i]->boneptr1->boneptr2->boneptr3->b40, Matrix, green);

					//little fing
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b32, ents[i]->boneptr1->boneptr2->boneptr3->b44, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b44, ents[i]->boneptr1->boneptr2->boneptr3->b45, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b45, ents[i]->boneptr1->boneptr2->boneptr3->b46, Matrix, green);

					//spine
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b25, ents[i]->boneptr1->boneptr2->boneptr3->b24, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b24, ents[i]->boneptr1->boneptr2->boneptr3->b23, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b23, ents[i]->boneptr1->boneptr2->boneptr3->b22, Matrix, green);

					//rleg
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b22, ents[i]->boneptr1->boneptr2->boneptr3->b6, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b6, ents[i]->boneptr1->boneptr2->boneptr3->b7, Matrix,  green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b7, ents[i]->boneptr1->boneptr2->boneptr3->b8, Matrix,  green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b8, ents[i]->boneptr1->boneptr2->boneptr3->b9, Matrix,  green);

					//lleg
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b22, ents[i]->boneptr1->boneptr2->boneptr3->b2, Matrix, green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b2, ents[i]->boneptr1->boneptr2->boneptr3->b3, Matrix,  green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b3, ents[i]->boneptr1->boneptr2->boneptr3->b4, Matrix,  green);
					DrawBone(ents[i]->boneptr1->boneptr2->boneptr3->b4, ents[i]->boneptr1->boneptr2->boneptr3->b5, Matrix,  green);





				}

			}
		}
}

void DrawEntNames() {

	std::string isValid;
	std::string validationName = "NPC COM [COM ]";

	memcpy(&rwidth, (PBYTE*)Gameresolution, sizeof(rwidth));
	memcpy(&rhight, (PBYTE*)(Gameresolution + 0x4), sizeof(rhight));
	memcpy(&Matrix, (PBYTE*)offsets::dwViewMatrix, sizeof(Matrix));

	for (int i = 0; i < 254; i++)
	{
		if (ents[i] != 0)
		{
			vec2 vScreen;

			if (WorldToScreen(ents[i]->coords, vScreen, Matrix, rwidth, rhight))
			{

				if (ents[i] != 0 && ents[i]->nameptr->name != "carlo") {
					entityName = ents[i]->nameptr->name;

					if (entityName == "carlo") {
						LocalPlayerCoords = ents[i]->coords;

					}

					DrawLine(vScreen.x, vScreen.y, windowWidth / 2, windowHeight / 2 + 400, 255, 0, 0, 255);
					DrawString((char*)entityName, vScreen.x, vScreen.y, 255, 0, 0, dx_Font);

				}
			}
		}
	}
}