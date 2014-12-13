KEY=5
DOOR=3

lib.setStartGate(1)

lib.onInteract(2, function()
	lib.endLevel(0)
end)



lib.onInteract(DOOR, function()
	inventory = lib.listInventory()
    for i=0,5 do
        if inventory[i] ~= 0 then
            lib.write( "The door has opened")
            lib.use( DOOR)
            return;
        end
    end
	lib.write("You need a key to open this door")
end)

lib.onInventoryAdd( function( itemId, index)
	if(itemId == KEY) then
		lib.write("That's it!\nNow you can open the door.")
	end
end)