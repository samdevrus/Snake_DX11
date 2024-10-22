////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "colorshaderclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
	struct Pos2D{
		float x,z;
	};
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput();
	bool RenderGraphics();
	bool MoveSnake();
	bool CheckHeadPosition();
	bool PutTarget();
	bool MoveSnakeTail();
	bool MoveSnakeTail(int ind);
private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;

	ModelClass* m_Target;
	ModelClass* m_Shead;
	ModelClass* m_STail;
	Pos2D* m_tailpos;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	ColorShaderClass* m_ColorShader;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	int dx,dy,m_snakec,m_tailmax,m_width,m_height,m_lifes,m_points;
	float m_speed;
	bool m_pause;
};

#endif