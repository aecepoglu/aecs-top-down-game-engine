START=1
END=2
MONSTER=3
SW_1=4
SW_2=5
SW_3=6

lib.setStartGate(START)

switch1_on = true
switch2_on = true
switch3_on = true

function printSwitch(x, value)
    if value then
        lib.write("switch " .. x .. " is on")
    else
        lib.write("switch " .. x .. " is off")
    end
end

lib.onInteract(START, function()
    lib.changeAIStatus(MONSTER, false)
end)

lib.onInteract( SW_1, function()
    switch1_on = not switch1_on
    lib.use( SW_1)
end)

lib.onInteract( SW_2, function()
    switch2_on = not switch2_on
    lib.use( SW_2)
end)

lib.onInteract( SW_3, function()
    switch3_on = not switch3_on
    lib.use( SW_3)
end)

lib.onInteract( END, function()
    if switch1_on or switch2_on or switch3_on then
        lib.write("\nAll of the switches must be on !")
    else
        lib.endLevel(1)
    end
end)
