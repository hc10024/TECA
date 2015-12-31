#include "teca_vtk_cartesian_mesh_writer.h"

#include "teca_cartesian_mesh.h"
#include "teca_array_collection.h"
#include "teca_variant_array.h"
#include "teca_file_util.h"
#include "teca_config.h"

#if defined(TECA_HAS_VTK)
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
#include "vtkRectilinearGrid.h"
#include "vtkXMLRectilinearGridWriter.h"
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
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>

#if defined(TECA_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

using std::vector;
using std::string;
using std::ostringstream;
using std::cerr;
using std::endl;

// helper for naming and/or selecting
// the corresponding vtk type
template <typename T> struct vtk_tt {};
#define VTK_TT_SPEC(_ctype, _vtype)     \
template <>                             \
struct vtk_tt <_ctype>                  \
{                                       \
    using type = _vtype;                \
                                        \
    static constexpr const char *str()  \
    { return #_ctype; }                 \
};
VTK_TT_SPEC(float, vtkFloatArray)
VTK_TT_SPEC(double, vtkDoubleArray)
VTK_TT_SPEC(char, vtkCharArray)
VTK_TT_SPEC(unsigned char, vtkUnsignedCharArray)
VTK_TT_SPEC(short, vtkShortArray)
VTK_TT_SPEC(unsigned short, vtkUnsignedShortArray)
VTK_TT_SPEC(int, vtkIntArray)
VTK_TT_SPEC(unsigned int, vtkUnsignedIntArray)
VTK_TT_SPEC(long, vtkLongArray)
VTK_TT_SPEC(unsigned long, vtkUnsignedLongArray)
VTK_TT_SPEC(long long, vtkLongLongArray)
VTK_TT_SPEC(unsigned long long, vtkUnsignedLongLongArray)

#if !defined(TECA_HAS_VTK)
// **************************************************************************
int write_vtk_legacy_header(FILE *ofile,
    const const_p_teca_variant_array &x, const const_p_teca_variant_array &y,
    const const_p_teca_variant_array &z, bool bin=true,
    const std::string &comment = "")
{
    if (!x && !y &&!z)
    {
        TECA_ERROR("data must be at least 1 dimensional")
        return -1;
    }

    std::ostringstream oss;
    oss << "# vtk DataFile Version 2.0" << std::endl;

    if (comment.empty())
    {
        time_t rawtime;
        time(&rawtime);
        struct tm *timeinfo = localtime(&rawtime);

        char date_str[128] = {'\0'};
        strftime(date_str, 128, "%F %T", timeinfo);
        oss << "writen by TECA cartesian mesh writer "
            << TECA_VERSION_DESCR << " " << date_str << endl;
    }
    else
    {
        oss << comment;
    }

    size_t nx = (x ? x->size() : 1);
    size_t ny = (y ? y->size() : 1);
    size_t nz = (z ? z->size() : 1);

    const char *coord_type_str = nullptr;
    TEMPLATE_DISPATCH(const teca_variant_array_impl,
        (x ? x.get() : (y ? y.get() : (z ? z.get() : nullptr))),
        coord_type_str = vtk_tt<NT>::str();
        )

    oss << (bin ? "BINARY" : "ASCII") << std::endl
        << "DATASET RECTILINEAR_GRID" << std::endl
        << "DIMENSIONS " << nx << " " << ny << " " << nz << std::endl
        << "X_COORDINATES " << nx << coord_type_str << std::endl;

    fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
    oss.str("");

    teca_binary_stream tmp;
    if (x)
    {
        if (bin)
        {
            x->to_stream(tmp);
            fwrite(tmp.get_data(), 1, tmp.size(), ofile);
        }
        else
        {
            x->to_stream(oss);
        }
    }
    else
    {
        oss << "0";
    }

    oss << std::endl << "Y_COORDINATES " << ny
        << coord_type_str << std::endl;

    fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
    oss.str("");

    if (y)
    {
        if (bin)
        {
            tmp.resize(0);
            y->to_stream(tmp);
            fwrite(tmp.get_data(), 1, tmp.size(), ofile);
        }
        else
        {
            y->to_stream(oss);
        }
    }
    else
    {
        oss << "0";
    }

    oss << std::endl << "Z_COORDINATES " << nz
        << coord_type_str << std::endl;

    fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
    oss.str("");

    if (z)
    {
        if (bin)
        {
            tmp.resize(0);
            z->to_stream(tmp);
            fwrite(tmp.get_data(), 1, tmp.size(), ofile);
        }
        else
        {
            z->to_stream(oss);
        }
    }
    else
    {
        oss << "0";
    }

    oss << std::endl;
    fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);

    return 0;
}

// **************************************************************************
enum center_t { cell, point, face, edge };

int write_vtk_legacy_attribute(FILE *ofile,
    const const_p_teca_array_collection &data, center_t cen, bool bin = true)
{
    size_t n_arrays = data->size();

    if (!n_arrays)
        return 0;

    std::ostringstream oss;
    oss << std::endl;
    switch (cen)
    {
    case center_t::cell: oss << "CELL"; break;
    case center_t::point: oss << "POINT"; break;
    default: oss << "FIELD"; break;
    }
    oss << "_DATA " << data->get(0)->size() << std::endl;

    fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
    oss.str("");

    teca_binary_stream tmp;
    for (size_t i = 0; i < n_arrays; ++i)
    {
        const_p_teca_variant_array array = data->get(i);

        oss << "SCALARS ";

        std::string array_name = data->get_name(i);
        if (array_name.empty())
            oss << "array_" << i;
        else
            oss << array_name;

        TEMPLATE_DISPATCH(const teca_variant_array_impl,
            array.get(),
            oss << " " << vtk_tt<NT>::str();
            )
        else
        {
            TECA_ERROR("unsupported type encountered")
            return -1;
        }

        oss << " 1" << std::endl
            << "LOOKUP_TABLE default" << std::endl;

        fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
        oss.str("");

        if (bin)
        {
            array->to_stream(tmp);
            fwrite(tmp.get_data(), 1, tmp.size(), ofile);
            fwrite("\n", 1, 1, ofile);

            tmp.resize(0);
        }
        else
        {
            array->to_stream(oss);
        }

        oss << std::endl;

        fwrite(oss.str().c_str(), 1, oss.tellp(), ofile);
        oss.str("");
    }

    return 0;
}
#endif

// --------------------------------------------------------------------------
teca_vtk_cartesian_mesh_writer::teca_vtk_cartesian_mesh_writer()
    : file_name(""), binary(1)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
teca_vtk_cartesian_mesh_writer::~teca_vtk_cartesian_mesh_writer()
{}

#if defined(TECA_HAS_BOOST)
// --------------------------------------------------------------------------
void teca_vtk_cartesian_mesh_writer::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for " + prefix
        + "(teca_vtk_cartesian_mesh_writer)");

    opts.add_options()
        TECA_POPTS_GET(string, prefix,file_name,
            "path/name to write series to")

        TECA_POPTS_GET(int, prefix,binary,
            "if set write raw binary (ie smaller, faster)")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void teca_vtk_cartesian_mesh_writer::set_properties(
    const string &prefix, variables_map &opts)
{
    TECA_POPTS_SET(opts, string, prefix, file_name)
    TECA_POPTS_SET(opts, int, prefix, binary)
}
#endif

// --------------------------------------------------------------------------
const_p_teca_dataset teca_vtk_cartesian_mesh_writer::execute(
    unsigned int port,
    const std::vector<const_p_teca_dataset> &input_data,
    const teca_metadata &request)
{
    (void)port;

    const_p_teca_cartesian_mesh mesh
        = std::dynamic_pointer_cast<const teca_cartesian_mesh>(
            input_data[0]);

    if (!mesh)
    {
        TECA_ERROR("empty input")
        return nullptr;
    }

    unsigned long time_step = 0;
    if (mesh->get_time_step(time_step) &&
        request.get("time_step", time_step))
    {
        TECA_ERROR("request missing \"time_step\"")
        return nullptr;
    }

    // if we have VTK then use their XML file formats
    // otherwise fallback to our legacy writer
#if defined(TECA_HAS_VTK)
    vector<unsigned long> extent(6, 0);
    if (mesh->get_extent(extent) && request.get("extent", extent))
    {
        TECA_ERROR("request missing \"extent\"")
        return nullptr;
    }

    vtkRectilinearGrid *rg = vtkRectilinearGrid::New();
    rg->SetExtent(
        extent[0], extent[1],
        extent[2], extent[3],
        extent[4], extent[5]);

    // transfer coordinates
    const_p_teca_variant_array x = mesh->get_x_coordinates();
    TEMPLATE_DISPATCH(const teca_variant_array_impl, x.get(),
        const TT *xx = static_cast<const TT*>(x.get());
        vtk_tt<NT>::type *a = vtk_tt<NT>::type::New();
        a->SetNumberOfTuples(xx->size());
        NT *p_a = a->GetPointer(0);
        memcpy(p_a, xx->get(), sizeof(NT)*xx->size());
        rg->SetXCoordinates(a);
        a->Delete();
        );

    const_p_teca_variant_array y = mesh->get_y_coordinates();
    TEMPLATE_DISPATCH(const teca_variant_array_impl, y.get(),
        const TT *yy = static_cast<const TT*>(y.get());
         vtk_tt<NT>::type *a = vtk_tt<NT>::type::New();
        a->SetNumberOfTuples(yy->size());
        NT *p_a = a->GetPointer(0);
        memcpy(p_a, yy->get(), sizeof(NT)*yy->size());
        rg->SetYCoordinates(a);
        a->Delete();
        )

    const_p_teca_variant_array z = mesh->get_z_coordinates();
    TEMPLATE_DISPATCH( const teca_variant_array_impl, z.get(),
        const TT *zz = static_cast<const TT*>(z.get());
        vtk_tt<NT>::type *a = vtk_tt<NT>::type::New();
        a->SetNumberOfTuples(zz->size());
        NT *p_a = a->GetPointer(0);
        memcpy(p_a, zz->get(), sizeof(NT)*zz->size());
        rg->SetZCoordinates(a);
        a->Delete();
        )

    // transform point data
    const_p_teca_array_collection pd = mesh->get_point_arrays();
    unsigned int n_arrays = pd->size();
    for (unsigned int i = 0; i< n_arrays; ++i)
    {
        const_p_teca_variant_array a = pd->get(i);
        string name = pd->get_name(i);

        TEMPLATE_DISPATCH(const teca_variant_array_impl, a.get(),
            const TT *aa = static_cast<const TT*>(a.get());
            vtk_tt<NT>::type *b = vtk_tt<NT>::type::New();
            b->SetNumberOfTuples(aa->size());
            b->SetName(name.c_str());
            NT *p_b = b->GetPointer(0);
            memcpy(p_b, aa->get(), sizeof(NT)*aa->size());
            rg->GetPointData()->AddArray(b);
            b->Delete();
            )
    }

    string out_file = this->file_name;
    teca_file_util::replace_timestep(out_file, time_step);

    vtkXMLRectilinearGridWriter *w = vtkXMLRectilinearGridWriter::New();
    w->SetDataModeToAppended();     // infact this is how you get
    w->SetEncodeAppendedDataOff();  // a binary file
    w->SetFileName(out_file.c_str());
    w->SetInputData(rg);
    w->Write();

    w->Delete();
    rg->Delete();
#else
    // built without VTK. write as legacy file
    std::string out_file = this->file_name;
    teca_file_util::replace_timestep(out_file, time_step);

    const char *mode = this->binary ? "wb" : "w";

    FILE *ofile = fopen(out_file.c_str(), mode);
    if (!ofile)
    {
        const char *err_desc = strerror(errno);
        TECA_ERROR("Failed to open \"" << out_file << "\""
             << std::endl << err_desc)
        return nullptr;
    }

    if (write_vtk_legacy_header(ofile, mesh->get_x_coordinates(),
        mesh->get_y_coordinates(), mesh->get_z_coordinates(),
        this->binary))
    {
        TECA_ERROR("failed to write the header")
        return nullptr;
    }

    if (write_vtk_legacy_attribute(ofile, mesh->get_point_arrays(),
        center_t::point, this->binary))
    {
        TECA_ERROR("failed to write point arrays")
        return nullptr;
    }

    if (write_vtk_legacy_attribute(ofile, mesh->get_cell_arrays(),
        center_t::cell, this->binary))
    {
        TECA_ERROR("failed to write point arrays")
        return nullptr;
    }
#endif

    return p_teca_dataset();
}
