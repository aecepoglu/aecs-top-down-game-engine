Scripting
===========

YZ-01 uses *lua* scripting language, *version 5.2*.  
To learn more about *lua*, please visit [lua manual](http://lua.org/manual/5.2)


On top of lua, a set of functions are available to command the engine. These are:

* [use( object-id)](#use)
* [onInteract( object-id, callback-function)](#onInteract)
* [startLevel( map-file-path, script-file-path, start-option): integer](#startLevel)
* [endLevel( level-return-value)](#endLevel)
* [setStartGate( object-id)](#setStartGate)
* [write( string)](#write)
* [clear()](#clear)
* [listInventory(): array](#listInventory)
* [onInventoryAdd( callback-function )](#onInventoryAdd)
* [onInventoryRemove( callback-function)](#onInventoryRemove)
* [setObjTextures( table )](#setObjTextures)
* [setTileTextures( table )](#setTileTextures)
* [printStack()](#printStack)

You must prefix with **lib.** to call these functions. For example:

    lib.write("Hello World!")

------------

## use( object-id) <a id="use"></a>

* object-id: id of the object(s) to use

Uses the object with given id.  

To use an object, it must have an AI set that has a use function.  
AIs with use functions are these:

* door: toggles visibility

------------

## onInteract( object-id, callback-function) <a id="onInteract"></a>

* object-id: integer: id object(s) whose interaction callbacks will be set
* callback-function: function: the function which will be called on interaction. This function takes a boolean as a parameter, that has the value of whether the object was interacted with from the front side

When object with given id is interacted with, the given callback function is called.

    function callbackFunction( isInFront)
        if isInFront
            print("object interaction from the front")
        else
            print("object interaction, not from the front")
        end
    end
    lib.setTriger( 5, callbackFunction)

------------

## startLevel( map-file-path, script-file-path, start-option) : integer <a id="startLevel"></a>

* map-file-path: string: path of the level map file
* script-file-path: string: path of the level script file
* start-option: integer: values to pass to the *init function* of the script-file. A level script file can optionally have a function named "init", that takes a single integer parameter.

Loads the map at given path, and the loads the script at given path.  
If the script has a function named "init" in it, then that function is called with the parameter *start-option*

### Return Value

This function returns the value of parameter given to [endLevel function](#endLevel)

    -- this is the scenario script file
    levelResult = lib.startLevel( "test-map.yz.map", "test-map.yz.lua", 17)
    -- levelResult will be 32

    -- this is the map script file
    function init( startOption)
        -- startOption will have the value 17
        print("map init function called with option value " .. startOption)
    end
    ...
    lib.endLevel( 32)

------------

## endLevel( level-end-value) <a id="endLevel"></a>

* level-end-value: integer: value returned to [startLevel function](#startLevel)

This function ends the current level.  
Value of level-end-value is going to be returned as the result of *startLevel* function

------------

## setStartGate( object-id) <a id="setStartGate"></a>

* object-id: integer: id of starting gate

When starting the level, player will start from infront of the object with given id

------------

## write( string) <a id="write"></a>

* string: string: the string that will be written

Writes the given string to the in-game console

The console is 5 rows high and 40 columns wide. It is placed on the top-left corner of the game screen.

------------

## clear() <a id="clear"></a>

Clears the console

------------

## listInventory() : array <a id="listInventory"></a>

Lists player's inventory.

This returns an array of size 6, consisting of ids of objects inventory

## onInventoryAdd( callback ) <a id="onInventoryAdd"></a>

* callback: function: the callback function. Takes two parameters:
  1. id of item picked
  2. index of where the item is put in the inventory

sets the callback function that will be called when player adds an item to inventory.

    lib.onInventoryAdd( function( itemId, index)
        print("Picked item with id " .. itemId .. " and put it at position " .. index .. " in the inventory")
    end)

## onInventoryRemove( callback ) <a id="onInventoryRemove"></a>

* callback: function: the ballback function. Takes two parameters:
  1. id of item dropped
  2. index of where the item was in the inventory

sets the callback function that will be called when player drops an item from his inventory.

    lib.onInventoryRemove( function( itemId, index)
        print("Dropped item with id " .. itemId .. "and from position " .. index .. " of inventory")
    end)

------------

## setObjTextures( table ) <a id="setObjTextures"></a>

* table: a table consist of integer-string pairs. The integer is the type of object whose texture will be set, and string is the path of texture-sheet.

Overwrites textures of given objects with texture-sheets read from the given paths.  
The new sheets must have as-many states( rows) as the older sheets, else it will print an error and move on.

    lib.setObjTextures( {0="newPlayer.png", 1="newMonster.png"})

## setTileTextures( table ) <a id="setTileTextures"></a>

* table: a table consist of integer-string pairs.

Works exactly the same as [setObjTextures](#setObjTextures), but for terrain textures.

    lib.setTileTextures( {1="newTile.png", 2="newWall.png"})

------------

## printStack() <a id="printStack"></a>

This is temporarily available, used for debugging purposes
