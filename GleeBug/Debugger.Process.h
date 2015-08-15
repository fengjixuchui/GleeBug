#ifndef _DEBUGGER_PROCESS_H
#define _DEBUGGER_PROCESS_H

#include "Debugger.Global.h"
#include "Debugger.Thread.h"
#include "Debugger.Dll.h"
#include "Debugger.Breakpoint.h"

namespace GleeBug
{
    /**
    \brief Process information structure.
    */
    class ProcessInfo
    {
    public:
        HANDLE hProcess;
        uint32 dwProcessId;
        uint32 dwMainThreadId;

        ThreadInfo* thread;
        bool systemBreakpoint;

        ThreadMap threads;
        DllMap dlls;
        BreakpointMap breakpoints;
        BreakpointCallbackMap breakpointCallbacks;
        BreakpointInfo restoreSoftwareBreakpoint;

        /**
        \brief Constructor.
        \param hProcess Process handle.
        \param dwProcessId Identifier for the process.
        \param dwMainThreadId Identifier for the main thread.
        */
        explicit ProcessInfo(HANDLE hProcess, uint32 dwProcessId, uint32 dwMainThreadId);

        /**
        \brief Read memory from the process.
        \param address The virtual address to read from.
        \param [out] buffer Destination buffer. Cannot be null. May be filled partially on failure.
        \param size The size to read.
        \return true if it succeeds, false if it fails.
        */
        bool MemRead(ptr address, void* buffer, ptr size) const;

        /**
        \brief Write memory to the process.
        \param address The virtual address to write to.
        \param [in] buffer Source buffer. Cannot be null.
        \param size The size to write.
        \return true if it succeeds, false if it fails.
        */
        bool MemWrite(ptr address, const void* buffer, ptr size);

        /**
        \brief Check if an address is a valid read pointer.
        \param address The address to check.
        \return true if the address is valid, false otherwise.
        */
        bool MemIsValidPtr(ptr address) const;

        /**
        \brief Sets a software breakpoint.
        \param address The address to put the breakpoint on.
        \param singleshoot (Optional) True to remove the breakpoint after the first hit.
        \param type (Optional) The software breakpoint type.
        \return true if the breakpoint was set, false otherwise.
        */
        bool SetBreakpoint(ptr address, bool singleshoot = false, SoftwareBreakpointType type = SoftwareBreakpointType::ShortInt3);

        /**
        \brief Sets a software breakpoint.
        \param address The address to put the breakpoint on.
        \param cbBreakpoint The breakpoint callback. Can be written using BIND1(this, MyDebugger::cb).
        \param singleshoot (Optional) True to remove the breakpoint after the first hit.
        \param type (Optional) The software breakpoint type.
        \return true if the breakpoint was set, false otherwise.
        */
        bool SetBreakpoint(ptr address, const BreakpointCallback & cbBreakpoint, bool singleshoot = false, SoftwareBreakpointType type = SoftwareBreakpointType::ShortInt3);

        /**
        \brief Sets a software breakpoint.
        \tparam T Generic type parameter. Must be a subclass of Debugger.
        \param address The address to put the breakpoint on.
        \param debugger This pointer to a subclass of Debugger.
        \param callback Pointer to the callback. Written like: &MyDebugger::cb
        \param singleshoot (Optional) True to remove the breakpoint after the first hit.
        \param type (Optional) The software breakpoint type.
        \return true if the breakpoint was set, false otherwise.
        */
        template <typename T>
        bool SetBreakpoint(ptr address, T* debugger, void(T::*callback)(const BreakpointInfo & info), bool singleshoot = false, SoftwareBreakpointType type = SoftwareBreakpointType::ShortInt3)
        {
            static_cast<void>(static_cast<Debugger*>(debugger));
            return SetBreakpoint(address, std::bind(callback, debugger, std::placeholders::_1), singleshoot, type);
        }
    };
};

#endif //_DEBUGGER_PROCESS_H