#pragma once

#include <soil.h>

class SpriteHandle
{
public:
	SpriteHandle(void);
	~SpriteHandle(void);
	void Load(const char* _Path);
	void Unload();
	void Update();
	void Draw();
};

