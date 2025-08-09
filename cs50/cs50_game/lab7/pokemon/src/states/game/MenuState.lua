--[[
    GD50
    Pokemon

    Author: Colton Ogden
    cogden@cs50.harvard.edu
]]

MenuState = Class{__includes = BaseState}

function MenuState:init(pokemon, hp, att, def, speed)
    -- self.battleState = battleState
    
    self.Menu = Menu {
        x = 64,
        y = 5,
        width = VIRTUAL_WIDTH / 1.5,
        height = VIRTUAL_HEIGHT / 1.5,
        items = {
            {
                text = "HP: " .. (pokemon.HP - hp) .. " + " .. hp .. " = " .. pokemon.HP,
                onSelect = function()
                    gStateStack:pop()
                end
            },
            {
                text = "Attack: " .. (pokemon.attack - att) .. " + " .. att .. " = " .. pokemon.attack,
            },
            {
                text = "Defense: " .. (pokemon.defense - def) .. " + " .. def .. " = " .. pokemon.defense,
            },
            {
                text = "Speed: " .. (pokemon.speed - speed) .. " + " .. speed .. " = " .. pokemon.speed,
            },
        },
        cursor = false
    }
end

function MenuState:update(dt)
    self.Menu:update(dt)
end

function MenuState:render()
    self.Menu:render()
end