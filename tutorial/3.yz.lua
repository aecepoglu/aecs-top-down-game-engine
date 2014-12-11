lib.setStartGate(1)

lib.onInteract(1, function()
	lib.write("This is the start gate. Find the other one")
end)

lib.onInteract(2, function()
    lib.endLevel(0)
end)

lib.onInteract(5, function()
	lib.write("You must turn both of the switches on")
end)

sw1_on = false
sw2_on = false

function tryToUnlock()
	if sw1_on and sw2_on then
		lib.use(5)
	end
end

lib.onInteract(3, function()
	lib.use(3)
	sw1_on = not sw1_on;
	tryToUnlock()
end)

lib.onInteract(4, function()
	lib.use(4)
	sw2_on = not sw2_on;
	tryToUnlock()
end)
