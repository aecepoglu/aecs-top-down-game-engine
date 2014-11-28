START=3
DOOR1=4
BUTTON2=12
DOOR2=5
SENSOR3=7
DOOR3=6
KEY4=9
DOOR4=8
DOOR5=1
FLOWER6=10
END=11

lib.setStartGate( START)

lib.onInteract( END, function()
    lib.endLevel(0)
end)

lib.onInteract( DOOR1, function()
    lib.use( DOOR1)
end)

lib.onInteract( BUTTON2, function()
    lib.use( DOOR2)
end)

lib.onInteract( DOOR2, function()
    lib.write( "\nUse the button to open the door")
end)

lib.onInteract( SENSOR3, function()
    lib.use( DOOR3)
end)


lib.onInteract( DOOR4, function()
    inventory = lib.listInventory()
    for i=0,5 do
        if inventory[i] ~= 0 then
            lib.write( "Unlocking the door")
            lib.use( DOOR4)
            return;
        end
    end
    lib.write( "\nYou need a key to open the door")
end)

lib.onInteract( DOOR5, function()
    inventory = lib.listInventory()
    for i=0,5 do
        x = inventory[i]
        if x ~= 0 then
            lib.write( "\nYour need to empty your inventory to get past this door")
            return
        end
    end
    lib.use( DOOR5)
end)
