--[[
    GD50
    Super Mario Bros. Remake

    -- LevelMaker Class --

    Author: Colton Ogden
    cogden@cs50.harvard.edu
]]

LevelMaker = Class{}

function LevelMaker.generate(width, height)
    local tiles = {}
    local entities = {}
    local objects = {}

    local tileID = TILE_ID_GROUND
    
    -- whether we should draw our tiles with toppers
    local topper = true
    local tileset = math.random(20)
    local topperset = math.random(20)

    local key_location = math.random(3, width - 2)
    local key_color = math.random(4)
    local lock_location
    local lock_object
    repeat
        lock_location = math.random(3, width - 2)
    until lock_location ~= key_location

    -- insert blank tables into tiles for later access
    for x = 1, height do
        table.insert(tiles, {})
    end

    -- first block
    for y = 1, 6 do
        table.insert(tiles[y],
            Tile(1, y, TILE_ID_EMPTY, nil, tileset, topperset))
    end

    for y = 7, height do
        table.insert(tiles[y],
            Tile(1, y, TILE_ID_GROUND, y == 7 and topper or nil, tileset, topperset))
    end

    -- column by column generation instead of row; sometimes better for platformers
    for x = 2, width - 2 do
        local tileID = TILE_ID_EMPTY
        
        -- lay out the empty space
        for y = 1, 6 do
            table.insert(tiles[y],
                Tile(x, y, tileID, nil, tileset, topperset))
        end

        if x == key_location then
            tileID = TILE_ID_GROUND

            -- height at which we would spawn a potential jump block
            local blockHeight = 4

            for y = 7, height do
                table.insert(tiles[y],
                    Tile(x, y, tileID, y == 7 and topper or nil, tileset, topperset))
            end

            table.insert(objects,
                -- jump block
                GameObject {
                    texture = 'keys-and-locks',
                    x = (x - 1) * TILE_SIZE,
                    y = (blockHeight - 1) * TILE_SIZE,
                    width = 16,
                    height = 16,

                    -- make it a random variant
                    frame = key_color,
                    collidable = true,
                    consumable = true,
                    solid = false,

                    onConsume = function(player, object)
                        gSounds['pickup']:play()
                        score = score + 50
                        
                        lock_object.collidable = true
                        lock_object.consumable = true
                        lock_object.solid = false

                        lock_object.onConsume = function(player, object)
                            score = score + 50
                            spawn_flag(objects, width, height)
                        end
                    end
                }
            )
        elseif x == lock_location then
            tileID = TILE_ID_GROUND

            -- height at which we would spawn a potential jump block
            local blockHeight = 4

            for y = 7, height do
                table.insert(tiles[y],
                    Tile(x, y, tileID, y == 7 and topper or nil, tileset, topperset))
            end

            lock_object = GameObject {
                texture = 'keys-and-locks',
                x = (x - 1) * TILE_SIZE,
                y = (blockHeight - 1) * TILE_SIZE,
                width = 16,
                height = 16,

                -- make it a random variant
                frame = key_color + 4,
                collidable = true,
                hit = false,
                solid = true,

                onCollide = function(player, object)
                    gSounds['empty-block']:play()
                end
            }

            table.insert(objects, lock_object)
            
        -- chance to just be emptiness
        elseif math.random(7) == 1 then
            for y = 7, height do
                table.insert(tiles[y],
                    Tile(x, y, tileID, nil, tileset, topperset))
            end
        else
            tileID = TILE_ID_GROUND

            -- height at which we would spawn a potential jump block
            local blockHeight = 4

            for y = 7, height do
                table.insert(tiles[y],
                    Tile(x, y, tileID, y == 7 and topper or nil, tileset, topperset))
            end

            -- chance to generate a pillar'
            if math.random(8) == 1 then
                blockHeight = 2
                
                -- chance to generate bush on pillar
                if math.random(8) == 1 then
                    table.insert(objects,
                        GameObject {
                            texture = 'bushes',
                            x = (x - 1) * TILE_SIZE,
                            y = (4 - 1) * TILE_SIZE,
                            width = 16,
                            height = 16,
                            
                            -- select random frame from bush_ids whitelist, then random row for variance
                            frame = BUSH_IDS[math.random(#BUSH_IDS)] + (math.random(4) - 1) * 7,
                            collidable = false
                        }
                    )
                end
                
                -- pillar tiles
                tiles[5][x] = Tile(x, 5, tileID, topper, tileset, topperset)
                tiles[6][x] = Tile(x, 6, tileID, nil, tileset, topperset)
                tiles[7][x].topper = nil
            
            -- chance to generate bushes
            elseif math.random(8) == 1 then
                table.insert(objects,
                    GameObject {
                        texture = 'bushes',
                        x = (x - 1) * TILE_SIZE,
                        y = (6 - 1) * TILE_SIZE,
                        width = 16,
                        height = 16,
                        frame = BUSH_IDS[math.random(#BUSH_IDS)] + (math.random(4) - 1) * 7,
                        collidable = false
                    }
                )
            end

            -- chance to spawn a block
            if math.random(10) == 1 then
                table.insert(objects,

                    -- jump block
                    GameObject {
                        texture = 'jump-blocks',
                        x = (x - 1) * TILE_SIZE,
                        y = (blockHeight - 1) * TILE_SIZE,
                        width = 16,
                        height = 16,

                        -- make it a random variant
                        frame = math.random(#JUMP_BLOCKS),
                        collidable = true,
                        hit = false,
                        solid = true,

                        -- collision function takes itself
                        onCollide = function(obj)

                            -- spawn a gem if we haven't already hit the block
                            if not obj.hit then

                                -- chance to spawn gem, not guaranteed
                                if math.random(5) == 1 then

                                    -- maintain reference so we can set it to nil
                                    local gem = GameObject {
                                        texture = 'gems',
                                        x = (x - 1) * TILE_SIZE,
                                        y = (blockHeight - 1) * TILE_SIZE - 4,
                                        width = 16,
                                        height = 16,
                                        frame = math.random(#GEMS),
                                        collidable = true,
                                        consumable = true,
                                        solid = false,

                                        -- gem has its own function to add to the player's score
                                        onConsume = function(player, object)
                                            gSounds['pickup']:play()
                                            score = score + 100
                                        end
                                    }
                                    
                                    -- make the gem move up from the block and play a sound
                                    Timer.tween(0.1, {
                                        [gem] = {y = (blockHeight - 2) * TILE_SIZE}
                                    })
                                    gSounds['powerup-reveal']:play()

                                    table.insert(objects, gem)
                                end

                                obj.hit = true
                            end

                            gSounds['empty-block']:play()
                        end
                    }
                )
            end
        end
    end

    -- last block
    for x = width - 1, width do
        for y = 1, 6 do
            table.insert(tiles[y],
                Tile(x, y, TILE_ID_EMPTY, nil, tileset, topperset))
        end

        for y = 7, height do
            table.insert(tiles[y],
                Tile(x, y, TILE_ID_GROUND, y == 7 and topper or nil, tileset, topperset))
        end
    end

    local map = TileMap(width, height)
    map.tiles = tiles
    
    return GameLevel(entities, objects, map)
end

function spawn_flag(objects, width, height)
    pole_color = math.random(6)
    for x = 1, 3 do
        table.insert(objects,
            GameObject {
                texture = 'flags',
                x = (width - 2) * TILE_SIZE,
                y = (2 + x) * TILE_SIZE,
                width = 16,
                height = 16,

                -- make it a random variant
                frame = (x - 1) * 9 + pole_color,
                collidable = true,
                consumable = true,
                solid = false,

                onConsume = function(player, object)
                    length = length + 5
                    score = score + 200
                    gStateMachine:change('play')
                end
            }
        )
    end

    table.insert(objects,
        GameObject {
            texture = 'flags',
            x = (width - 2) * TILE_SIZE + 0.55 * TILE_SIZE,
            y = 3 * TILE_SIZE + 0.25 * TILE_SIZE,
            width = 16,
            height = 16,

            -- make it a random variant
            frame = 7,
            collidable = true,
            consumable = true,
            solid = false,

            onConsume = function(player, object)
                length = length + 5
                score = score + 200
                gStateMachine:change('play')
            end
        }
    )
end