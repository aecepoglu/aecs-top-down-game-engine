-- SCENE 1
lib.cutClear( 0,0,0)
lib.cutWrite( 10, 10, "Welcome to YZ-01\n\n"
    .. "This project started as a platform where I could write and test AIs.\n"
    .. "Now it has got a bit bigger. This tutorial will demonstrate some of\n"
	.. "its mechanics.\n\n"
	
	.. "Press any key to continue"
    )

lib.cutRender()
lib.cutReadKey()

-- SCENE 2
lib.cutClear( 0,0,0)
lib.cutWrite( 10, 10, "Use mouse to look around\n\n"

    .. "    Press WASD to move,\n"
	.. "        or ,AOE for Dvorak users\n\n"

    .. "Click once to interact with objects\n\n\n"


    .. "Interact with the gate to end the level.\n\n"

    .. "Press any key to start the level"
    )
lib.cutRender()
lib.cutReadKey()

lib.startLevel( "1.yz.map", "level1.yz.lua", 0)
lib.startLevel( "2.yz.map", "level1.yz.lua", 0)
lib.startLevel( "3.yz.map", "3.yz.lua", 0)

lib.cutClear(0,0,0)
lib.cutWrite( 10,200, "Press 'F' or 'U' to pick up an item");
lib.cutRender()
lib.cutReadKey()

lib.startLevel("lockedDoor.yz.map", "lockedDoor.yz.lua", 0)


lib.cutClear(0,0,0)
lib.cutWrite( 10, 100, "Press Shift-\x80 to push objects")
lib.cutWrite( 10, 300, "Push boxes onto marked tiles")
lib.cutRender()
lib.cutReadKey()


while lib.startLevel( "level3.yz.map", "level3.yz.lua", 0) == 0 do
    lib.cutClear(50,0,0)
    lib.cutWrite( 10, 10, "Aww, seems like you failed to solve the puzzle.\n"
        .. "Don't get beat up over it.\n"
        .. "Go ahead and try again\n\n"
        .. "Press any key to start the same level"
        )
    lib.cutRender()
    lib.cutReadKey()
end



-- SCENE 6
lib.cutClear(0,0,0)
lib.cutWrite( 10, 10, "Of course, the main reason why I started this\n"
    .. " project was because I wanted to write and demo some AIs\n\n"
    
    .. "This next map features the AI called Hungry-Left-Turner (stolen from N)\n"
    .. "He just walks forward, and turns left if he cannot move forward\n"
    .. "If he sees you at his left, then he will come to get you, but \n"
    .. " he only sees his left\n\n"
    
    .. "Press any key to start the level"
    )
lib.cutRender()
lib.cutReadKey()
while lib.startLevel( "level5.yz.map", "level5.yz.lua", 0) == 0 do
    lib.cutClear(50,0,0)
    lib.cutWrite(10,10, "Okay... so you died\n"
        .. "Just get out of its way\n\n"
        
        .. "Remember, he can see his left in a line\n"
        .. "So just stay out of his sight, and if he sees you, escape\n"
        .. " towards his right side\n\n"
        
        .. "Here we go again!\n"
        .. "Press any key to start the level"
    )
    lib.cutRender()
    lib.cutReadKey()
end



-- SCENE 7
lib.cutClear(0,0,0)
lib.cutWrite(10,10, "I'm not going to bother showing you every single AI\n\n"
    
    .. "Press any key to continue"
    )
lib.cutRender()
lib.cutReadKey()

-- SCENE 8
lib.cutClear(0,0,0)
lib.cutWrite(10,10, "So... This is the end of the tutorial\n\n"
    
    .. "This game is freely available at\n"
    .. "http://bitbucket.org/aecepoglu/aecs-top-down-game-engine\n\n"

    .. "It also has a level-editor and is scriptable using Lua\n"
    .. "To learn more about this project, go to the link above\n\n"

    .. "Programmed by me, AEC\n"
    .. "Sprites by Furkan Dutoglu\n\n"
    
    .. "Released under TeamFromBeyond (http://teamfrombeyond.com)\n\n"

    .. "You can contact me at aecepoglu@fastmail.fm\n\n"
    
    .. "Press any key to quit"
    )
lib.cutImg(550, 195, 192, 64, "res/tfb-logo.png")
lib.cutRender()
lib.cutReadKey()
