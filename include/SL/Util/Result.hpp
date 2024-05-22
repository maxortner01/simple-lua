#pragma once

#include "../Def.hpp"

#include <string>
#include <cassert>
#include <optional>

namespace SL::Util
{
    /**
     * @brief Simple wrapper class for passing errors along to user.
     * @tparam T Raw error type
     */
    template<typename T = int>
    struct Error
    {
        Error(const T& num, const std::string& msg = "");
        ~Error() = default;

        const T& code() const;
        const std::string& message() const;
    
    private:
        T _num;
        std::string _msg;
    };

    /**
     * @brief Basic way to return a value or an error.
     * 
     * @tparam T The return type of the result
     * @tparam E The type of error to be handled
     */
    template<typename T, typename E = Error<>>
    struct Result
    {
        Result(const Result&) = delete;
        Result(Result&&) = delete;
        
        Result(T&& val);
        Result(const E& err);
        ~Result() = default;

        T&       value();
        const T& value() const;
        const E& error() const;
        bool     good()  const;

        operator bool() const;

        constexpr T*       operator->()       { return &*_val; }
        constexpr const T* operator->() const { return &*_val; }
        constexpr T&       operator*()        { return *_val; }
        constexpr const T& operator*()  const { return *_val; }

    private:
        std::optional<T> _val;
        std::optional<E> _err;
    };

    template<typename E>
    struct Result<void, E>
    {
        Result(const Result&) = delete;
        Result(Result&&) = delete;

        Result() = default;
        Result(const E& err);
        ~Result() = default;

        const E& error() const;
        bool     good()  const;

        operator bool() const;

    private:
        std::optional<E> _err;
    };

    /* struct Error */
    template<typename T>
    Error<T>::Error(const T& num, const std::string& msg) :
        _num(num),
        _msg(msg)
    {   }

    template<typename T>
    const T& Error<T>::code() const
    { return _num; }

    template<typename T>
    const std::string& Error<T>::message() const
    { return _msg; }

    /* struct Result */
    template<typename T, typename E>
    Result<T, E>::Result(T&& val) :
        _val(val)
    {   }

    template<typename T, typename E>
    Result<T, E>::Result(const E& err) :
        _err(err)
    {   }

    template<typename E>
    Result<void, E>::Result(const E& err) :
        _err(err)
    {   }

    template<typename T, typename E>
    T& Result<T, E>::value()
    {
        SL_ASSERT(good(), "Result not good!");
        return _val.value();
    }

    template<typename T, typename E>
    const T& Result<T, E>::value() const
    {
        SL_ASSERT(good(), "Result not good!");
        return _val.value();
    }

    template<typename T, typename E>
    const E& Result<T, E>::error() const
    {
        SL_ASSERT(_err.has_value(), "Result is good!");
        return _err.value();
    }

    template<typename E>
    const E& Result<void, E>::error() const
    {
        SL_ASSERT(_err.has_value(), "Result is good!");
        return _err.value();
    }

    template<typename T, typename E>
    bool Result<T, E>::good() const
    {
        return _val.has_value();
    }

    template<typename E>
    bool Result<void, E>::good() const
    {
        return !_err.has_value();
    }

    template<typename T, typename E>
    Result<T, E>::operator bool() const
    { return good(); }

    template<typename E>
    Result<void, E>::operator bool() const
    { return good(); }

} // S2D::Util
