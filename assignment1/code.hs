-- | Assignment 1, exercise №1 Traffic Lights
-- | Ivan Orekhov

{-# LANGUAGE OverloadedStrings #-}
import CodeWorld

-- | Frame for traffic lights of cars
frame1 :: Picture
frame1 = rectangle 2.5 7

-- | Frame for traffic lights of pedestrians and cyclists
frame2 :: Picture
frame2 = rectangle 2.5 5

-- | A circle for traffic lights given color
lightCircle :: Color -> Picture
lightCircle c = colored c (solidCircle 1)

-- | Green and gray traffic light with given Y-offset
greenLight :: Double -> Picture
greenLight y = translated 0 y (lightCircle green)

noGreenLight :: Double -> Picture
noGreenLight y = translated 0 y (lightCircle gray)

-- | Red and gray traffic light with given Y-offset
redLight :: Double -> Picture
redLight y = translated 0 y (lightCircle red)

noRedLight :: Double -> Picture
noRedLight y = translated 0 y (lightCircle gray)

-- | Yellow and gray traffic light with given Y-offset
yellowLight :: Double -> Picture
yellowLight y = translated 0 y (lightCircle yellow)

noYellowLight :: Double -> Picture
noYellowLight y = translated 0 y (lightCircle gray)

-- | The traffic lights for cars. It has four states according to the assignment.
trafficLight1 :: Int -> Picture
trafficLight1 1 = frame1 <> greenLight (-2.2)   <> noRedLight 2.2 <> noYellowLight 0
trafficLight1 2 = frame1 <> noGreenLight (-2.2) <> noRedLight 2.2 <> yellowLight 0
trafficLight1 3 = frame1 <> noGreenLight (-2.2) <> redLight 2.2   <> noYellowLight 0
trafficLight1 4 = frame1 <> noGreenLight (-2.2) <> redLight 2.2   <> yellowLight 0

-- | The traffic lights for pedestrians with three states
trafficLight2 :: Int -> Picture
trafficLight2 1 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B6") <> greenLight (-1.2) <> noRedLight 1.2
trafficLight2 2 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B6") <> noGreenLight (-1.2) <> noRedLight 1.2
trafficLight2 3 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B6") <> noGreenLight (-1.2) <> redLight 1.2

-- | The traffic lights for cyclists (similar to previous one) with three states
trafficLight3 :: Int -> Picture
trafficLight3 1 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B2") <> greenLight (-1.2) <> noRedLight 1.2
trafficLight3 2 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B2") <> noGreenLight (-1.2) <> noRedLight 1.2
trafficLight3 3 = frame2 <> translated 0 (-1.2) (lettering "\x1F6B2") <> noGreenLight (-1.2) <> redLight 1.2

-- | The final composition of all three traffic lights with corresponding coordinates
finalPicture :: Picture -> Picture -> Picture -> Picture
finalPicture p1 p2 p3 = translated (-3) 0 p1 <> translated 0 0 p2 <> translated 3 0 p3

-- | trafficController function displays traffic lights' colors according to the time. 
-- | [0; 3) seconds   -- trafficLight1, trafficLight2 and trafficLight3 shows green color
-- | [3; 3.5) seconds -- trafficLight1 shows yellow color, trafficLight2 and trafficLight3 show gray colors (they start "blinking")
-- | [3.5; 4) seconds -- trafficLight1 still shows yellow, trafficLight2 and trafficLight3 show green colors (continue blinking)
-- | [4; 7) seconds   -- all trafficLights show red light
-- | then this cycle repeats over and over.

trafficController :: Double -> Picture
trafficController t
  | (floor t `mod` 8) < 3                            = finalPicture (trafficLight1 1) (trafficLight2 1) (trafficLight3 1)
  | (floor t `mod` 8) >= 3 && (fromIntegral(floor t `mod` 8) + (t - fromIntegral(floor t)) ) < 3.5  = finalPicture (trafficLight1 2) (trafficLight2 2) (trafficLight3 2)
  | (fromIntegral(floor t `mod` 8) + (t - fromIntegral(floor t)) ) < 4 && (fromIntegral(floor t `mod` 8) + (t - fromIntegral(floor t)) ) >= 3.5  = finalPicture (trafficLight1 2) (trafficLight2 1) (trafficLight3 1)
  | (floor t `mod` 8) >= 4 && (floor t `mod` 8) < 7  = finalPicture (trafficLight1 3) (trafficLight2 3) (trafficLight3 3)
  | otherwise                                        = finalPicture (trafficLight1 4) (trafficLight2 3) (trafficLight3 3)

-- | Solution1 animates all trafficlights according to time.
solution1 :: IO()
solution1 = animationOf trafficController

main :: IO ()
main = solution1
