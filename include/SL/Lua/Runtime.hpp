#pragma once

#include <filesystem>

#include "Lib.hpp"
#include "TypeMap.hpp"

#define LUA_HOT_RELOAD

namespace SL
{

    /**
     * @brief Represents a single Lua runtime.
     */
    struct Runtime
    {
        enum class ErrorCode
        {
            None,
            TypeMismatch,
            VariableDoesntExist,
            NotFunction,
            FunctionError
        };

        template<typename T>
        using Result = Util::Result<T, Util::Error<ErrorCode>>;

        /**
         * @brief Construct a Lua runtime from a script
         * @param filename File path to the script
         */
        Runtime(const std::string& filename);
        
        Runtime(Runtime&& r);
        Runtime(const Runtime&) = delete;

        ~Runtime();

        /**
         * @brief Creates a runtime with the given Libraries loaded into it.
         * @tparam Libraries List of library types that are derived from \ref SL::Lib::Base.
         * @param filename Name of the file to load into the runtime
         * @return Runtime The created runtime 
         */
        template<typename... Libraries>
        static Runtime create(const std::string& filename);

        /**
         * @brief Registers a C++ function for use in the Lua runtime
         * @param table_name Name of the table to register function in
         * @param func_name Name of the function to call in Lua
         * @param function Pointer to a static function 
         * @return Result<void> Returns if an error has occured
         */
        Result<void>
        registerFunction(
            const std::string& table_name,
            const std::string& func_name,
            SL::Function function);

        /**
         * @brief Get a global variable by name from runtime
         * @tparam T Type of the global variable (supported types in Lua namespace)
         * @param name Name of the variable in the script
         * @return Result<T> The value of the variable or error
         */
        template<typename T>
        Result<T>
        getGlobal(const std::string& name);

        // TODO: Need to load globals in datastructure so that the file can be hot-reloaded
        // when the lua state is reloaded, the globals that have been loaded in need to be 
        // transferred

        /**
         * @brief Set a global variable from a value to a name
         * @tparam T Type of the global variable (supported types in Lua namespace)
         * @param name  Name of the global variable
         * @param value Value of the global variable
         * @return Result<void> The status of the operation
         */
        template<typename T>
        Result<void>
        setGlobal(const std::string& name, const T& value);

        /**
         * @brief Invokes a Lua function from this environment
         * @tparam Return Expected return types from the function 
         * @tparam Args   Arguments to pass into the function
         * @param name Name of the function
         * @param args Values of the arguments
         * @return Result<std::tuple<Return...>> Contains the values returned from the function or error
         */
        template<typename... Return, typename... Args>
        Result<std::tuple<Return...>>
        runFunction(
            const std::string& name,
            Args&&... args);

        bool     good() const;
        operator bool() const;


        const auto& filename() const { return _filename; }

    private:
        void _pop(std::size_t n = 1) const;
        int _call_func(uint32_t args, uint32_t ret) const;

        State L;
        bool _good;
        std::string _filename;

#   ifdef LUA_HOT_RELOAD
        std::filesystem::file_time_type _last_modified;
#   endif
    };

    template<typename... Libraries>
    Runtime Runtime::create(const std::string& filename)
    {
        using namespace Util::CompileTime;

        Runtime runtime(filename);

        static_for<sizeof...(Libraries)>([&](auto n) 
        {
            const std::size_t i = n;
            using Library = NthType<i, Libraries...>;
            static_assert(std::is_base_of_v<SL::Lib::Base, Library>);
            Library().registerFunctions(runtime);
        });

        return runtime;
    }

    template<typename... Return, typename... Args>
    Runtime::Result<std::tuple<Return...>>
    Runtime::runFunction(
        const std::string& name,
        Args&&... args)
    {
        const auto glob_res = getGlobal<SL::Function>(name);
        if (!glob_res)
        {
            _pop();
            if (glob_res.error().code() == Runtime::ErrorCode::TypeMismatch) return { Runtime::ErrorCode::NotFunction };
            else return { glob_res.error() };
        }

        auto args_set = std::tuple(std::forward<Args>(args)...);
        Util::CompileTime::static_for<sizeof...(args)>([&](auto n){
            constexpr std::size_t I = n;
            using Type = std::remove_reference_t<Util::CompileTime::NthType<I, Args...>>;
            CompileTime::TypeMap<Type>::push(L, std::get<I>(args_set));
        });

        if (_call_func(sizeof...(Args), sizeof...(Return)) != 0) return { { ErrorCode::FunctionError, CompileTime::TypeMap<SL::String>::construct(L) } };
        
        bool err = false;
        auto left = sizeof...(Return);
        auto return_vals = std::tuple<Return...>();
        Util::CompileTime::static_for<sizeof...(Return)>([&](auto n) {
            constexpr std::size_t I = n;
            using Type = Util::CompileTime::NthType<I, Return...>;
            using Map  = SL::CompileTime::TypeMap<Type>;

            if (!err)
            {
                if (Map::check(L))
                {
                    std::get<I>(return_vals) = Map::construct(L);
                    left--;
                }
                else err = true;
            }
        });

        SL_ASSERT(!left, "Still arguments left.");
        if (err)
        {
            _pop(left);
            return { ErrorCode::TypeMismatch };
        }

        return { std::move(return_vals) };
    }

} // SL
