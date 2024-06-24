#pragma once

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "core/fs.h"
#include "core/initialize.h"
#include "core/mesh.h"
#include "core/mesh_draw.h"
#include "core/primitives.h"
#include "core/shader.h"
#include "core/texture.h"
#include "math/general.h"
#include "math/vector.h"
#include "math/matrix.h"

// Vulkan
#include "vulkan/choose.h"
