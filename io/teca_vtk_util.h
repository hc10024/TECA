#ifndef teca_vtk_util_h
#define teca_vtk_util_h

#include "teca_config.h"
#include "teca_cartesian_mesh.h"

#if defined(TECA_HAS_VTK) || defined(TECA_HAS_PARAVIEW)
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkLongArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkLongLongArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkPointData.h"
class vtkRectilinearGrid;
#else
using vtkFloatArray = void*;
using vtkDoubleArray = void*;
using vtkCharArray = void*;
using vtkUnsignedCharArray = void*;
using vtkShortArray = void*;
using vtkUnsignedShortArray = void*;
using vtkIntArray = void*;
using vtkUnsignedIntArray = void*;
using vtkLongArray = void*;
using vtkUnsignedLongArray = void*;
using vtkLongLongArray = void*;
using vtkUnsignedLongLongArray = void*;
class vtkRectilinearGrid;
#endif

namespace teca_vtk_util
{
// traits class for naming and/or selecting
// the VTK type given a C++ type
template <typename T> struct vtk_tt {};
#define VTK_TT_SPEC(_ctype, _vtype, _fmt)   \
template <>                                 \
struct vtk_tt <_ctype>                      \
{                                           \
    using type = _vtype;                    \
                                            \
    static constexpr const char *str()      \
    { return #_ctype; }                     \
                                            \
    static constexpr const char *fmt()      \
    { return _fmt; }                        \
};
VTK_TT_SPEC(float, vtkFloatArray, "%g")
VTK_TT_SPEC(double, vtkDoubleArray, "%g")
VTK_TT_SPEC(char, vtkCharArray, "%hhi")
VTK_TT_SPEC(unsigned char, vtkUnsignedCharArray, "%hhu")
VTK_TT_SPEC(short, vtkShortArray, "%hi")
VTK_TT_SPEC(unsigned short, vtkUnsignedShortArray, "%hu")
VTK_TT_SPEC(int, vtkIntArray, "%i")
VTK_TT_SPEC(unsigned int, vtkUnsignedIntArray, "%u")
VTK_TT_SPEC(long, vtkLongArray, "%li")
VTK_TT_SPEC(unsigned long, vtkUnsignedLongArray, "%lu")
VTK_TT_SPEC(long long, vtkLongLongArray, "%lli")
VTK_TT_SPEC(unsigned long long, vtkUnsignedLongLongArray, "%llu")

// deep copy input mesh into the VTK object
// return is 0 if successful
int deep_copy(vtkRectilinearGrid *output,
    const_p_teca_cartesian_mesh input);

};

#endif