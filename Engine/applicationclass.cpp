////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
#include "math.h"

ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Target = 0;
	m_Shead = 0;
	m_STail = NULL;
	m_LightShader = 0;
	m_Light = 0;
	m_Camera = 0;
	m_Terrain = 0;
	m_ColorShader = 0;
	m_Timer = 0;
	m_Position = 0;
	m_FontShader = 0;
	m_Text = 0;
	m_pause=false;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	float rotX, rotY, rotZ;
	int i;
	D3DXMATRIX baseViewMatrix;

	//Инициализация переменных
	m_width=15;
	m_height=15;
	dx=1;
	dy=0;
	m_lifes=3;
	m_points=0;
	m_snakec=0;
	m_speed=0.1;
	m_tailmax=30;
	screenWidth-=10;
	screenHeight-=40;
	// Создание объекта DirectInput
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Создание объекта Direct3D.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Создание камеры.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Иниациализация камеры для 2D интерфейса.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Начальная позиция камеры.
	cameraX = m_width+10.0f;
	cameraY = m_height+0.0f;
	cameraZ = m_width/2;
	rotX = 43;
	rotY = 270;
	rotZ = 0;

	m_Camera->SetPosition(cameraX, cameraY, cameraZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Создание 3D объектов.
	//Цель
	m_Target = new ModelClass;
	if(!m_Target)
	{
		return false;
	}
	result = m_Target->Initialize(m_Direct3D->GetDevice(), "../Engine/data/target.txt", L"../Engine/data/2.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// Голова змеи.
	m_Shead = new ModelClass;
	if(!m_Shead)
	{
		return false;
	}
	result = m_Shead->Initialize(m_Direct3D->GetDevice(), "../Engine/data/Shead.txt", L"../Engine/data/123.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model2 object.", L"Error", MB_OK);
		return false;
	}
	m_Shead->SetPositionXYZ(5,0.5,5);
	//Хвост змеи
	m_STail = new ModelClass[m_tailmax];
	m_tailpos= new Pos2D[m_tailmax];
	if(!m_STail)
	{
		return false;
	}
	for (i=0;i<m_tailmax;i++) {
		result = (m_STail[i]).Initialize(m_Direct3D->GetDevice(), "../Engine/data/model.txt", L"../Engine/data/123.dds");
		if(!result){
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}
	}
	
	// Сетка.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Initialize(m_Direct3D->GetDevice(),m_width,m_height);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Создание шейдера для изменения цвета.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
	//Создание объекта позиции
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}
	//Копиование начальных позиций у камеры
	m_Position->SetPosition(cameraX, cameraY, cameraZ);
	m_Position->SetRotation(rotX, rotY, rotZ);
	
	// Создание шейдера для шрифта.
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Создание объекта текста.
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}
	// Созданеи шейдера для работы со светом.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Создание объекта света.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-2.0f, -15.0f,- 2.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(50.0f);
	PutTarget();

	return true;
}

void ApplicationClass::Shutdown()
{
	int i;
	// Удаление созданных объектов
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
	if(m_Target)
	{
		m_Target->Shutdown();
		delete m_Target;
		m_Target = 0;
	}
	if(m_STail)
	{
		for (i=0;i<m_tailmax;i++) 
		{
			m_STail[i].Shutdown();
		}
		delete[] m_STail;
		m_STail=0;
	}


	if(m_Shead)
	{
		m_Shead->Shutdown();
		delete m_Shead;
		m_Shead = 0;
	}
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	return;
}

bool ApplicationClass::Frame()
{
	bool result;


	// Чтение входных данных
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}
	//Проверка оставшихся жизней
	if (m_lifes==0) {
		m_Text->SetInformation("Game over. Press S to start new game, Esc to exit"+'\0',m_Direct3D->GetDeviceContext());
		m_pause=true;
			//return false;
	}
	else {

		m_Text->SetInformation("Press Esc to exit"+'\0',m_Direct3D->GetDeviceContext());	
	}

	// Обработка входных данных.
	result = HandleInput();
	if(!result)
	{
		return false;
	}
	//Вывод информации
	m_Text->SetHp(m_lifes,m_Direct3D->GetDeviceContext());
	m_Text->SetPoints(m_points,m_Direct3D->GetDeviceContext());
	
	//Перемещение змеи
	if (!m_pause) MoveSnake();
	
	// Вывод графики.
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}

bool ApplicationClass::HandleInput()
{
	bool keyDown, mouse_pos,result;
	float posX, posY, posZ, rotX, rotY, rotZ;
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);
	
	//Изменение позиции камеры
	mouse_pos=m_Input->MouseLeft() & m_Input->MouseLeftButton();
	if (mouse_pos) {
		rotY-=1;
	}

	mouse_pos=m_Input->MouseRight() & m_Input->MouseLeftButton();
	if (mouse_pos) {
		rotY+=1;
	}

	mouse_pos=m_Input->MouseUp() & m_Input->MouseLeftButton();
	if (mouse_pos) {
		posY+=0.1;
	}

	mouse_pos=m_Input->MouseDown() & m_Input->MouseLeftButton() ;
	if (mouse_pos & posY>1) {
		
		posY-=0.1;
	}

	mouse_pos=m_Input->MouseWheelForward();
	if (mouse_pos) {
		posX-=0.3;

	}
	mouse_pos=m_Input->MouseWheelBackward();
	if (mouse_pos) {
		posX+=0.3;
	}
	
	keyDown = m_Input->IsSPressed();
	if (keyDown) {
		if (m_lifes==0) {
			m_pause=false;
			m_lifes=5;
			m_points=0;
		}
	}
	
	keyDown = m_Input->IsPPressed();
	if (keyDown) {
		if (m_lifes>0)
		m_pause=!m_pause;
	}
	//Управление змеей
	keyDown = m_Input->IsUpPressed();
	if (keyDown) {
		dx = -1;
		if ( m_Shead->GetPosZ()!=(floor( m_Shead->GetPosZ() )+0.5))
			m_Shead->SetPositionXYZ(( m_Shead->GetPosX() ),m_Shead->GetPosY(),floor(m_Shead->GetPosZ())+0.5);
		dy = 0;
	}
	keyDown = m_Input->IsDownPressed();
	if (keyDown) {
		dx = +1;
		if ( m_Shead->GetPosZ()!=(floor( m_Shead->GetPosZ() )+0.5))
			m_Shead->SetPositionXYZ(( m_Shead->GetPosX() ),m_Shead->GetPosY(),floor(m_Shead->GetPosZ())+0.5);
		dy = 0;
	}
	keyDown = m_Input->IsRightPressed();
	if (keyDown) {
		dx = 0;
		dy = +1;
		if ( m_Shead->GetPosX()!=(floor( m_Shead->GetPosX() )+0.5))
			m_Shead->SetPositionXYZ(floor( m_Shead->GetPosX())+0.5,m_Shead->GetPosY(),m_Shead->GetPosZ());

	}
	keyDown = m_Input->IsLeftPressed();
	if (keyDown) {
		dx = 0;
		dy = -1;
		if ( m_Shead->GetPosX()!=(floor( m_Shead->GetPosX() )+0.5))
			m_Shead->SetPositionXYZ(floor( m_Shead->GetPosX())+0.5,m_Shead->GetPosY(),m_Shead->GetPosZ());
	}
	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	// Получение координат позиции

	//m_Position->GetPosition(posX, posY, posZ);
	//m_Position->GetRotation(rotX, rotY, rotZ);

	mouse_pos=m_Input->MouseLeft() & m_Input->MouseRightButton();
	if (mouse_pos) {
		posZ=posZ-0.1;
	}
	mouse_pos=m_Input->MouseRight() & m_Input->MouseRightButton();
	if (mouse_pos) {
		posZ=posZ+0.1;
	}
	

	m_Position->SetPosition(posX,posY,posZ);
	m_Position->SetRotation(rotX,rotY,rotZ);
	

	// Установка новой позиции камеры
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	int i;


	// Очистка сцены.
	m_Direct3D->BeginScene(0.7f, 0.7f, 0.7f, 1.0f);

	// Создание матрицы вида.
	m_Camera->Render();

	// Получение основных матриц.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Построение сетки.
	m_Terrain->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}
	//Построение цели
	m_Target->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Target->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
								   m_Target->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), 
								   m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	/*
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Target->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
	
							   m_Target->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	
	*/
	if(!result)
	{
		return false;
	}


	//Построение головы змеи
	m_Shead->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Shead->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
								   m_Shead->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), 
								   m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	
	
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shead->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}
	//Построение хвоста
	for (i=0;i<m_snakec;i++) {
		m_STail[i].Render(m_Direct3D->GetDeviceContext());
		//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_STail[i].GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
		result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_STail[i].GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
								   m_STail[i].GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), 
								   m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	
		if(!result)
		{
			return false;
		}
	}

	// Отключение Z буффера для отрисовки текста.
	m_Direct3D->TurnZBufferOff();
		
	// Включение прозрачности перед отрисовкой текста.
	m_Direct3D->TurnOnAlphaBlending();

	// Отрисовка текста.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	// Вывод сцены.
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::MoveSnake() {
	float x,z;
	//Вычисление новых координат
	x =( m_Shead->GetPosX()+m_speed*dx)  ;
	z =( m_Shead->GetPosZ()+m_speed*dy) ;
	//Проверка достижения цели
	if (((int)x==(int)m_Target->GetPosX() ) & ( (int)z==(int)m_Target->GetPosZ() ) ) {
		PutTarget();
		m_points+= (m_speed*100+ceil(abs((m_width/2)-x))+ceil(abs((m_height/2)-z)))*(m_snakec+1);
		m_speed+=0.01;
		m_snakec=(m_snakec+1)%m_tailmax;
	}
	//Проверка перехода в другую ячейку
	if (((int)x!=(int)m_Shead->GetPosX()) | 
		((int)z!=(int)m_Shead->GetPosZ()) ) 
			MoveSnakeTail();
	
	//Изменение позиции головы
	m_Shead->SetPositionXYZ(x,m_Shead->GetPosY(),z);	
	//Проверка позиции головы перед изменением буфера
	if ( CheckHeadPosition() ) {
		m_Shead->SetPosition(m_Direct3D->GetDevice());
	} else
	{
		
		m_snakec=0;
		m_lifes--;
		m_speed=0.1;
		PutTarget();
		m_Shead->SetPositionXYZ((m_width/2),m_Shead->GetPosY(),m_height/2);
	}
	return true;
}

bool ApplicationClass::PutTarget() {
	float x,z;
	//Добавление цели  в случаюную ячейку сетки
	x =( rand()%(m_width-1)) + 0.5;
	z =( rand()%(m_height-1)) + 0.5 ;
	if (((int)x==(int)m_Shead->GetPosX() ) & ( (int)z==(int)m_Shead->GetPosZ() ) ) {
		PutTarget();	
	
	} 
	else {
	if ( (x>=0.5) & (x<=m_width-1.5) & (z>=0.5) & (z<=m_height-1.5) ) {
		m_Target->SetPositionXYZ(x,m_Shead->GetPosY(),z);
		m_Target->SetPosition(m_Direct3D->GetDevice());
	}
	}
	return true;
}

bool ApplicationClass::MoveSnakeTail() {
	float x,z;
	int i;
	//Перемещение хвоста
	if (m_snakec>0) {
		m_tailpos[0].x=m_STail[0].GetPosX();
		m_tailpos[0].z=m_STail[0].GetPosZ();
		for (i=1;i<m_snakec;i++) {
			MoveSnakeTail(i);
		}
		m_STail[0].SetPositionXYZ(m_Shead->GetPosX(),m_Shead->GetPosY(),m_Shead->GetPosZ());
		m_STail[0].SetPosition(m_Direct3D->GetDevice());
		
		return true;
	}
}

bool ApplicationClass::MoveSnakeTail(int ind) {
	int i;
	if (ind>0) {
		m_tailpos[ind].x=m_STail[ind].GetPosX();
		m_tailpos[ind].z=m_STail[ind].GetPosZ();
		m_STail[ind].SetPositionXYZ(m_tailpos[ind-1].x,m_STail[ind-1].GetPosY(),m_tailpos[ind-1].z);
		m_STail[ind].SetPosition(m_Direct3D->GetDevice());
		return true;
	}
}

bool ApplicationClass::CheckHeadPosition() {
	int i;
	//Проверка позиций змеи
	if ( (m_Shead->GetPosX()>=0) & (m_Shead->GetPosX()<=m_width-1) &
		 (m_Shead->GetPosZ()>=0) & (m_Shead->GetPosZ()<=m_height-1) ) {
		if (m_snakec>1)
		for (i=1;i<m_snakec;i++) {
			if (((int)m_STail[i].GetPosX()==(int)m_Shead->GetPosX() ) & 
				((int)m_STail[i].GetPosZ()==(int)m_Shead->GetPosZ() ) ) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}

}