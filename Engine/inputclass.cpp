////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;


	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;

	// Создание DirectInput
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Создание устройства клавиатуры
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Создание устройства мыши
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}
	GetMouseLocation(m_lastX,m_lastY);
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;


	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
	return;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool InputClass::MouseLeft()
{
	
	if(m_mouseX<m_lastX)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseLeftButton()
{
	
	if(m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseRightButton()
{
	
	if(m_mouseState.rgbButtons[1] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseRight()
{
	
	if(m_mouseX>m_lastX)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseUp()
{
	
	if(m_mouseY<m_lastY)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseDown()
{
	
	if(m_mouseY>m_lastY)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseWheelForward()
{
	
	if(m_mouseState.lZ>0)
	{
		return true;
	}

	return false;
}

bool InputClass::MouseWheelBackward()
{
	
	if(m_mouseState.lZ<0)
	{
		return true;
	}

	return false;
}

bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsAPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsPPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_P] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsSPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsPgUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsPgDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}