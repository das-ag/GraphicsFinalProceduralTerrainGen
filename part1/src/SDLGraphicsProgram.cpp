#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "glm/vec2.hpp"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Agastya Das - Procedural Texture Generation",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                w,
                                h,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();


    // Setup our Renderer
    m_renderer = new Renderer(w,h);    
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    if(m_renderer!=nullptr){
        delete m_renderer;
    }


    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}



//Loops forever!
void SDLGraphicsProgram::Loop(){

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_openGLContext);
    ImGui_ImplOpenGL3_Init("#version 410");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    int terrainChunkSize = 512;
    std::vector<Terrain*> terrains;
    std::vector<SceneNode*> scenenodes;

    std::vector<glm::vec2> offsets = {
        glm::vec2(0,0),
        glm::vec2(1,0),
        glm::vec2(1,1),
        glm::vec2(0,1),
        glm::vec2(-1,1),
        glm::vec2(-1,0),
        glm::vec2(-1,-1),
        glm::vec2(0,-1),
        glm::vec2(1,-1)
    };

    for (int i = 0; i < offsets.size(); i++)
    {
        Terrain* t = new Terrain(terrainChunkSize, 0, offsets[i].x, offsets[i].y);
        terrains.push_back(t);
        t->LoadPerlinTexture();
        SceneNode* tn = new SceneNode(t);
        scenenodes.push_back(tn);
    }
     
    m_renderer->setRoot(scenenodes.at(0));
    
    
    for (int i = 1; i < offsets.size(); i++){
        scenenodes.at(0)->AddChild(scenenodes.at(i));
    }

    // Set a default position for our camera
    m_renderer->GetCamera(0)->SetCameraEyePosition(0.0f,100.0f,100.0f);

    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 5.0f;
    SDL_WarpMouseInWindow(m_window,m_renderer->m_screenWidth/2,m_renderer->m_screenHeight/2);

    // Get a pointer to the keyboard state
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);    
    // While application is running
    while(!quit){

        // For our terrain setup the identity transform each frame
        // TODO maybe move this
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        //Handle events on queue
        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
            }
            ImGui_ImplSDL2_ProcessEvent(&e);
            // Handle keyboard input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                m_renderer->GetCamera(0)->MouseLook(mouseX, mouseY);
            }
            if(keyboardState[SDL_SCANCODE_LEFT]){
                m_renderer->GetCamera(0)->MoveLeft(cameraSpeed);
            }else if(keyboardState[SDL_SCANCODE_RIGHT]){
                m_renderer->GetCamera(0)->MoveRight(cameraSpeed);
            }

            // Move forward or back
            if(keyboardState[SDL_SCANCODE_UP]){
                m_renderer->GetCamera(0)->MoveForward(cameraSpeed);
            }else if(keyboardState[SDL_SCANCODE_DOWN]){
                m_renderer->GetCamera(0)->MoveBackward(cameraSpeed);
            }

            // Move up or down
            if(keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT])   {
                m_renderer->GetCamera(0)->MoveUp(cameraSpeed);
            }else if(keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL]){
                m_renderer->GetCamera(0)->MoveDown(cameraSpeed);
            }
        } // End SDL_PollEvent loop.
		
        for (int i = 0; i < offsets.size(); i++){
            scenenodes.at(i)->GetLocalTransform().LoadIdentity();
            scenenodes.at(i)->GetLocalTransform().Translate(offsets[i].x * terrainChunkSize,0, offsets[i].y * terrainChunkSize);
        }

        // Update our scene through our renderer
        m_renderer->Update();
        // Render our scene using our selected renderer
        m_renderer->Render();

        ImGui::Begin("Demo window");
        ImGui::SliderInt("terrainChunkSize", &terrainChunkSize, 0, 512);
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Delay to slow things down just a bit!
        SDL_Delay(25);  // TODO: You can change this or implement a frame
                        // independent movement method if you like.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
