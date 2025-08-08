--[[
    GD50
    Legend of Zelda

    Author: Colton Ogden
    cogden@cs50.harvard.edu
]]

GameObject = Class{}

function GameObject:init(def, x, y)
    
    -- string identifying this object type
    self.type = def.type

    self.texture = def.texture
    self.frame = def.frame or 1

    -- whether it acts as an obstacle or not
    self.solid = def.solid

    self.defaultState = def.defaultState
    self.state = self.defaultState
    self.states = def.states

    -- dimensions
    self.x = x
    self.y = y
    self.width = def.width
    self.height = def.height

    self.collidable = def.collidable
    self.consumable = def.consumable
    self.removed = false

    self.carried = false
    self.flying = false

    -- default empty collision callback
    self.onCollide = function() end
end

function GameObject:update(dt)

end

function GameObject:collides(target)
    local selfY, selfHeight = self.y + self.height / 2, self.height - self.height / 2
    
    return not (self.x + self.width < target.x or self.x > target.x + target.width or
                selfY + selfHeight < target.y or selfY > target.y + target.height)
end

function GameObject:render(adjacentOffsetX, adjacentOffsetY)
    if self.states then
        love.graphics.draw(gTextures[self.texture], gFrames[self.texture][self.states[self.state].frame],
            self.x + adjacentOffsetX, self.y + adjacentOffsetY)
    elseif self.type == "heart" then
        love.graphics.draw(gTextures[self.texture], gFrames[self.texture][self.frame],
            self.x + adjacentOffsetX + 3, self.y + adjacentOffsetY + 3, 0, 0.6)
    else
        love.graphics.draw(gTextures[self.texture], gFrames[self.texture][self.frame],
            self.x + adjacentOffsetX, self.y + adjacentOffsetY)
    end
end