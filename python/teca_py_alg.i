%{
#include "teca_config.h"
#include "teca_algorithm.h"
#include "teca_ar_detect.h"
#include "teca_cartesian_mesh_subset.h"
#include "teca_cartesian_mesh_regrid.h"
#include "teca_connected_components.h"
#include "teca_dataset_diff.h"
#include "teca_derived_quantity.h"
#include "teca_derived_quantity_numerics.h"
#include "teca_descriptive_statistics.h"
#include "teca_event_filter.h"
#include "teca_l2_norm.h"
#include "teca_mask.h"
#include "teca_programmable_algorithm.h"
#include "teca_table_calendar.h"
#include "teca_table_sort.h"
#include "teca_table_reduce.h"
#include "teca_table_to_stream.h"
#include "teca_tc_candidates.h"
#include "teca_tc_trajectory.h"
#include "teca_tc_classify.h"
#include "teca_temporal_average.h"
#include "teca_vorticity.h"
#include "teca_py_object.h"
#include "teca_py_algorithm.h"
#include "teca_py_gil_state.h"
%}

%include "teca_config.h"
%include "teca_py_common.i"
%include "teca_py_shared_ptr.i"
%include "teca_py_core.i"
%include "teca_py_data.i"
%include <std_string.i>

/***************************************************************************
 ar_detect
 ***************************************************************************/
%ignore teca_ar_detect::shared_from_this;
%shared_ptr(teca_ar_detect)
%ignore teca_ar_detect::operator=;
%include "teca_ar_detect.h"

/***************************************************************************
 cartesian_mesh_subset
 ***************************************************************************/
%ignore teca_cartesian_mesh_subset::shared_from_this;
%shared_ptr(teca_cartesian_mesh_subset)
%ignore teca_cartesian_mesh_subset::operator=;
%include "teca_cartesian_mesh_subset.h"

/***************************************************************************
 cartesian_mesh_regrid
 ***************************************************************************/
%ignore teca_cartesian_mesh_regrid::shared_from_this;
%shared_ptr(teca_cartesian_mesh_regrid)
%ignore teca_cartesian_mesh_regrid::operator=;
%include "teca_cartesian_mesh_regrid.h"

/***************************************************************************
 connected_components
 ***************************************************************************/
%ignore teca_connected_components::shared_from_this;
%shared_ptr(teca_connected_components)
%ignore teca_connected_components::operator=;
%include "teca_connected_components.h"

/***************************************************************************
 l2_norm
 ***************************************************************************/
%ignore teca_l2_norm::shared_from_this;
%shared_ptr(teca_l2_norm)
%ignore teca_l2_norm::operator=;
%include "teca_l2_norm.h"

/***************************************************************************
 mask
 ***************************************************************************/
%ignore teca_mask::shared_from_this;
%shared_ptr(teca_mask)
%ignore teca_mask::operator=;
%include "teca_mask.h"

/***************************************************************************
 table_reduce
 ***************************************************************************/
%ignore teca_table_reduce::shared_from_this;
%shared_ptr(teca_table_reduce)
%ignore teca_table_reduce::operator=;
%include "teca_table_reduce.h"

/***************************************************************************
 table_to_stream
 ***************************************************************************/
%ignore teca_table_to_stream::shared_from_this;
%shared_ptr(teca_table_to_stream)
%ignore teca_table_to_stream::operator=;
%ignore teca_table_to_stream::set_stream(std::ostream *);
%include "teca_table_to_stream.h"

/***************************************************************************
 temporal_average
 ***************************************************************************/
%ignore teca_temporal_average::shared_from_this;
%shared_ptr(teca_temporal_average)
%ignore teca_temporal_average::operator=;
%include "teca_temporal_average.h"

/***************************************************************************
 vorticity
 ***************************************************************************/
%ignore teca_vorticity::shared_from_this;
%shared_ptr(teca_vorticity)
%ignore teca_vorticity::operator=;
%include "teca_vorticity.h"

/***************************************************************************
 programmable_algorithm
 ***************************************************************************/
%ignore teca_programmable_algorithm::shared_from_this;
%shared_ptr(teca_programmable_algorithm)
%extend teca_programmable_algorithm
{
    // note: it s not worth acquiring the GIL while setting the callbacks
    // as these are intended to be used only from the main thread during
    // initialization

    void set_report_callback(PyObject *f)
    {
        self->set_report_callback(teca_py_algorithm::report_callback(f));
    }

    void set_request_callback(PyObject *f)
    {
        self->set_request_callback(teca_py_algorithm::request_callback(f));
    }

    void set_execute_callback(PyObject *f)
    {
        self->set_execute_callback(teca_py_algorithm::execute_callback(f));
    }
}
%ignore teca_programmable_algorithm::operator=;
%ignore teca_programmable_algorithm::set_report_callback;
%ignore teca_programmable_algorithm::get_report_callback;
%ignore teca_programmable_algorithm::set_request_callback;
%ignore teca_programmable_algorithm::get_request_callback;
%ignore teca_programmable_algorithm::set_execute_callback;
%ignore teca_programmable_algorithm::get_execute_callback;
%include "teca_programmable_algorithm_fwd.h"
%include "teca_programmable_algorithm.h"

/***************************************************************************
 derived_quantity
 ***************************************************************************/
%ignore teca_derived_quantity::shared_from_this;
%shared_ptr(teca_derived_quantity)
%extend teca_derived_quantity
{
    // see notes in teca_programmable_algorithm

    void set_execute_callback(PyObject *f)
    {
        self->set_execute_callback(teca_py_algorithm::execute_callback(f));
    }
}
%ignore teca_derived_quantity::operator=;
%ignore teca_derived_quantity::set_execute_callback;
%ignore teca_derived_quantity::get_execute_callback;
%include "teca_derived_quantity.h"
%include "teca_derived_quantity_numerics.h"

/***************************************************************************
 descriptive_statistics
 ***************************************************************************/
%ignore teca_descriptive_statistics::shared_from_this;
%shared_ptr(teca_descriptive_statistics)
%ignore teca_descriptive_statistics::operator=;
%include "teca_descriptive_statistics.h"

/***************************************************************************
 table_sort
 ***************************************************************************/
%ignore teca_table_sort::shared_from_this;
%shared_ptr(teca_table_sort)
%ignore teca_table_sort::operator=;
%include "teca_table_sort.h"

/***************************************************************************
 table_calendar
 ***************************************************************************/
%ignore teca_table_calendar::shared_from_this;
%shared_ptr(teca_table_calendar)
%ignore teca_table_calendar::operator=;
%include "teca_table_calendar.h"

/***************************************************************************
 tc_candidates
 ***************************************************************************/
%ignore teca_tc_candidates::shared_from_this;
%shared_ptr(teca_tc_candidates)
%ignore teca_tc_candidates::operator=;
%include "teca_tc_candidates.h"

/***************************************************************************
 tc_trajectory
 ***************************************************************************/
%ignore teca_tc_trajectory::shared_from_this;
%shared_ptr(teca_tc_trajectory)
%ignore teca_tc_trajectory::operator=;
%include "teca_tc_trajectory.h"

/***************************************************************************
 tc_classify
 ***************************************************************************/
%ignore teca_tc_classify::shared_from_this;
%shared_ptr(teca_tc_classify)
%ignore teca_tc_classify::operator=;
%include "teca_tc_classify.h"

/***************************************************************************
 dataset_diff
 ***************************************************************************/
%ignore teca_dataset_diff::shared_from_this;
%shared_ptr(teca_dataset_diff)
%ignore teca_dataset_diff::operator=;
%include "teca_dataset_diff.h"

/***************************************************************************
 event_filter
 ***************************************************************************/
%ignore teca_event_filter::shared_from_this;
%shared_ptr(teca_event_filter)
%ignore teca_event_filter::operator=;
%include "teca_event_filter.h"
%extend teca_event_filter
{
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(unsigned long, region_size)
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(unsigned long, region_start);
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(double, region_x_coordinate);
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(double, region_y_coordinate);
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(int, region_id);
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(std::string, region_name);
    TECA_PY_ALGORITHM_VECTOR_PROPERTY(std::string, region_long_name);
}

/***************************************************************************
 tc_stats
 ***************************************************************************/
%pythoncode %{
from teca_tc_stats import *
%}

/***************************************************************************
 tc_trajectory_scalars
 ***************************************************************************/
%pythoncode %{
from teca_tc_trajectory_scalars import *
%}
