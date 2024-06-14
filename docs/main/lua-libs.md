# Lua C++ Libraries {#cpplibs}
Often times you may want to represent an object that contains methods and data-members in Lua. We can do this using the `SL::Table`, where the methods are functions and the data-members are anything else. `SL::Lib::Base` helps us with this process. We can create a C++ library that is passable to Lua as a table like so
~~~~~~{.cpp}
struct ExampleLib : SL::Lib::Base
{
    ExampleLib() : Base("ExampleLib",
        {
            { "run",        run        },
            { "printValue", printValue }
        })
    {   }

    static int run(SL::State state)
    {
        using SL::CompileTime;
        TypeMap<SL::String>::push(state, "Hello");
        return 1;
    }

    static int printValue(SL::State state)
    {
        using SL::CompileTime;
        const auto [ obj ] = extractArgs<SL::Table>(state);
        std::cout << obj.get<SL::String>("value") << "\n";
        return 0;
    }
};
~~~~~~
Then, we can construct a Lua function to consume this object
~~~~~~{.lua}
function ExampleObj(obj)
    obj:printValue()
    obj.value = obj:run()
    obj:printValue()
    return obj
end
~~~~~~
The `:` syntax means we're calling the function stored at `printValue` and passing the `obj` in as the first argument (a lot like the `self` syntax from Python). So, we can call this Lua function from C++ like
~~~~~~{.cpp}

ExampleLib lib;
auto table = lib.toTable();
table.set("value", "Hi");

const auto res = runtime.runFunction<SL::Table>("ExampleObj", table);
SL_ASSERT(res, "Error running function: " << res.error().message());
std::cout << std::get<0>(*res).get<SL::String>("value") << "\n";
~~~~~~
This prints 
```
Hi
Hello
Hello
```

## Serializing
The primary utility of this struct is that commonly we have structures in C++ that we want to expose to Lua scripts which in turn call back to C++ in order to get values or modify members. This is typically done by writing a library like `ExampleLib` [above](@ref cpplibs), but adding a `void*` member that points to the object you're modifying, or is a `int64_t` id that you use in an id system (like [an ECS](https://github.com/SanderMertens/flecs)). This kind of work flow could occur as follows.

Firstly, we have our data structure
~~~~~~{.cpp}
struct Object
{
    struct 
    {
        float x, y;
    } position;
    float rotation;
};
~~~~~~
Then we have a library for this object
~~~~~~{.cpp}
struct ObjectLib : SL::Lib::Base
{
    ObjectLib() : Base("ObjLib",
        {
            { "getRadius", getRadius }
        })
    {   }

    static int getRadius(SL::State state)
    {
        using SL::CompileTime;
        const auto [ obj ] = extractArgs<SL::Table>(state);
        const auto* real_obj = static_cast<Object*>(obj.get<void*>("ptr"));
        TypeMap<SL::Number>::push(state,
            std::sqrtf(powf(real_obj->position.x, 2) + powf(real_obj->position.y, 2));
        );
        return 1;
    }
};
~~~~~~
Now we can write a Lua function that takes this object
~~~~~~{.lua}
function GetRadius(obj)
    print(obj:getRadius())
end
~~~~~~
Then we construct the object and call this Lua function
~~~~~~{.cpp}
ObjectLib lib;
auto table = lib.toTable();

Object obj;
obj.position.x = 1,
table.set<void*>("ptr", &obj);

const auto res = runtime.runFunction<SL::Table>("GetRadius", table);
assert(res);
~~~~~~
This prints `1`. 

@note
This may seem counter intuitive, but the purpose is broader. If you have an [an ECS](https://github.com/SanderMertens/flecs), you can construct these tables as functionality associated with an entity ID. Then, the C++ functions can manipulate the actual data, while the logical flow happens in the Lua script. An example of this kind of functionality is this basic movement script
~~~~~{.lua}
function Start(world, entity)
    local rigidbody = entity:getComponent(Component.Rigidbody)

    rigidbody.linearDrag = 10.0

    entity:setComponent(rigidbody)
end

function Update(world, entity)
    local force = {
        x = 0, y = 0
    }

    if Input.getDown("D") then
        force.x = force.x + 1
    end

    if (Input.getDown("A")) then
        force.x = force.x - 1
    end

    if (Input.getDown("S")) then
        force.y = force.y + 1
    end

    if (Input.getDown("W")) then
        force.y = force.y - 1
    end

    local rigidbody = entity:getComponent(Component.Rigidbody)

    local magnitude = 4000
    force = Math.normalize(force)
    force.x = force.x * magnitude
    force.y = force.y * magnitude

    rigidbody.addedForce.x = force.x;
    rigidbody.addedForce.y = force.y;
    
    entity:setComponent(rigidbody)
end
~~~~~