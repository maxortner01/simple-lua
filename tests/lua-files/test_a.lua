TestTable = {
    name = "Test",
    sub = { number = 4.5 }
}

function AddTwo(val) 
    return val + 2
end

function CallGlobalFunction(val)
    return Global.CppAddTwo(val)
end

function TestLib(obj)
    obj.value = obj:addToValue(2.0)
    return obj
end