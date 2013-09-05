#pragma once
#pragma warning(disable:4305)
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#pragma warning(disable:4193)
#pragma warning(disable:4275)

#include <windows.h>

#include <list>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <fstream>
#include <iostream>
#include <time.h>

#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreMaterial.h>

#include <OIS.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Texture.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#include <QTextCodec>
#include <QImage>
#include <QFile>
#include <QDataStream>


#include <NxOgre.h>
#include <Critter.h>

#include <Hydrax.h>
#include <Noise/Perlin/Perlin.h>
#include <Modules/ProjectedGrid/ProjectedGrid.h>

extern "C" 
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

#include "lua_tinker.h"
#include <boost/thread.hpp>