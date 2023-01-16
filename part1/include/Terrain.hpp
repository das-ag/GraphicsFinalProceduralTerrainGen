
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "PerlinNoise.hpp"
#include "Image.hpp"
#include "Object.hpp"
#include "glm/vec3.hpp"

#include <vector>
#include <string>
#include <glad/glad.h>

class Terrain : public Object {
public:
    // Takes in a Terrain and a filename for the heightmap.
    Terrain (unsigned int chunkSize,  unsigned int LOD, float xOffset, float zOffset);
    // Destructor
    ~Terrain ();
    // override the initialization routine.
    void Init();
    // Loads a heightmap based on a PPM image
    // This then sets the heights of the terrain.
    void LoadHeightMap(Image image);
    float LayerPerlinNoise(float x, float z, int numOctaves, int startOctave);
    void GenerateNoiseMap();
    void LoadPerlinTexture();
    // From 1-6
    unsigned int m_LOD;
    unsigned int m_scaledSize;

    // number of tiles from origin
    float m_xOffset;
    float m_zOffset;
    float m_persistence = 0.3f;
    float m_amplitude = 1.0f;
    float m_frequency = 4.0f;

private:
    // data
    unsigned int m_chunkSize;

    // Store the height in a multidimensional array
    float* m_noiseData;
    uint8_t* m_terrainColor;
    // Textures for the terrain
    std::vector<Texture> m_textures;
};

#endif
