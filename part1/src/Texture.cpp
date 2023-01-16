#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif


#include "Texture.hpp"

#include <stdio.h>
#include <string.h>

#include <fstream>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <memory>

// Default Constructor
Texture::Texture(){

}


// Default Destructor
Texture::~Texture(){
	// Delete our texture from the GPU
	glDeleteTextures(1,&m_textureID);

    // Delete our image
    if(m_image != nullptr){
        delete m_image;
    }

}

void Texture::LoadPerlinTexture(unsigned int m_chunkSize, uint8_t* m_noiseData){

    glEnable(GL_TEXTURE_2D); 
	// Generate a buffer for our texture
    glGenTextures(1,&m_textureID);
    // Similar to our vertex buffers, we now 'select'
    // a texture we want to bind to.
    // Note the type of data is 'GL_TEXTURE_2D'
    glBindTexture(GL_TEXTURE_2D, m_textureID);
	// Now we are going to setup some information about
	// our textures.
	// There are four parameters that must be set.
	// GL_TEXTURE_MIN_FILTER - How texture filters (linearly, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	// Wrap mode describes what to do if we go outside the boundaries of
	// texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	// At this point, we are now ready to load and send some data to OpenGL.
	glTexImage2D(GL_TEXTURE_2D,
							0 ,
						GL_RGB,
                        m_chunkSize,
                        m_chunkSize,
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						m_noiseData); // Here is the raw pixel data
    // We are done with our texture data so we can unbind.
    // Generate a mipmap
    glGenerateMipmap(GL_TEXTURE_2D);                        
	// We are done with our texture data so we can unbind.    
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadCubemapTexture(){
	std::vector<std::string> faces = {
		"skybox/right.ppm",
		"skybox/left.ppm",
		"skybox/top.ppm",
		"skybox/bottom.ppm",
		"skybox/front.ppm",
		"skybox/back.ppm"
	};

	glEnable(GL_TEXTURE_CUBE_MAP); 
	glGenTextures(1,&m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	for (int i = 0; i < faces.size(); i++){
		std::string filepath = faces[i];
		Image im = Image(filepath);
    	im.LoadPPM(true);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
						0 ,
						GL_RGB,
                        im.GetWidth(),
                        im.GetHeight(),
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						im.GetPixelDataPtr());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	

}


void Texture::LoadTexture(const std::string filepath){
	// Set member variable
    m_filepath = filepath;
    // Load our actual image data
    // This method loads .ppm files of pixel data
    m_image = new Image(filepath);
    m_image->LoadPPM(true);

    glEnable(GL_TEXTURE_2D); 
	// Generate a buffer for our texture
    glGenTextures(1,&m_textureID);
    // Similar to our vertex buffers, we now 'select'
    // a texture we want to bind to.
    // Note the type of data is 'GL_TEXTURE_2D'
    glBindTexture(GL_TEXTURE_2D, m_textureID);
	// Now we are going to setup some information about
	// our textures.
	// There are four parameters that must be set.
	// GL_TEXTURE_MIN_FILTER - How texture filters (linearly, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	// Wrap mode describes what to do if we go outside the boundaries of
	// texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	// At this point, we are now ready to load and send some data to OpenGL.
	glTexImage2D(GL_TEXTURE_2D,
							0 ,
						GL_RGB,
                        m_image->GetWidth(),
                        m_image->GetHeight(),
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						 m_image->GetPixelDataPtr()); // Here is the raw pixel data
    // We are done with our texture data so we can unbind.
    // Generate a mipmap
    // glGenerateMipmap(GL_TEXTURE_2D);                        
	// We are done with our texture data so we can unbind.    
	glBindTexture(GL_TEXTURE_2D, 0);
}


// slot tells us which slot we want to bind to.
// We can have multiple slots. By default, we
// will set our slot to 0 if it is not specified.
void Texture::Bind(unsigned int slot) const{
	// Using OpenGL 'state' machine we set the active texture
	// slot that we want to occupy. Again, there could
	// be multiple at once.
	// At the time of writing, OpenGL supports 8-32 depending
	// on your hardware.
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}


