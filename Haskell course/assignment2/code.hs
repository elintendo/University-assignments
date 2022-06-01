{-# LANGUAGE OverloadedStrings #-}
module Project where

import CodeWorld

-- These are fundamental datatypes which will be used further.
data DoorColor = Blue | Red | Purple
data Tile = Wall | Floor | Door DoorColor | DoorOpen DoorColor| Exit | SkullFloor
data Direction = South | North | West | East

type Coords = (Int, Int)

squareSize :: Double
squareSize = 1
-- This is the main character of the game (prince!)
character :: Picture
character = scaled squareSize squareSize $ lettering "\x1F934"

-- The princess whom prince is trying to save.
princess :: Picture
princess = scaled squareSize squareSize $ lettering "\x1F478"

-- Simple boolean functions that will help to determine whether the character can move in a given tile.
isWall :: Tile -> Bool
isWall Wall = True
isWall _    = False

isDoor :: Tile -> Bool
isDoor (Door Red)    = True
isDoor (Door Blue)   = True
isDoor (Door Purple) = True
isDoor _             = False

-- The function takes coordinates and a "getTile" function (which updates when the prince opens a door)
-- and returns "false" whether the tile is wall or door.
canMove :: Coords -> (Coords ->Tile) -> Bool
canMove coord f 
  | isWall $ f coord = False  
  | isDoor $ f coord = False  
  | otherwise        = True

-- The function takes a direction, current coordinates and a "getTile" function and returns new coordinates
-- if the prince can go to a tile and returns the same coords if the prince cannot.
moveAt :: Direction -> Coords -> (Coords -> Tile) -> Coords
moveAt dir coord f = if canMove coord' f then coord' else coord
  where
    coord' = moved dir coord 
    moved South (x, y) = (x, y - 1)
    moved North (x, y) = (x, y + 1)
    moved East (x, y)  = (x + 1, y)
    moved West (x, y)  = (x - 1, y)

-- Depending on the coordinates, the function is fixing the position of walls, floors, 
-- three types of doors and doorOpens, skullFloor and Princess!
getTile :: Coords -> Tile
getTile (x, y)
  | (abs x + abs y) >= 15                        = Wall
  | (abs x + abs y) <= 3 && ((x, y) /= (0, 0))   = Door Purple
  | (x == 0 && y == 0)                           = Exit
  | (abs x == 10) || (abs y == 10)               = Wall
  | (abs x == 9) && (y == 0)                     = Door Blue
  | (x == -2) && (y == -2)                       = DoorOpen Blue
  | (x == -2) && (y == 2)                        = DoorOpen Red
  | (x == 2) && (y == 2)                         = DoorOpen Purple
  | (x == 0) && (y == 9)                         = Door Red
  | (y == 0) && ((abs x) <= 8) && ((abs x) >= 3) = Wall
  | (x == 0) && ((abs y) <= 8) && ((abs y) >= 3) = Wall
  | (x == 2) && (y == (-2) )                     = SkullFloor
  | otherwise                                    = Floor

-- The function determines the design of every element on the map.
tileToPicture :: Tile -> Picture
tileToPicture Wall              = colored black $ solidRectangle squareSize squareSize
tileToPicture Floor             = colored yellow $ solidRectangle squareSize squareSize
tileToPicture (Door Blue)       = (colored blue $ solidCircle $ squareSize / 2) <> (colored black $ solidRectangle squareSize squareSize)
tileToPicture (Door Red)        = (colored red $ solidCircle $ squareSize / 2)  <> (colored black $ solidRectangle squareSize squareSize)
tileToPicture (Door Purple)     = (colored purple $ rectangle squareSize squareSize)
tileToPicture (DoorOpen Blue)   = (colored blue $ solidCircle $ squareSize / 2) <> (colored yellow $ solidRectangle squareSize squareSize)
tileToPicture (DoorOpen Red)    = (colored red $ solidCircle $ squareSize / 2) <> (colored yellow $ solidRectangle squareSize squareSize)
tileToPicture (DoorOpen Purple) = (colored purple $ solidCircle $ squareSize / 2) <> (colored yellow $ solidRectangle squareSize squareSize)
tileToPicture Exit              = lettering "\x1F478" <> (colored yellow $ solidRectangle squareSize squareSize)
tileToPicture SkullFloor        = lettering "\x1F480" <> (colored yellow $ solidRectangle squareSize squareSize)

-- The function takes coordinates and a picture and returns the picture on a shifted position.
drawAt :: Coords -> Picture -> Picture
drawAt (x, y) = translated (fromIntegral x + squareSize) (fromIntegral y + squareSize)

-- The function takes tileToPicture and feeds all possible combinations of integers less or equal than 10.
-- Then, drawAt functions draws all elements on the map.
arr :: (Coords -> Tile) -> [Picture]
arr f  = [(\ c -> drawAt c $ tileToPicture $ f c) (x, y) | x <- [- 10 .. 10], y <- [- 10 .. 10]]

-- Returns True if both colors are same.
areSameColors :: DoorColor -> DoorColor -> Bool
areSameColors Red Red       = True
areSameColors Blue Blue     = True
areSameColors Purple Purple = True
areSameColors _ _           = False

-- If the prince activates doorOpen tile of a given color, then all doors of the same color are going to open.
-- Guard function replaces doors with floors if the colors of door and doorOpen are same. 
-- Guard does not change tile if it is not a door.
openDoors :: DoorColor -> (Coords -> Tile) -> (Coords -> Tile)
openDoors color f = guard . f
  where
    guard (Door c) = if areSameColors c color then Floor else (Door c)
    guard a = a

-- If the prince saves the princess, the game ends.
endGame :: (Coords -> Tile) -> (Coords -> Tile)
endGame f = guard . f
  where 
    guard _ = Floor

-- Game is a pair of player's coordinates and a tileToPicture function.
data Game = Game { playerCoords :: Coords
                 , f :: Coords -> Tile
                 }
-- Solution function is based on activityOf that takes three functions as arguments.
-- initialGame is a game where a player always starts (prince is at location (0,9) and getTile function is not changed yet).
-- updateGame  is a function that takes "keyPress", current game (= coords and getTile function) and returns new game.
-- drawGame is a function that draws a map, a prince and other tiles.
solution :: IO()
solution = activityOf initialGame updateGame drawGame
  where
    drawGame game = drawAt (playerCoords game) character <> pictures (arr (f game))
    initialGame = Game { playerCoords = (0, -9), f = getTile }

    updateGame :: Event -> Game -> Game
    updateGame event game = Game { playerCoords = newCoords, f = newF}
      where
        pos = playerCoords game 
        f' = f game

        newCoords = case event of
          KeyPress "Up"      -> moveAt North pos f'
          KeyPress "Down"    -> moveAt South pos f'
          KeyPress "Left"    -> moveAt West pos f'
          KeyPress "Right"   -> moveAt East pos f'
          _                  -> pos

        newF = case (f game) newCoords of
                  DoorOpen c -> openDoors c (f game)
                  Exit       -> endGame (f game)
                  _          -> f game

 -- When the prince saves the princess, everything disappears because it was just a bad dream!
main :: IO()
main = solution
  
