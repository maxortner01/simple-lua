#pragma once

#include "TypeMap.hpp"

#include "../Util.hpp"
#include "../Def.hpp"

namespace SL
{
    struct Runtime;
}

namespace SL::Lib
{
    /**
     * @brief Represents a base library package in a Lua script
     */
    struct SL_SYMBOL Base
    {
        Base(const Base&) = delete;
        Base(Base&&) = delete;

        using Map = std::unordered_map<std::string, SL::Function>;

        /**
         * @brief Helper function extracting a list of types arguments from a Lua stack.
         * @tparam Args Types of the arguments
         * @param L     Lua state
         * @return std::tuple<Args...> Values of the Lua arguments
         */
        template<typename... Args>
        static std::tuple<Args...>
        extractArgs(State L);

        /**
         * @brief Registers the functions in this library with the given runtime.
         * @param runtime Lua runtime to register the functions with
         */
        void registerFunctions(SL::Runtime& runtime) const;

        SL::Table asTable() const;

    protected:
        Base(
            const std::string& name,
            const Map& funcs);
        
        ~Base() = default;

        std::string _name;
        Map         _funcs;
    };

    namespace detail
    {
    
    std::size_t __getTop(State L);
    bool        __isNil(State L);
    void        __pop(State L, uint32_t n);

    }

    /* struct Base */
    template<typename... Args>
    std::tuple<Args...>
    Base::extractArgs(State L)
    {
        SL_ASSERT(detail::__getTop(L) == sizeof...(Args), "Lua arguments do not match expected args.");
        std::tuple<Args...> values;
        Util::CompileTime::static_for<sizeof...(Args)>([&](auto n) {
            SL_ASSERT(!detail::__isNil(L), "Argument nil");

            const std::size_t I = n;
            const auto i = sizeof...(Args) - I - 1;
            using Type = Util::CompileTime::NthType<i, Args...>;
            SL_ASSERT(SL::CompileTime::TypeMap<Type>::check(L), "Type mismatch");
            
            const auto count = detail::__getTop(L);
            std::get<i>(values) = SL::CompileTime::TypeMap<Type>::construct(L);
            if (detail::__getTop(L) == count) detail::__pop(L, 1);
        });
        return values;
    }

} // SL::Lib
