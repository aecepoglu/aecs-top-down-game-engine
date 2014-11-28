START=1
END=5
BOX=2
DOOR=4
BUTTON=6

lib.setStartGate( START)

lib.onInteract( DOOR, function()
    if lib.objAtPos(2,1) == BOX and lib.objAtPos(3,4) == BOX then
        lib.write("\nUnlocking door")
        lib.use( DOOR)
    end
end)

lib.onInteract( BUTTON, function()
    lib.endLevel(0)
end)

lib.onInteract( END, function()
    lib.endLevel(1)
end)
