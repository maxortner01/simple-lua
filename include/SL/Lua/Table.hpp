#pragma once

#include "../Def.hpp"
#include "Lua.hpp"

#include <optional>
#include <unordered_map>
#include <memory>
#include <functional>

namespace SL
{
    struct Table
    {
        /**
         * @brief Represents a value in the table stored in memory with a type.
         */
        struct Data
        {
            std::shared_ptr<void> data;
            int type;

            /**
             * @brief Construct a data entry from a value
             * @tparam T    Type of the data entry
             * @param value Value of the entry
             * @return Data The entry
             */
            template<typename T>
            SL_SYMBOL static Data fromValue(const T& value);

            /**
             * @brief Allocate room and copy the memory of a given value
             * @tparam T    Type of the entry
             * @param value Value of the entry
             * @return std::shared_ptr<void> Pointer to the allocated entry
             */
            template<typename T>
            SL_SYMBOL static std::shared_ptr<void> emplace(const T& value);
        };

        using Map = std::unordered_map<std::string, Data>;

        Table() = default;
        SL_SYMBOL Table(const Map& map);

        /**
         * @brief Constructs a table from the current Lua stack
         * 
         * This assumes that there is a table already at index -1 in the Lua
         * stack. Once executed, it pops the table off the stack completely.
         * 
         * @param L 
         */
        SL_SYMBOL Table(State L);
        ~Table() = default;

        SL_SYMBOL const Data& getRaw(const std::string& name) const;

        template<typename T>
        SL_SYMBOL void each(std::function<void(uint32_t, T&)> lambda);

        template<typename T>
        SL_SYMBOL void each(std::function<void(uint32_t, const T&)> lambda) const;

        template<typename T>
        SL_SYMBOL void try_get(const std::string& name, std::function<void(T&)> lambda, std::optional<std::function<void()>> if_not = std::nullopt);

        template<typename T>
        SL_SYMBOL void try_get(const std::string& name, std::function<void(const T&)> lambda, std::optional<std::function<void()>> if_not = std::nullopt) const;

        /**
         * @brief Make the entries equivalent to another table
         * @param table Table to get the entries from
         */
        SL_SYMBOL void fromTable(const Table& table);

        /**
         * @brief Copy the entries from a table into this table
         * @param table Table to get the entries from
         */
        SL_SYMBOL void superimpose(const Table& table);

        /**
         * @brief Copy the entries from a map into this table
         * @param map Map to get the entries from
         */
        SL_SYMBOL void superimpose(const Map& map);

        SL_SYMBOL bool hasValue(const std::string& name) const;

        /**
         * @brief Get a reference to a value in the table
         * @tparam T   Type of the value (with SL:: prefix)
         * @param name The value's key
         * @return T&  The reference to the value
         */
        template<typename T>
        SL_SYMBOL T& get(const std::string& name);
        
        template<typename T>
        SL_SYMBOL std::vector<T> get() const;

        /**
         * @brief Get a constant reference to a value in the table
         * @tparam T   Type of the value (with SL:: prefix)
         * @param name The value's key
         * @return const T& The constant reference to the value
         */
        template<typename T>
        SL_SYMBOL const T& get(const std::string& name) const;
        SL_SYMBOL void* get(const std::string& name) const;

        /**
         * @brief Set the value associated with a key
         * @tparam T The type of the value (with SL:: prefix)
         * @param name  The key
         * @param value The value
         */
        template<typename T>
        SL_SYMBOL void set(const std::string& name, const T& value);
        SL_SYMBOL void set(const std::string& name, void* value);

        /**
         * @brief Get the raw mapping of this table
         * @return const Map& The map in this table
         */
        SL_SYMBOL const Map& getMap() const;

        /**
         * @brief Dump all the map information onto the given Lua stack
         * @param L The Lua state to dump the table onto
         */
        SL_SYMBOL void toStack(State L) const;

        SL_SYMBOL void fromStack(State L);

        SL_SYMBOL std::string toString(uint32_t indent = 0) const;
    
    private:
        Map dictionary;
    };
}
