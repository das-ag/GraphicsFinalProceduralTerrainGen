#include "Terrain.hpp"
#include "Image.hpp"
#include "PerlinNoise.hpp"

#include <glad/glad.h>
#include <memory>
#include <iostream>

// Constructor for our object
// Calls the initialization method
Terrain::Terrain(unsigned int chunkSize, unsigned int LOD, float xOffset, float zOffset) : m_chunkSize(chunkSize){
    std::cout << "(Terrain.cpp) Constructor called \n";
    

    // WIP Level of Detail
    if (LOD == 0){
        m_LOD = 1;
    } else {
        m_LOD = 2*LOD;
    }

    m_scaledSize = m_chunkSize / m_LOD;

    m_xOffset = m_chunkSize * xOffset;
    m_zOffset = m_chunkSize * zOffset;

    // Initiliaze height data
    m_noiseData = new float[m_scaledSize*m_scaledSize];

    
    Init();
}

// Destructor
Terrain::~Terrain(){
    // Delete our allocatted higheithmap data
    if(m_noiseData!=nullptr){
        delete m_noiseData;
    }

    if(m_terrainColor!=nullptr){
        delete m_terrainColor;
    }
}

// For creating a height curve
float noiseToHeight(float noiseval){
    // Noise	 	Color				Terrain
    // 0		 	0, 0, 128, 255		Deep	
    // 0.0375		0, 0, 255, 255		Shallow
    // 0.5			0, 128, 255, 255	shore
    // 0.53125		240, 240, 64, 255	sand
    // 0.5625		32, 160, 0, 255		grass
    // 0.6875		224, 224, 0, 255	dirt
    // 0.875		128, 128, 128, 255	rock
    // 1			255, 255, 255, 255	snow
    
    
    float height;
    // level height at sea level
    if (noiseval <= 0.5f) {
        height = 50;
    } 
    // deep to shallow
    else {
         height = noiseval * 100;
    }

    return height;
}

void Terrain::Init(){
    // Create the initial grid of vertices.
    GenerateNoiseMap();
    // TODO: (Inclass) Build grid of vertices! 
    for(unsigned int z = 0; z < m_chunkSize; ++z){
        for(unsigned int x = 0; x < m_chunkSize; ++x){


            float v = ((float) z / (float) m_chunkSize);
            float u = ((float) x / (float) m_chunkSize);

            float noise = m_noiseData[x+(z*m_chunkSize)];

            float y = noiseToHeight(noise);
            
            m_geometry.AddVertex((float) x, y, (float) z, u, v);
            
        }   
    }
    
    // TODO: (Inclass) Build triangle strip

    for (unsigned int z=0; z < m_chunkSize-1; ++z){
        for (unsigned int x =0; x < m_chunkSize-1; ++x) {

            m_geometry.AddIndex(x+(z*m_chunkSize));  // 0 = 0
            m_geometry.AddIndex(x+(z*m_chunkSize) + m_chunkSize); // 1 = m_chunkSize
            m_geometry.AddIndex(x+(z*m_chunkSize) + 1); // 2 = 1

            m_geometry.AddIndex(x+(z*m_chunkSize) + 1);
            m_geometry.AddIndex(x+(z*m_chunkSize) + m_chunkSize);
            m_geometry.AddIndex(x+(z*m_chunkSize) + m_chunkSize+1);
        }
    }

   // Finally generate a simple 'array of bytes' that contains
   // everything for our buffer to work with.
   m_geometry.Gen();  
   // Create a buffer and set the stride of information
   m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());
}

// Loads an image and uses it to set the heights of the terrain.
void Terrain::LoadHeightMap(Image image){

}

void Terrain::LoadPerlinTexture(){
   m_textureDiffuse.LoadPerlinTexture(m_chunkSize, m_terrainColor);
}

glm::uvec3 interpolateColor(float noiseval, float noiseFloor, float noiseCeiling, glm::vec3 baseCol, glm::vec3 ceilingCol){
    float frac = (noiseval - noiseFloor) / (noiseCeiling - noiseFloor);
    return glm::uvec3(((ceilingCol - baseCol) * frac) + baseCol);
}


glm::uvec3 noiseToColor(float noiseval){
     // Noise	 	Color				Terrain
    // 0		 	0, 0, 128, 255		Deep	
    // 0.0375		0, 0, 255, 255		Shallow
    // 0.5			0, 128, 255, 255	shore
    // 0.53125		240, 240, 64, 255	sand
    // 0.5625		32, 160, 0, 255		grass
    // 0.6875		224, 224, 0, 255	dirt
    // 0.875		128, 128, 128, 255	rock
    // 1			255, 255, 255, 255	snow

    static glm::vec3 deepCol = glm::vec3(0.0, 0.0, 128.0);
    static glm::vec3 shallowCol = glm::vec3(0.0, 0.0, 255.0);    
    static glm::vec3 shoreCol = glm::vec3(0.0, 128.0, 255.0);
    static glm::vec3 sandCol = glm::vec3(240.0, 240.0, 64.0);
    static glm::vec3 grassCol = glm::vec3(32.0, 160.0, 0.0);
    static glm::vec3 dirtCol = glm::vec3(64.0, 64.0, 64.0);
    static glm::vec3 rockCol = glm::vec3(128.0, 128.0, 128.0);
    static glm::vec3 snowCol = glm::vec3(255.0, 255.0, 255.0);

    glm::uvec3 interpolatedCol;

    // Rock to snow
    if (noiseval >= 0.875f){
        interpolatedCol = interpolateColor(noiseval, 0.875f, 1.0f, rockCol, snowCol);
    } 
    // Dirt to rock
    else if (noiseval >= 0.6875f) {
        interpolatedCol = interpolateColor(noiseval, 0.6875f, 0.875f, dirtCol, rockCol);
    } 
    // Grass to dirt
    else if (noiseval >= 0.5625f) {
        interpolatedCol = interpolateColor(noiseval, 0.5625f, 0.6875f, grassCol, dirtCol);
    } 
    // Sand to grass
    else if (noiseval >= 0.53125f) {
        interpolatedCol = interpolateColor(noiseval, 0.53125f, 0.5625f, sandCol, grassCol);
    } 
    // shore to sand
    else if (noiseval >= 0.5f) {
        interpolatedCol = interpolateColor(noiseval, 0.5f, 0.53125f, shoreCol, sandCol);
    } 
    // shallow to shore
    else if (noiseval >= 0.0375f) {
        interpolatedCol = interpolateColor(noiseval, 0.0375f, 0.5f, shallowCol, shoreCol);
    } 
    // deep to shallow
    else {
        interpolatedCol = interpolateColor(noiseval, 0.0f, 0.0375f, deepCol, shallowCol);
    }

    return interpolatedCol;
}

float Terrain::LayerPerlinNoise(float x, float z, int numOctaves, int startOctave = 1){
    float result = 0;
    
    float persistence = 0.3f;
    float amplitude = 1.0f;
    float frequency = 4.0f;
    float noiseWeight = amplitude;
    
    const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };
    
    for (int i = (startOctave - 1); i < numOctaves; ++i){

        

        float sampleX = (x + m_xOffset) * (frequency / m_chunkSize);  // m_chunkSize = width
        float sampleY = (z + m_zOffset) * (frequency / m_chunkSize);  // m_chunkSize = height
   
        result += amplitude * perlin.octave2D_01(sampleX, sampleY, (i + 1),  persistence);

        if (i == numOctaves - 1){
           break;
        }
          
        persistence += 0.05f;
        frequency *= 2.0f;
        amplitude *= 0.5f;
        noiseWeight += amplitude;
    }

    result = result/noiseWeight;

    // if(result < 0.1f){
    //     result = 0.1f;
    // }

    return result; 
}



void Terrain::GenerateNoiseMap(){

    m_terrainColor = new uint8_t[m_chunkSize*m_chunkSize*3];

    for(unsigned int z = 0; z < m_chunkSize; ++z){
        for(unsigned int x = 0; x < m_chunkSize; ++x){


            float noiseval = LayerPerlinNoise(x, z, 6, 1);
            
            m_noiseData[x+(z*m_chunkSize)] = noiseval;

            glm::uvec3 interpolatedCol = noiseToColor(noiseval);
             
            m_terrainColor[3*(x+(z*m_chunkSize))    ] = interpolatedCol.r;
            m_terrainColor[3*(x+(z*m_chunkSize)) + 1] = interpolatedCol.g;
            m_terrainColor[3*(x+(z*m_chunkSize)) + 2] = interpolatedCol.b;

        }
    }


    std::cout <<"noise generated" <<std::endl;

}


