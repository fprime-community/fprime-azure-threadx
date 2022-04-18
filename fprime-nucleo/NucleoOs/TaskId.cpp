// File TaskId.cpp
// Author: Ben Soudry (benjamin.s.soudry@jpl.nasa.gov)
//         Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// ThreadX implementation of the TaskId class.

#include <StandardTypes.hpp>
#include <Os/TaskId.hpp>

namespace Os
{
    TaskId::TaskId(void)
    {
        id = tx_thread_identify();
    }
    TaskId::~TaskId(void)
    {
    }

    bool TaskId::operator==(const TaskId& T) const
    {
        return (T.id == id);
    }

    bool TaskId::operator!=(const TaskId& T) const
    {
        return (T.id != id);
    }

    TaskIdRepr TaskId::getRepr(void) const
    {
        return this->id;
    }
}
