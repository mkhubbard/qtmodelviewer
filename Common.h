#ifndef __COMMON_H__
#define __COMMON_H__

#include "Ogre.h"

#include <iostream>
#include <QDebug>

#define RENDER_UPD_INT 17        // roughly 60fps is our target (QTimer takes integer)

#define ID_WINDOW   "__WINDOW__"
#define ID_CAMERA   "__CAMERA__"
#define ID_SCENEMGR "__SCENEMGR__"
#define ID_LIGHT    "__LIGHT__"

// Resources for materials and meshes used by the
#define RES_GRP_CORE    "__MV_CORE__"

// Resources which have been loaded automatically, typically the path a mesh was loaded from.
#define RES_GRP_AUTO    "__AUTOLOADED__"

#define MESH_NAME           "__MESH__"
#define MESH_NODE_ROOT      "__NODE_PITCH__"
#define MESH_NODE_ENTITY    "__NODE_MESH__"
#define MESH_NODE_SKELETON  "__NODE_MESH_SKELETON__"

#define MESH_NAME_GRID        "__MV_GRID__"
#define MESH_NAME_GRID_BORDER "__MV_GRID_BORDER__"
#define MESH_NAME_GRID_AXIS_Z "__MV_GRID_AXIS_Z"
#define MESH_NAME_GRID_AXIS_X "__MV_GRID_AXIS_X"
#define MESH_NAME_GRID_AXIS_Y "__MV_GRID_AXIS_Y"

#define NODE_GRID_ROOT      "__MV_NODE_GRID__"

#define BOUNDING_BOX_NAME   "__MV_BOUNDING_BOX__"

#define MAT_BOUNDING_BOX    "__MV_BOUNDING_BOX__"
#define MAT_WIREFRAME       "__MV_WIRE__"
#define MAT_POINTS          "__MV_POINTS__"
#define MAT_GRID            "__MV_GRID__"
#define MAT_GRID_BORDER     "__MV_GRID_BORDER__"
#define MAT_GRID_AXIS_Z     "__MV_GRID_AXIS_Z__"
#define MAT_GRID_AXIS_X     "__MV_GRID_AXIS_X__"
#define MAT_GRID_AXIS_Y     "__MV_GRID_AXIS_Y__"

#define BONE_MESH_PREFIX    "__MV_BONE_AXES_"
#define MESH_AXES           "axes.mesh"

// these structures are used to track input movement and other 'view' states
// for the window.
// @todo: would like to reduce this to an object, provide something like a clearTransforms() method for
// taking care of resetting yaw/pitch/roll and any other values thrown away during update.
struct ViewState {
  Ogre::Radian yaw;             // mesh yaw transformation
  Ogre::Radian pitch;           // mesh pitch transformation
  Ogre::Radian roll;            // mesh roll transformation
  Ogre::Vector3 defCamPos;      // default camera position for the scene
  Ogre::Vector3 curCamPos;      // current camera position
  int mouseLastX;               // last mouse X position
  int mouseLastY;               // last mouse Y position
};

#endif // __COMMON_H__
