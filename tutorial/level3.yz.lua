START=1
END=5
BOX=2
DOOR=4
BUTTON=6

lib.setStartGate( START)

function tryUnlock()
    if lib.objAtPos(2,1) == BOX and lib.objAtPos(3,4) == BOX then
        lib.write("\nUnlocking door")
        lib.use( DOOR)
        return true
    else
        return false
    end
end

lib.onInteract( DOOR, tryUnlock)

lib.onInteract( BUTTON, function()
    if not tryUnlock() then
        lib.endLevel(0)
    end
end)

lib.onInteract( END, function()
    lib.endLevel(1)
end)
