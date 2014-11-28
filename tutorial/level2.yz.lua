START=1
BUTTON=2
DOOR0=3
SW1=4
SW2=5
SW3=6
DOOR1=7
DOOR2=8
DOOR3=9
END=10

SW_DOOR = {
    [SW1] = {DOOR1, DOOR3},
    [SW2] = {DOOR2, DOOR3},
    [SW3] = {DOOR2}
}

lib.setStartGate( START)
lib.onInteract( END, function()
    lib.endLevel(0)
end)

lib.onInteract(BUTTON, function()
    lib.use(DOOR0)
end)

for sw,doors in pairs(SW_DOOR) do
    lib.onInteract(sw, function()
        for i,door in pairs(doors) do
            lib.use(door)
        end
    end)
end
