#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Math/float4x4.h"

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw();

	bool            Init();
	update_status   Update();
	bool            CleanUp();

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);

    void            CheckRaycast(float3 begin, float3 end, float3 colour = { 255.0f, 0.0f, 0.0f });

    bool check_raycast = false;
private:
    float3 begin_ray = { 0.0f, 0.0f, 0.0f };
    float3 end_ray = { 0.0f , 0.0f, 0.0f };
    float3 colour_ray = { 0.0f , 0.0f, 0.0f };
    static DDRenderInterfaceCoreGL* implementation;
};

#endif /* _MODULE_DEBUGDRAW_H_ */
