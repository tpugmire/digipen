//--------------------------------------------------------------------------------------
// File: MultiAnimation.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <Stdafx.h>
#include "DXUT\SDKmisc.h"
#include "DXUT\SDKsound.h"
//#include "imgui_impl_dx9.h"

using namespace std;

//#define OLD_TINY

#define TXFILE_FLOOR L"floor.jpg"
#define FLOOR_TILECOUNT 2


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
CFirstPersonCamera      g_Camera;               // A model viewing camera
CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls
ID3DXMesh*              g_pMeshFloor = NULL;    // floor geometry
D3DXMATRIXA16           g_mxFloor;              // floor world xform
D3DMATERIAL9            g_MatFloor;             // floor material
IDirect3DTexture9*      g_pTxFloor = NULL;      // floor texture
CSoundManager           g_DSound;               // DirectSound class
CMultiAnim              g_MultiAnim;            // the MultiAnim class for holding Tiny's mesh and frame hierarchy
vector< CTiny* >        g_v_pCharacters;        // array of character objects; each can be associated with any of the CMultiAnims
DWORD                   g_dwFollow = 0xffffffff;// which character the camera should follow; 0xffffffff for static camera
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
bool                    g_bPlaySounds = true;   // whether to play sounds
double                  g_fLastAnimTime = 0.0;  // Time for the animations

World*					g_pWorld = NULL;		// World object

bool paused = false;
//bool showBehaviorViewer = false;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_ADDTINY             5
#define IDC_NEXTVIEW            6
#define IDC_PREVVIEW            7
#define IDC_ENABLESOUND         8
#define IDC_CONTROLTINY         9
#define IDC_RELEASEALL          10
#define IDC_RESETCAMERA         11
#define IDC_RESETTIME           12
#define	IDC_PAUSE				13
#define	IDC_RESET				14
//#define	IDC_TOGGLEIMGUI			15

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
bool    CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext);
HRESULT CALLBACK OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void    CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
void    CALLBACK OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void    CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void    CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
void    CALLBACK OnLostDevice(void* pUserContext);
void    CALLBACK OnDestroyDevice(void* pUserContext);

void    InitApp();
HRESULT LoadMesh(IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh);
void    RenderText();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// get console
	AllocConsole();

#if _MSC_VER >= 1900	// VS2015
	freopen("CONOUT$", "w", stdout);
#else
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
#endif

#endif

	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXUTStaticWndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("ImGui Example"), _T("Fall 2016 CS380/CS580 Project 1: Behavior Trees"), WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL)
	{
		std::cout << GetLastError();
	}
	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{

		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 0;
	}
	static D3DPRESENT_PARAMETERS    g_d3dpp;
	static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
	g_DSound.Initialize(DXUTGetHWND(), DSSCL_PRIORITY);
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 0;
	}


	// Set the callback functions. These functions allow DXUT to notify
	// the application about device changes, user input, and windows messages.  The
	// callbacks are optional so you need only set callbacks for events you're interested
	// in. However, if you don't handle the device reset/lost callbacks then the sample
	// framework won't be able to reset your device since the application must first
	// release all device resources before resetting.  Likewise, if you don't handle the
	// device created/destroyed callbacks then DXUT won't be able to
	// recreate your device resources.
	DXUTSetCallbackD3D9DeviceAcceptable(IsDeviceAcceptable);
	DXUTSetCallbackD3D9DeviceCreated(OnCreateDevice);
	DXUTSetCallbackD3D9DeviceReset(OnResetDevice);
	DXUTSetCallbackD3D9DeviceLost(OnLostDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(OnDestroyDevice);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(KeyboardProc);
	DXUTSetCallbackD3D9FrameRender(OnFrameRender);
	DXUTSetCallbackFrameMove(OnFrameMove);

	// Show the cursor and clip it when in full screen
	DXUTSetCursorSettings(true, true);

	InitApp();

	// Initialize DXUT and create the desired Win32 window and Direct3D
	// device for the application. Calling each of these functions is optional, but they
	// allow you to set several options which control the behavior of the framework.
	DXUTInit(true, true); // Parse the command line, handle the default hotkeys, and show msgboxes


	// We need to set up DirectSound after we have a window.



	// Setup ImGui binding
	//ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);
	DXUTSetWindow(hwnd, hwnd, hwnd, false);

	DXUTSetD3D9Device(g_pd3dDevice);

	// Pass control to DXUT for handling the message pump and
	// dispatching render calls. DXUT will call your FrameMove
	// and FrameRender callback when there is idle time between handling window messages.
	DXUTMainLoop();

#ifdef OLD_TINY
	// Perform any application-level cleanup here. Direct3D device resources are released within the
	// appropriate callback functions and therefore don't require any cleanup code here.
	vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
	for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
	{
		( * itCurCP )->Cleanup();
		delete * itCurCP;
	}
	g_v_pCharacters.clear();
#endif

	delete g_apSoundsTiny[0];
	delete g_apSoundsTiny[1];
	//ImGui_ImplDX9_Shutdown();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClass(_T("ImGui Example"), wc.hInstance);
	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
	// Initialize dialogs
	g_SettingsDlg.Init(&g_DialogResourceManager);
	g_HUD.Init(&g_DialogResourceManager);
	g_SampleUI.Init(&g_DialogResourceManager);

	g_HUD.SetCallback(OnGUIEvent); int iY = 10;
	g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen (F5)", 35, iY, 125, 22, VK_F5);
	//g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
	g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2);
	//g_HUD.AddButton(IDC_TOGGLEIMGUI, L"Toggle ImGui (F8)", 35, iY += 24, 125, 22, VK_F8);
	g_HUD.AddButton(IDC_PAUSE, L"Pause (F9)", 35, iY += 24, 125, 22, VK_F9);
	g_HUD.AddButton(IDC_RESET, L"Reset (F10)", 35, iY += 24, 125, 22, VK_F10);

	g_SampleUI.SetCallback(OnGUIEvent); iY = 10;
	//g_SampleUI.AddButton( IDC_ADDTINY, L"Add Instance", 45, iY, 120, 24 );
	//g_SampleUI.AddButton( IDC_NEXTVIEW, L"(N)ext View", 45, iY += 26, 120, 24, L'N' );
	//g_SampleUI.AddButton( IDC_PREVVIEW, L"(P)revious View", 45, iY += 26, 120, 24, L'P' );
	//g_SampleUI.AddButton( IDC_RESETCAMERA, L"(R)eset view", 45, iY += 26, 120, 24, L'R' );
	//g_SampleUI.AddCheckBox( IDC_ENABLESOUND, L"Enable sound", 25, iY += 26, 140, 24, true );
	//g_SampleUI.AddCheckBox( IDC_CONTROLTINY, L"(C)ontrol this instance", 25, iY += 26, 140, 24, false, L'C' );
	//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( false );
	//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( false );
	//g_SampleUI.AddButton( IDC_RELEASEALL, L"Auto-control all", 45, iY += 26, 120, 24 );
	//g_SampleUI.AddButton( IDC_RESETTIME, L"Reset time", 45, iY += 26, 120, 24 );

	// Add mixed vp to the available vp choices in device settings dialog.
	DXUTGetD3D9Enumeration()->SetPossibleVertexProcessingList(true, false, false, true);

	// Create the World
	g_pWorld = new World();
	g_pWorld->InitializeSingletons();

	// Setup the camera with view matrix
	D3DXVECTOR3 vEye(.5f, .55f, -.4f);
	D3DXVECTOR3 vAt(.5f, -.125f, .5f);
	g_Camera.SetViewParams(&vEye, &vAt);
	g_Camera.SetScalers(0.01f, 1.0f);  // Camera movement parameters

}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
	D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat)))
		return false;

	// Need to support ps 1.1
	if (pCaps->PixelShaderVersion < D3DPS_VERSION(1, 1))
		return false;

	// Need to support A8R8G8B8 textures
	if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, 0,
		D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8)))
		return false;

	return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext)
{
	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
	// then switch to SWVP.
	if ((pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		pCaps->VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// If the hardware cannot do vertex blending, use software vertex processing.
	if (pCaps->MaxVertexBlendMatrices < 2)
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// If using hardware vertex processing, change to mixed vertex processing
	// so there is a fallback.
	if (pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

	// Debugging vertex shaders requires either REF or software vertex processing
	// and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
	if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
	{
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef DEBUG_PS
	pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		s_bFirstTime = false;
		if (pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF)
			DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
	}

	return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;


	V_RETURN(g_DialogResourceManager.OnD3D9CreateDevice(pd3dDevice));
	V_RETURN(g_SettingsDlg.OnD3D9CreateDevice(pd3dDevice));
	// Initialize the font
	V_RETURN(D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &g_pFont));

	// Initialize floor textures
	WCHAR str[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, TXFILE_FLOOR));
	V_RETURN(D3DXCreateTextureFromFile(pd3dDevice, str, &g_pTxFloor));

	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the
	// shader debugger. Debugging vertex shaders requires either REF or software vertex
	// processing, and debugging pixel shaders requires REF.  The
	// D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the
	// shader debugger.  It enables source level debugging, prevents instruction
	// reordering, prevents dead code elimination, and forces the compiler to compile
	// against the next higher available software target, which ensures that the
	// unoptimized shaders do not exceed the shader model limitations.  Setting these
	// flags will cause slower rendering since the shaders will be unoptimized and
	// forced into software.  See the DirectX documentation for more information about
	// using the shader debugger.
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	// Read the D3DX effect file
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"MultiAnimation.fx"));

	// If this fails, there should be debug output as to
	// they the .fx file failed to compile
	V_RETURN(D3DXCreateEffectFromFile(pd3dDevice, str, NULL, NULL, dwShaderFlags,
		NULL, &g_pEffect, NULL));
	g_pEffect->SetTechnique("RenderScene");

	D3DXMATRIX mx;
	// floor geometry transform
	D3DXMatrixRotationX(&g_mxFloor, -D3DX_PI / 2.0f);
	D3DXMatrixRotationY(&mx, D3DX_PI / 4.0f);
	D3DXMatrixMultiply(&g_mxFloor, &g_mxFloor, &mx);
	D3DXMatrixTranslation(&mx, 0.5f, 0.0f, 0.5f);
	D3DXMatrixMultiply(&g_mxFloor, &g_mxFloor, &mx);

	// set material for floor
	g_MatFloor.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, .75f);
	g_MatFloor.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_MatFloor.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	g_MatFloor.Emissive = D3DXCOLOR(.0f, 0.f, 0.f, 0.f);
	g_MatFloor.Power = 0.f;
	//ImGui_ImplDX9_CreateDeviceObjects();
	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice(IDirect3DDevice9* pd3dDevice,
	const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(g_DialogResourceManager.OnD3D9ResetDevice());
	V_RETURN(g_SettingsDlg.OnD3D9ResetDevice());

	// set up MultiAnim
	WCHAR sXFile[MAX_PATH];
	WCHAR str[MAX_PATH];

	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"MultiAnimation.fx"));
	V_RETURN(DXUTFindDXSDKMediaFileCch(sXFile, MAX_PATH, L"tiny_4anim.x"));

	CMultiAnimAllocateHierarchy AH;
	AH.SetMA(&g_MultiAnim);

	V_RETURN(g_MultiAnim.Setup(pd3dDevice, sXFile, str, &AH));

	// get device caps
	D3DCAPS9 caps;
	pd3dDevice->GetDeviceCaps(&caps);

	// Select the technique that fits the shader version.
	// We could have used ValidateTechnique()/GetNextValidTechnique() to find the
	// best one, but this is convenient for our purposes.
	if (caps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
		g_MultiAnim.SetTechnique("Skinning20");
	else
		g_MultiAnim.SetTechnique("Skinning11");

#ifdef OLD_TINY
	// Restore steps for tiny instances
	vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
	for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
	{
		( * itCurCP )->RestoreDeviceObjects( pd3dDevice );
	}
#endif

	g_pWorld->RestoreDeviceObjects(pd3dDevice);

	//Initialize world if needed
	g_pWorld->Initialize(&g_MultiAnim, &g_v_pCharacters, &g_DSound, DXUTGetGlobalTimer()->GetTime());



#ifdef OLD_TINY
	// If there is no instance, make sure we have at least one.
	if( g_v_pCharacters.size() == 0 )
	{
		CTiny * pTiny = new CTiny;
		if( pTiny == NULL )
			return E_OUTOFMEMORY;

		hr = pTiny->Setup( & g_MultiAnim, & g_v_pCharacters, & g_DSound, 0.f );
		pTiny->SetSounds( g_bPlaySounds );
	}
#endif

	if (g_pFont)
		V_RETURN(g_pFont->OnResetDevice());
	if (g_pEffect)
		V_RETURN(g_pEffect->OnResetDevice());
	ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
	if (pMAEffect)
	{
		pMAEffect->OnResetDevice();
		pMAEffect->Release();
	}

	// Create a sprite to help batch calls when drawing many lines of text
	V_RETURN(D3DXCreateSprite(pd3dDevice, &g_pTextSprite));

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 3, fAspectRatio, 0.001f, 100.0f);

	// set lighting
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// create the floor geometry
	LPD3DXMESH pMesh;
	V_RETURN(D3DXCreatePolygon(pd3dDevice, 1.2f, 4, &pMesh, NULL));
	V_RETURN(pMesh->CloneMeshFVF(D3DXMESH_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, pd3dDevice, &g_pMeshFloor));
	SAFE_RELEASE(pMesh);

	DWORD dwNumVx = g_pMeshFloor->GetNumVertices();

	struct Vx
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNorm;
		float fTex[2];
	};

	// Initialize its texture coordinates
	Vx * pVx;
	hr = g_pMeshFloor->LockVertexBuffer(0, (VOID **)& pVx);
	if (FAILED(hr))
		return hr;

	for (DWORD i = 0; i < dwNumVx; ++i)
	{
		if (fabs(pVx->vPos.x) < 0.01)
		{
			if (pVx->vPos.y > 0)
			{
				pVx->fTex[0] = 0.0f;
				pVx->fTex[1] = 0.0f;
			}
			else
				if (pVx->vPos.y < 0.0f)
				{
				pVx->fTex[0] = 1.0f * FLOOR_TILECOUNT;
				pVx->fTex[1] = 1.0f * FLOOR_TILECOUNT;
				}
				else
				{
					pVx->fTex[0] = 0.5f * FLOOR_TILECOUNT;
					pVx->fTex[1] = 0.5f * FLOOR_TILECOUNT;
				}
		}
		else
			if (pVx->vPos.x > 0.0f)
			{
			pVx->fTex[0] = 1.0f * FLOOR_TILECOUNT;
			pVx->fTex[1] = 0.0f;
			}
			else
			{
				pVx->fTex[0] = 0.0f;
				pVx->fTex[1] = 1.0f * FLOOR_TILECOUNT;
			}

		++pVx;
	}

	g_pMeshFloor->UnlockVertexBuffer();

	// reset the timer
	g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();;

	// Adjust the dialog parameters.
	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	g_SampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 270);
	g_SampleUI.SetSize(170, 220);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
#ifdef OLD_TINY
	vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
	for( itCur = g_v_pCharacters.begin(); itCur != itEnd; ++ itCur )
		( * itCur )->Animate( fTime - g_fLastAnimTime );
#endif

	double dt = fTime - g_fLastAnimTime;

	g_pWorld->Update(static_cast<float>(dt));
	g_pWorld->Animate(dt);

	g_fLastAnimTime = fTime;

	// Update the camera's position based on user input
	g_Camera.FrameMove(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	// If the settings dialog is being shown, then
	// render it instead of rendering the app's scene
	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.OnRender(fElapsedTime);
		return;
	}

	HRESULT hr;

	pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_ARGB(0, 0x3F, 0xAF, 0xFF), 1.0f, 0L);

	if (SUCCEEDED(pd3dDevice->BeginScene()))
	{
		//ImGui_ImplDX9_NewFrame();
		//GUIWindow::DrawWindows();
		/**/
		// set up the camera
		D3DXMATRIXA16 mx, mxView, mxProj;
		D3DXVECTOR3 vEye;
		D3DXVECTOR3 vLightDir;

		// are we following a tiny, or an independent arcball camera?
		if (g_dwFollow == 0xffffffff)
		{
			// Light direction is same as camera front (reversed)
			vLightDir = -(*g_Camera.GetWorldAhead());

			// set static transforms
			mxView = *g_Camera.GetViewMatrix();
			mxProj = *g_Camera.GetProjMatrix();
			V(pd3dDevice->SetTransform(D3DTS_VIEW, &mxView));
			V(pd3dDevice->SetTransform(D3DTS_PROJECTION, &mxProj));
			vEye = *g_Camera.GetEyePt();
		}
		else
		{
			// set follow transforms
			CTiny * pChar = g_v_pCharacters[g_dwFollow];

			D3DXVECTOR3 vCharPos = pChar->GetOwner()->GetBody().GetPos();
			D3DXVECTOR3 vCharFacing = pChar->GetOwner()->GetBody().GetDir();
			vEye = D3DXVECTOR3(vCharPos.x, 0.25f, vCharPos.z);
			D3DXVECTOR3 vAt(vCharPos.x, 0.0125f, vCharPos.z),
				vUp(0.0f, 1.0f, 0.0f);
			vCharFacing.x *= .25; vCharFacing.y = 0.f; vCharFacing.z *= .25;
			vEye -= vCharFacing;
			vAt += vCharFacing;

			D3DXMatrixLookAtLH(&mxView, &vEye, &vAt, &vUp);
			V(pd3dDevice->SetTransform(D3DTS_VIEW, &mxView));

			const D3DSURFACE_DESC *pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
			float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
			D3DXMatrixPerspectiveFovLH(&mxProj, D3DX_PI / 3, fAspectRatio, 0.1f, 100.0f);
			V(pd3dDevice->SetTransform(D3DTS_PROJECTION, &mxProj));

			// Set the light direction and normalize
			D3DXVec3Subtract(&vLightDir, &vEye, &vAt);
			D3DXVec3Normalize(&vLightDir, &vLightDir);
		}

		// set view-proj
		D3DXMatrixMultiply(&mx, &mxView, &mxProj);
		g_pEffect->SetMatrix("g_mViewProj", &mx);
		ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
		if (pMAEffect)
		{
			pMAEffect->SetMatrix("g_mViewProj", &mx);
		}

		// Set the light direction so that the
		// visible side is lit.
		D3DXVECTOR4 v(vLightDir.x, vLightDir.y, vLightDir.z, 1.0f);
		g_pEffect->SetVector("lhtDir", &v);
		if (pMAEffect)
			pMAEffect->SetVector("lhtDir", &v);

		SAFE_RELEASE(pMAEffect);

		// set the fixed function shader for drawing the floor
		V(pd3dDevice->SetFVF(g_pMeshFloor->GetFVF()));

		// Draw the floor
		V(g_pEffect->SetTexture("g_txScene", g_pTxFloor));
		V(g_pEffect->SetMatrix("g_mWorld", &g_mxFloor));
		UINT cPasses;
		V(g_pEffect->Begin(&cPasses, 0));
		for (UINT p = 0; p < cPasses; ++p)
		{
			V(g_pEffect->BeginPass(p));
			V(g_pMeshFloor->DrawSubset(0));
			V(g_pEffect->EndPass());
		}
		V(g_pEffect->End());


		g_pWorld->AdvanceTimeAndDraw(pd3dDevice, &mx, fElapsedTime, &vEye);

#ifdef OLD_TINY
		// draw each tiny
		vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
		for( itCur = g_v_pCharacters.begin(); itCur != itEnd; ++ itCur )
		{
			// set the time to update the hierarchy
			( * itCur )->AdvanceTime( fElapsedTime, & vEye );
			// draw the mesh
			( * itCur )->Draw();
		}
#endif

		//*****/
		//
		// Output text information
		//        
		//ImGui::Render();
		pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		RenderText();


		V(g_HUD.OnRender(fElapsedTime));
		V(g_SampleUI.OnRender(fElapsedTime));


		pd3dDevice->EndScene();
	}
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	// The helper object simply helps keep track of text position, and color
	// and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
	// If NULL is passed in as the sprite object, then it will work however the
	// pFont->DrawText() will not be batched together.  Batching calls will improves performance.
	CDXUTTextHelper txtHelper(g_pFont, g_pTextSprite, 15);
	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

	// Output statistics
	txtHelper.Begin();
	txtHelper.SetInsertionPos(5, 5);
	txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	//txtHelper.DrawTextLine( DXUTGetFrameStats() );
	//txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	// Dump out the FPS and device stats
	//txtHelper.SetInsertionPos( 5, 150 );
	//txtHelper.DrawFormattedTextLine( L"  Time: %2.3f", DXUTGetGlobalTimer()->GetTime() );
	//txtHelper.DrawFormattedTextLine( L"  Number of models: %d", g_v_pCharacters.size() );


	// Print out states
	txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	dbCompositionList list;
	g_database.ComposeList(list, OBJECT_Ignore_Type);
	int starttext = 5;
	int count = 0;
	dbCompositionList::iterator i;
	for (i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->HasStateMachine())
		{
			StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
			char* name = (*i)->GetName();
			char* statename = pStateMachine->GetCurrentStateNameString();
			char* substatename = pStateMachine->GetCurrentSubstateNameString();
			TCHAR* unicode_name = new TCHAR[strlen(name) + 1];
			TCHAR* unicode_statename = new TCHAR[strlen(statename) + 1];
			TCHAR* unicode_substatename = new TCHAR[strlen(substatename) + 1];
			mbstowcs(unicode_name, name, strlen(name) + 1);
			mbstowcs(unicode_statename, statename, strlen(statename) + 1);
			mbstowcs(unicode_substatename, substatename, strlen(substatename) + 1);
			if (substatename[0] != 0)
			{
				txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
				txtHelper.SetInsertionPos(5, starttext - 1 + (12 * count));
				txtHelper.DrawFormattedTextLine(L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename);
				txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				txtHelper.SetInsertionPos(4, starttext + (12 * count++));
				txtHelper.DrawFormattedTextLine(L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename);
			}
			else
			{
				txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
				txtHelper.SetInsertionPos(4, starttext - 1 + (12 * count));
				txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_statename);
				txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				txtHelper.SetInsertionPos(5, starttext + (12 * count++));
				txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_statename);
			}
			delete unicode_name;
			delete unicode_statename;
			delete unicode_substatename;
		}
		//else if ((*i)->HasBehaviorTree())
		//{
		//	char *name = (*i)->GetName();
		//	std::string BTPath = (*i)->m_behaviorTreeInfo.str();
		//	(*i)->m_behaviorTreeInfo.str("");
		//	TCHAR* unicode_name = new TCHAR[strlen(name) + 1];
		//	TCHAR* unicode_BTPath = new TCHAR[BTPath.length() + 1];
		//	mbstowcs(unicode_BTPath, BTPath.c_str(), BTPath.length() + 1);
		//	mbstowcs(unicode_name, name, strlen(name) + 1);
		//	if (BTPath.c_str()[0])
		//	{
		//		txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		//		txtHelper.SetInsertionPos(5, starttext - 1 + (12 * count));
		//		txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_BTPath);
		//		txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		//		txtHelper.SetInsertionPos(4, starttext + (12 * count++));
		//		txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_BTPath);
		//	}
		//	delete unicode_name;
		//	delete unicode_BTPath;
		//}
		else
		{
				char *name = (*i)->GetName();
				BT::AgentAbstractData *agent_userdata = g_trees.GetAgentData(name).GetAgentAbstractData();
				std::string runningleaves = agent_userdata->m_runningnodes;
				TCHAR* unicode_name = new TCHAR[strlen(name) + 1];
				TCHAR* unicode_runningleaves = new TCHAR[runningleaves.length() + 1];
				mbstowcs(unicode_runningleaves, runningleaves.c_str(), runningleaves.length() + 1);
				mbstowcs(unicode_name, name, strlen(name) + 1);
				if (runningleaves.c_str()[0])
				{
					txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
					txtHelper.SetInsertionPos(5, starttext - 1 + (12 * count));
					txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_runningleaves);
					txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					txtHelper.SetInsertionPos(4, starttext + (12 * count++));
					txtHelper.DrawFormattedTextLine(L"%s:   %s", unicode_name, unicode_runningleaves);
				}
				delete unicode_name;
				delete unicode_runningleaves;
		}
	}


	txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	txtHelper.SetInsertionPos(5, 70);

	// We can only display either the behavior text or help text,
	// with the help text having priority.
	if (g_bShowHelp)
	{
		// output data for T[m_dwFollow]
		if (g_dwFollow != -1)
		{
			//txtHelper.DrawTextLine( L"Press F1 to hide animation info\n"
			//                        L"Quit: Esc" );

			if (g_v_pCharacters[g_dwFollow]->IsUserControl())
			{
				txtHelper.SetInsertionPos(pd3dsdBackBuffer->Width - 150, 150);
				txtHelper.DrawTextLine(L"       Tiny control:\n"
					L"Move forward\n"
					L"Run forward\n"
					L"Turn\n");
				txtHelper.SetInsertionPos(pd3dsdBackBuffer->Width - 55, 150);
				txtHelper.DrawTextLine(L"\n"
					L"W\n"
					L"Shift W\n"
					L"A,D\n");
			}

			int i;
			CTiny * pChar = g_v_pCharacters[g_dwFollow];
			vector < String > v_sReport;
			pChar->Report(v_sReport);
			txtHelper.SetInsertionPos(5, pd3dsdBackBuffer->Height - 115);
			for (i = 0; i < 6; ++i)
			{
				txtHelper.DrawTextLine(v_sReport[i].c_str());
			}
			txtHelper.DrawTextLine(v_sReport[16].c_str());

			txtHelper.SetInsertionPos(210, pd3dsdBackBuffer->Height - 85);
			for (i = 6; i < 11; ++i)
			{
				txtHelper.DrawTextLine(v_sReport[i].c_str());
			}

			txtHelper.SetInsertionPos(370, pd3dsdBackBuffer->Height - 85);
			for (i = 11; i < 16; ++i)
			{
				txtHelper.DrawTextLine(v_sReport[i].c_str());
			}
		}
		else
		{
			//txtHelper.DrawTextLine( L"\n"
			//                        L"Quit: Esc" );
		}
	}
	else
	{
		//if( g_dwFollow != 0xffffffff )
		//    txtHelper.DrawTextLine( L"Press F1 to display animation info\n"
		//                            L"Quit: Esc" );
		//else
		//    txtHelper.DrawTextLine( L"\n"
		//                            L"Quit: Esc" );
	}

	txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	//ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam);
	//if (uMsg == WM_LBUTTONDOWN  && !ImGui::GetIO().WantCaptureMouse)
	if (uMsg == WM_LBUTTONDOWN)
	{
		IDirect3D9 *d3d = DXUTGetD3D9Object();
		IDirect3DDevice9 *dev = DXUTGetD3D9Device();
		if (d3d && dev)
		{
			D3DXVECTOR3 click2D;
			click2D.x = (float)LOWORD(lParam);
			click2D.y = (float)HIWORD(lParam);

			D3DVIEWPORT9 vp;
			dev->GetViewport(&vp);

			D3DXMATRIX world;
			D3DXMatrixIdentity(&world);

			D3DXMATRIX view;
			dev->GetTransform(D3DTS_VIEW, &view);

			D3DXMATRIX projection;
			dev->GetTransform(D3DTS_PROJECTION, &projection);

			D3DXVECTOR3 nearClick3D;
			D3DXVECTOR3 farClick3D;

			click2D.z = vp.MinZ;
			D3DXVec3Unproject(&nearClick3D, &click2D, &vp, &projection, &view, &world);

			click2D.z = vp.MaxZ;
			D3DXVec3Unproject(&farClick3D, &click2D, &vp, &projection, &view, &world);

			D3DXVECTOR3 ray = farClick3D - nearClick3D;
			D3DXVECTOR3 click3D;
			if (ray.y != 0.0f)
			{
				click3D = nearClick3D + (-nearClick3D.y / ray.y) * ray;
			}

			click3D.y = 0.0f;
			g_database.SendMsgFromSystem(MSG_MouseClick, MSG_Data(click3D));
		}
		return 0;
	}


	// Pass messages to camera class for camera movement if the
	// global camera if active
	if (-1 == g_dwFollow)
		g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);

	return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if (bKeyDown)
	{
		switch (nChar)
		{
		case VK_F1:
			g_bShowHelp = !g_bShowHelp;
			break;

			//        case VK_F10:
			//g_database.SendMsgFromSystem( MSG_Reset );
			//break;
			//        case VK_F9:
			//          DXUTPause(paused = !paused,false);
			//          break;
			//        case VK_F8:
			//          GUIWindow::SetActive("BehaviorViewer", showBehaviorViewer = !showBehaviorViewer);
			//          break;
		}
	}
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:        DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive(!g_SettingsDlg.IsActive()); break;
	case IDC_ADDTINY:
	{
		/*
					CTiny * pTiny = new CTiny;
					if( pTiny == NULL )
					break;

					if( SUCCEEDED( pTiny->Setup( & g_MultiAnim, & g_v_pCharacters, & g_DSound, DXUTGetGlobalTimer()->GetTime() ) ) )
					pTiny->SetSounds( g_bPlaySounds );
					else
					delete pTiny;
					break;
					*/
	}

	case IDC_NEXTVIEW:
		if (g_v_pCharacters.size() != 0)
		{
			if (g_dwFollow == 0xffffffff)
				g_dwFollow = 0;
			else if (g_dwFollow == (DWORD)g_v_pCharacters.size() - 1)
				g_dwFollow = 0xffffffff;
			else
				++g_dwFollow;

			if (g_dwFollow == 0xffffffff)
			{
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( false );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( false );
			}
			else
			{
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( true );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( true );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetChecked( g_v_pCharacters[g_dwFollow]->IsUserControl() );
			}
		}
		break;

	case IDC_PREVVIEW:
		if (g_v_pCharacters.size() != 0)
		{
			if (g_dwFollow == 0xffffffff)
				g_dwFollow = (DWORD)g_v_pCharacters.size() - 1;
			else if (g_dwFollow == 0)
				g_dwFollow = 0xffffffff;
			else
				--g_dwFollow;

			if (g_dwFollow == 0xffffffff)
			{
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( false );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( false );
			}
			else
			{
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( true );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( true );
				//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetChecked( g_v_pCharacters[g_dwFollow]->IsUserControl() );
			}
		}
		break;

	case IDC_RESETCAMERA:
		g_dwFollow = 0xffffffff;
		//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetEnabled( false );
		//g_SampleUI.GetCheckBox( IDC_CONTROLTINY )->SetVisible( false );
		break;

	case IDC_ENABLESOUND:
	{
		g_bPlaySounds = ((CDXUTCheckBox*)pControl)->GetChecked();
		vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
		for (itCur = g_v_pCharacters.begin(); itCur != itEnd; ++itCur)
			(*itCur)->SetSounds(g_bPlaySounds);
		break;
	}

	case IDC_CONTROLTINY:
		break;

	case IDC_RELEASEALL:
		break;

	case IDC_RESETTIME:
	{
		DXUTGetGlobalTimer()->Reset();
		g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();
		vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
		for (itCur = g_v_pCharacters.begin(); itCur != itEnd; ++itCur)
			(*itCur)->ResetTime();
		break;
	}

	//case IDC_TOGGLEIMGUI:
	//{
	//	GUIWindow::SetActive("BehaviorViewer", showBehaviorViewer = !showBehaviorViewer);
	//	break;
	//}

	case IDC_PAUSE:
	{
		DXUTPause(paused = !paused, false);
		break;
	}

	case IDC_RESET:
	{
		g_database.SendMsgFromSystem(MSG_Reset);
		break;
	}
	}
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D9LostDevice();
	g_SettingsDlg.OnD3D9LostDevice();
	if (g_pFont)
		g_pFont->OnLostDevice();
	if (g_pEffect)
		g_pEffect->OnLostDevice();
	ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
	if (pMAEffect)
	{
		pMAEffect->OnLostDevice();
		pMAEffect->Release();
	}

	SAFE_RELEASE(g_pTextSprite);
	SAFE_RELEASE(g_pMeshFloor);

	vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
	for (itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++itCurCP)
	{
		(*itCurCP)->InvalidateDeviceObjects();
		//ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	CMultiAnimAllocateHierarchy AH;
	AH.SetMA(&g_MultiAnim);
	g_MultiAnim.Cleanup(&AH);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice(void* pUserContext)
{
	//ImGui_ImplDX9_Shutdown();
	g_DialogResourceManager.OnD3D9DestroyDevice();
	g_SettingsDlg.OnD3D9DestroyDevice();
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pFont);

	SAFE_RELEASE(g_pTxFloor);
	SAFE_RELEASE(g_pMeshFloor);

	delete g_pWorld;

}
