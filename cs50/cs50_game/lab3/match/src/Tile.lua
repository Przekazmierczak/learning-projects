--[[
    GD50
    Match-3 Remake

    -- Tile Class --

    Author: Colton Ogden
    cogden@cs50.harvard.edu

    The individual tiles that make up our game board. Each Tile can have a
    color and a variety, with the varietes adding extra points to the matches.
]]

Tile = Class{}

function Tile:init(x, y, color, variety, special)
    
    -- board positions
    self.gridX = x
    self.gridY = y

    -- coordinate positions
    self.x = (self.gridX - 1) * 32
    self.y = (self.gridY - 1) * 32

    -- tile appearance/points
    self.color = color
    self.variety = variety

    self.transparency = 1
    self.special = special

    if self.special then
        self.brighter = false
        self.transparentTimer = Timer.every(0.03, function()
            
            -- shift every color to the next, looping the last to front
            -- assign it to 0 so the loop below moves it to 1, default start
            if self.brighter then
                self.transparency = self.transparency + 0.05
            else
                self.transparency = self.transparency - 0.05
            end

            if self.transparency < 0.8 then
                self.brighter = true
            elseif self.transparency > 1 then
                self.brighter = false
            end
        end)
    end
end

function Tile:render(x, y)
    
    -- draw shadow
    love.graphics.setColor(0, 0, 0, 1)
    love.graphics.draw(gTextures['main'], gFrames['tiles'][self.color][self.variety],
        self.x + x + 2, self.y + y + 2)

    -- draw tile itself
    love.graphics.setColor(self.transparency, self.transparency, self.transparency, self.transparency)
    love.graphics.draw(gTextures['main'], gFrames['tiles'][self.color][self.variety],
        self.x + x, self.y + y)
end