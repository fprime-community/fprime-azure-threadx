#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

#include "app_azure_rtos.h"

// Type used to piggyback on class member "m_handle" all required data to
// operate a Task object without the need to add extra members in Task.hpp
typedef struct taskHandleDataTag
{
    TX_THREAD handle; // handle for implementation specific task on Azure ThreadX
    ULONG*    stackStart;
} taskHandleData_t;

static void thread_entry_wrapper(ULONG arg) {
    FW_ASSERT(arg);
    Os::Task::TaskRoutineWrapper *task = reinterpret_cast<Os::Task::TaskRoutineWrapper*>(arg);
    FW_ASSERT(task->routine);
    task->routine(task->arg);
}

namespace Os {
    Task::Task() : m_handle(reinterpret_cast<POINTER_CAST>(nullptr)), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false), m_routineWrapper() {
    }
    Task::TaskStatus Task::start(const Fw::StringBase &name, taskRoutine routine, void* arg, NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize,  NATIVE_UINT_TYPE cpuAffinity, NATIVE_UINT_TYPE identifier) {

        FW_ASSERT(routine);

        this->m_name = "TV_";
        this->m_name += name;
        this->m_identifier = identifier;
        this->m_started = false;
        this->m_affinity = cpuAffinity; // ThreadX does not support AMP/SMP on STM32H7(ARM Cortex-Mx) platform.
        
        // If a registry has been registered, register task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->addTask(this);
        }

        TaskStatus taskStatus = TASK_OK;
        UINT ret = TX_SUCCESS;

        // ThreadX memory pool pointer
        TX_BYTE_POOL* bytePoolPtr;

        // Get ThreadX memory pool
        ret = app_tx_get_byte_pool(&bytePoolPtr);

        if(ret == TX_SUCCESS)
        {
            // Allocate the memory for auxiliary task data handling structure
            if (tx_byte_allocate(bytePoolPtr, (VOID **)&this->m_handle, sizeof(taskHandleData_t), TX_NO_WAIT) != TX_SUCCESS)
            {
                ret = TX_POOL_ERROR;
            }
            else
            {
                // Allocate the memory for the task stack
                if (tx_byte_allocate(bytePoolPtr, (VOID **)&(reinterpret_cast<taskHandleData_t*>(this->m_handle)->stackStart), stackSize, TX_NO_WAIT) != TX_SUCCESS)
                {
                    ret = TX_POOL_ERROR;
                }
                else
                {
                    this->m_routineWrapper.routine = routine;
                    this->m_routineWrapper.arg = arg;

                    ret = tx_thread_create(reinterpret_cast<TX_THREAD*>(this->m_handle),
                                           (char*)this->m_name.toChar(),
                                           thread_entry_wrapper,
                                           reinterpret_cast<ULONG>(&this->m_routineWrapper),
                                           reinterpret_cast<taskHandleData_t*>(this->m_handle)->stackStart,
                                           stackSize,
                                           (priority),
                                           (priority),
                                           TX_NO_TIME_SLICE,
                                           TX_AUTO_START);
                }
            }
        }

        // Check ThreadX return status
        switch (ret)
        {
            case TX_SUCCESS:
                Task::s_numTasks++;
                break;
            case TX_THREAD_ERROR:
            case TX_PTR_ERROR:
            case TX_POOL_ERROR:
            case TX_SIZE_ERROR:
            case TX_PRIORITY_ERROR:
            case TX_THRESH_ERROR:
            case TX_START_ERROR:
            case TX_CALLER_ERROR:
                printf("thread creation [ERROR][0x%.2x]\n", ret);
                taskStatus = TASK_INVALID_PARAMS;
                break;
            default:
                taskStatus = TASK_UNKNOWN_ERROR;
                break;
        }

        return taskStatus;
    }
    
    Task::~Task() {
        // If a registry has been registered, remove task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->removeTask(this);
        }
        
        // Delete the thread object from ThreadX
        tx_thread_delete(reinterpret_cast<TX_THREAD*>(this->m_handle));

        // Release task's stack allocated memory
        (void)tx_byte_release((VOID *)reinterpret_cast<taskHandleData_t*>(this->m_handle)->stackStart);

        // Release auxiliary handle allocated memory
        (void)tx_byte_release((VOID *)this->m_handle);

    }
    
    Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
    {

        // Lookup the number of clock ticks for the specified
        // number of milliseconds. Use the ticks to call the
        // task sleep function call.
        Task::TaskStatus status = TASK_DELAY_ERROR;

        //NOTE: For ThreadX the number o ticks per second is set on
        //      TX_TIMER_TICKS_PER_SECOND defined at tx_api.h (currently 1000 ticks/s)
        U32 ticks = ((milliseconds * TX_TIMER_TICKS_PER_SECOND + 999) / 1000);

        if (tx_thread_sleep(ticks) == TX_SUCCESS) {
            status = TASK_OK;
        }
        return status;

    }
    
    void Task::suspend(bool onPurpose) {
        FW_ASSERT(tx_thread_suspend(reinterpret_cast<TX_THREAD*>(this->m_handle)) == TX_SUCCESS);
    }
                    
    void Task::resume(void) {
        FW_ASSERT(tx_thread_resume(reinterpret_cast<TX_THREAD*>(this->m_handle)) == TX_SUCCESS);
    }

    bool Task::isSuspended(void) {

        CHAR *name;
        UINT state;
        ULONG run_count;
        UINT priority;
        UINT preemption_threshold;
        ULONG time_slice;
        TX_THREAD *next_thread;
        TX_THREAD *suspended_thread;

        // Retrieve information about the previously created thread
        // If status equals TX_SUCCESS, the information requested is valid.
        FW_ASSERT(tx_thread_info_get(reinterpret_cast<TX_THREAD*>(this->m_handle),
                                     &name,
                                     &state,
                                     &run_count,
                                     &priority,
                                     &preemption_threshold,
                                     &time_slice,
                                     &next_thread,
                                     &suspended_thread) == TX_SUCCESS);

        return (state == TX_SUSPENDED)?true:false;
    }

    TaskId Task::getOsIdentifier(void) {
      TaskId T;
      return T;
    }

    Task::TaskStatus Task::join(void **value_ptr) {
        // No task join in ThreadX. Return error if used.
        return TASK_JOIN_ERROR;
    }
}

