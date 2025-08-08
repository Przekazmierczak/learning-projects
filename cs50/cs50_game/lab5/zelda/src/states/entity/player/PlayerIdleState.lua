--[[
    GD50
    Legend of Zelda

    Author: Colton Ogden
    cogden@cs50.harvard.edu
]]

PlayerIdleState = Class{__includes = EntityIdleState}

function PlayerIdleState:enter(params)
    
    -- render offset for spaced character sprite (negated in render function of state)
    self.entity.offsetY = 5
    self.entity.offsetX = 0
end

function PlayerIdleState:update(dt)
    if love.keyboard.isDown('left') or love.keyboard.isDown('right') or
       love.keyboard.isDown('up') or love.keyboard.isDown('down') then
        self.entity:changeState('walk')
    end

    if love.keyboard.wasPressed('space') then
        if self.entity.carring then
            table.insert(self.entity.thrown, self.entity.carring)

            if self.entity.direction == 'left' then
                pot_tween(self.entity, self.entity.carring, self.entity.x - 64, self.entity.y + 5, self.entity.thrown)
            elseif self.entity.direction == 'right' then
                pot_tween(self.entity, self.entity.carring, self.entity.x + 64, self.entity.y + 5, self.entity.thrown)
            elseif self.entity.direction == 'up' then
                pot_tween(self.entity, self.entity.carring, self.entity.x, self.entity.y - 64 + 11, self.entity.thrown)
            elseif self.entity.direction == 'down' then
                pot_tween(self.entity, self.entity.carring, self.entity.x, self.entity.y + 64 - 11, self.entity.thrown)
            end
            
            self.entity.carring.carried = false
            self.entity.carring.flying = true 
            self.entity.carring = false
        else
            self.entity:changeState('swing-sword')
        end
    end
end

function pot_tween(player, object, new_x, new_y, thrown)
    Timer.tween(0.2, {
        [player.carring] = { x = new_x, y = new_y }
    }):finish(function()
        object.flying = false
        table.remove(thrown, 1)
        object.removed = true
    end)
end