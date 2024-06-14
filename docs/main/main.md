[TOC]

The goal is simple-lua is to provide an easy and quick interface for using Lua scripts in C++.

# Getting Started

## Building
To build the library, first clone from the repository with 
~~~~~
git clone https://github.com/maxortner01/simple-lua.git
~~~~~
Then, in your `CMakeLists.txt` add
~~~~~~{.cpp}
add_subdirectory(${PATH_TO_SIMPLE_LUA})
~~~~~~
You can expose your project to the library with
~~~~~~{.cmake}
target_link_directories(PROJECT_NAME PRIVATE simple-lua)
~~~~~~

### CMake Options
There are three important options:
 1) `-DSL_BUILD_LIB=ON` is on by default, but you can choose not to build the library (for example if you only want documentation)
 2) `-DSL_UNIT_TESTS=ON` will build the unit tests (requires the library), which includes pulling the [googletest](https://github.com/google/googletest) repository, run `ctest` to actually run the tests
 3) `-DSL_BUILD_DOCS=ON` will build the documentation, which includes pulling [doxygen-awesome](https://github.com/jothepro/doxygen-awesome-css) which is used for basic formatting

## Basic Usage
To learn the Lua scripting language, [check out this page](https://www.lua.org/start.html). Once you have a script you're ready to integrate into your C++ program (and have set up the subdirectory with cmake), all you need to do is include `#include <SL/Lua.hpp>` at the top of your file. 

The lifetime of a Lua script is `SL::Runtime`, which can be created by
~~~~~~{.cpp}
SL::Runtime runtime("[[PATH TO LUA SCRIPT]]");
assert(runtime);
~~~~~~
You can also create a runtime, specifying `SL::Lib::Base` libraries with (more on this [here](@ref cpplibs))
~~~~~~{.cpp}
auto runtime = SL::Runtime::create<LibA, LibB>("[[PATH TO LUA SCRIPT]]");
assert(runtime);
~~~~~~

### Error Checking
All types returned from `SL::Runtime` methods are `SL::Results` of some kind. These objects can be casted to bool (or called `good()`) to determine if an error occured. If it does return false you can print the error message. Utilizing the `SL_ASSERT` macro, we can check with
~~~~~~{.cpp}
SL_ASSERT(runtime, "An error occured creating the runtime: " << res.error().message());
~~~~~~

### Supported Types {#supportedtypes}
It is advised that when getting values from an `SL::Runtime` or `SL::Table` that you only use the types described in `TypeMap.hpp`, that is
 - `SL::String` is `std::string`
 - `SL::Number` is `float`
 - `SL::Boolean` is `bool`
 - `SL::Function` is `int (*)(SL::State)`

@warning
The getting/setting of a `void*` pointer is also technically supported, but not super well. It is not meant to be used in a way that involves accessing data from Lua, but instead for the case of serializing the an object an retaining its identity by storing its location in memory so that you can deserialize it and access it from C++. More about this workflow is [talked about later](@ref serializing)

### Globals
Globals are values in the global scope in a Lua script like
~~~~~~{.lua}
GlobalValue = 4.0
~~~~~~
You can access the value with
~~~~~~{.cpp}
const SL::Runtime::Result<SL::Number> res = runtime.getGlobal<SL::Number>("GlobalValue");
assert(res);
std::cout << *res << "\n";
~~~~~~
prints `4`. 

@note
The types can get rather verbose with the nested `struct`s and `namespace`s, so it's common to simply do
~~~~~~{.cpp}
const auto res = runtime.getGlobal<SL::Number>("GlobalValue");
~~~~~~

### Lua Functions
We can call functions in the global scope like
~~~~~~{.lua}
function GetNumber()
    return 42
end
~~~~~~
by calling the `SL::Runtime::runFunction` method. This method returns an `std::tuple` of the types specified in the call to `runFunction` which are the return types of the Lua function. The types *have* to match in type and count, otherwise you'll get errors. The return of this function is itself a result, so you can capture the errors and do what you need with them. To execute this Lua function we'd write
~~~~~~{.cpp}
const auto res = runtime.runFunction<SL::Number>("GetNumber");
SL_ASSERT(res, "Error running function: " << res.error().message());
std::cout << std::get<0>(*res) << "\n";
~~~~~~
This prints `42`.

### C++ Functions from Lua
We can easily put C++ functions in the global scope of a Lua script easily using the `SL::Runtime::registerFunction` method. Functions implemented in C++ must have a specific signature: that of `SL::Function` (and *must* be static). We can create one of these functions 
~~~~~~{.cpp}
static int cppFunction(SL::State state)
{
    ...
}
~~~~~~
### Extracting Arguments
Lua works using a stack, when a function is called the arguments are pushed to the stack. So, to get the arguments passed from Lua, we simply pop the expected types off the top of the stack. The `SL::Lib::Base::extractArgs` method is helpful for this reason (to avoid the verbosity, we can have our method be a static one inside a struct inheriting from SL::Lib::Base). Then, we run our function. Finally, we push onto the stack the returns and return from the C++ function the count of returned types.
~~~~~~{.cpp}
static int cppFunction(SL::State state)
{
    using namespace SL::CompileTime;
    // Extract the arguments passed from Lua
    const auto [ number ] = extractArgs<SL::Number>(state);
    // Push the return (just adding two to what's passed in)
    TypeMap<SL::Number>::push(state, number + 2);
    // Tell Lua we returned one item
    return 1;
}
~~~~~~
We can put this in the global scope of a Lua runtime with
~~~~~~{.cpp}
const auto res = runtime.registerFunction("Global", "cppFunction", cppFunction);
SL_ASSERT(res, "Error registering function: " << res.error().message());
~~~~~~

@note
Really what this is doing is creating a table in the global scope and inserting the C++ function as a member of it. The name of the table is the first argument and the name of the function the second. Creating a table whose entries are functions is exactly what `SL::Lib::Base` handles, which is [talked about later](@ref cpplibs).

Then, in a Lua function:
~~~~~~{.lua}
function TestCpp(num)
    return Global.cppFunction(num)
end
~~~~~~
Finally, we run the Lua function:
~~~~~~{.cpp}
const auto res = runtime.runFunction<SL::Number>("TestCpp", 4);
SL_ASSERT(res, "Error running function: " << res.error().message());
SL_ASSERT(std::get<0>(*res) == 6, "Something went wrong");
~~~~~~

### Tables
The only other type missing from the [supported types](@ref supportedtypes) is `SL::Table` which will more than likely be the most commonly used type. 