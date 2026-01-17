#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Core/Application/Application.h"
#include <iostream>

extern Omnia::Application Omnia::OmniaApplication;

namespace Omnia
{
    ChunkDataTypePtr _GetChunkDataForMeshing (int cx, int cz)
    {
        if (OmniaApplication.GetWorld() && OmniaApplication.GetWorld()->ChunkExistsInMap(cx, cz))
        {
            Chunk* chunk = OmniaApplication.GetWorld()->RetrieveChunkFromMap(cx, cz);
            return &chunk->p_ChunkContents;
        }

        return nullptr;
    }

    ChunkLightDataTypePtr _GetChunkLightDataForMeshing(int cx, int cz)
    {
        if (OmniaApplication.GetWorld() && OmniaApplication.GetWorld()->ChunkExistsInMap(cx, cz))
        {
            Chunk* chunk = OmniaApplication.GetWorld()->RetrieveChunkFromMap(cx, cz);
            return &chunk->p_ChunkLightInformation;
        }

        return nullptr;
    }

    Block* GetWorldBlock(const glm::vec3& block_pos)
    {
        std::pair<Block*, Chunk*> block = OmniaApplication.GetWorld()->GetBlockFromPosition(block_pos);
        return block.first;
    }
}

int main()
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    while (!glfwWindowShouldClose(Omnia::OmniaApplication.m_Window))
    {
        Omnia::OmniaApplication.OnUpdate();
    }
    return 0;
}