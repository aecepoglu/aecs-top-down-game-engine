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
lib.cutWrite( 10, 10, "You're going to use the cursor keys to move around.\n\t\x80 to move forward\n\t\x81 to turn right, \x83 to turn left\n\t\x82 to move backwards\n\n"
    .. "Press 'u' key to interact with objects\nYou'll need to interact with the gate at the end\n of the level to finish it.\n\n"
    .. "Press any key to start the level"
    )
lib.cutRender()
lib.cutReadKey()

lib.startLevel( "level1.yz.map", "level1.yz.lua", 0)



-- SCENE 3
lib.cutClear( 0, 0, 0)
lib.cutWrite( 10, 10, "So that's how you move around the map.\n\n"
    .. "You can interact with more than just gates.\n"
    .. "This next level will have a few more\n"
    .. " interactable objects\n\n"
    .. "Press any key to start the level\n"
    )
lib.cutRender()
lib.cutReadKey()



lib.startLevel( "level2.yz.map", "level2.yz.lua", 0)



-- SCENE 4
lib.cutClear(0,0,0)
lib.cutWrite( 10, 10, "I loved playing Sokoban growing up,\n"
    .. " so I couldn't not put a similar mechanic in this-\n\n"
    .. "In the next level, you need to push the boxes onto-\n"
    .. " highlit tiles to unlock the gate\n\n"
    .. "You need to press 'SHIFT-\x80' when standing against an object to push it forward\n\n"
    .. "If you fail to solve the puzzle, press the button in the level\n\n"
    .. "Press any key to start the level"
    )
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



-- SCENE 5
lib.cutClear(0, 50, 0)
lib.cutWrite( 10, 10, "Good job solving the puzzle.\n\n"

    .. "So you can push objects, interact with them; you can also \n"
    .. "  - hit them, by trying to move onto them\n"
    .. "  - eat them, by pressing 'e' when they are dead\n"
    .. "  - pick them to inventory, by pressing 'p'\n"
    .. "  - press number keys [1-6] to drop items from inventory\n\n"

    .. "Some objects give health when you eat them, but some damage you\n"
    .. "Different objects have different amount of defence.\n"
    .. " You might not be able to destroy them all\n"
    .. "You cannot pick every object into your inventory\n\n."

    .. "This next level will have you use these mechanics\n\n"

    .. "Press any key to start the level"
    )
lib.cutRender()
lib.cutReadKey()

lib.startLevel( "level4.yz.map", "level4.yz.lua", 0)



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
