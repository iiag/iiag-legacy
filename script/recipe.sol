-----------------------
-- script/recipe.lua --
-----------------------

-- ingots --
recipe({
	class = 'ingot',
	made_by = 'smelter',
	tile = 53,

})

component({
	mat = '1',
	class = 'ore',
	weight = 't',
	modify_ac = ' f + t',
	modify_attack = 'f + t',
})

-- slaging an item --
recipe({
	class = 'ore',
	made_by = 'smelter',
	mat = 'slag',
	tile = 21,
	weight = 3000,
	modify_ac = 1,
	modify_attack = 1,


})

component({
	class = 'slag',
})

-- links --
recipe({
	class = 'links',
	made_by = 'forge',
	made_with = 2,
	tile = 21,
	modify_ac = 1,
	modify_attack = 0,


})

component({
	mat = '1',
	class = 'ingot',
	weight = 't * 0.5',
	modify_ac = 't',
})

-- shaft --
recipe({
	class = 'shaft',
	made_by = 'forge',
	made_with = 4,
	tile = 20,
	modify_attack = 1,
	equipable = true,
	slot = 'right-hand',

})

component({
	mat = '1',
	class = 'ingot',
	weight = 't',
	modify_attack = 'f + t',
	modify_ac = 'f + t',
})

-- plate --
recipe({
	class = 'plate',
	made_by = 'forge',
	made_with = 6,
	tile = 19,
	modify_ac = 1,

})

component({
	mat = '1',
	class = 'ingot',
	weight = 't',
	modify_ac = 'f + t',
})

-- blade --
recipe({
	class = 'blade',
	made_by = 'forge',
	made_with = 8,
	tile = 20,
	modify_attack = 1,

})

component({
	mat = '1',
	class = 'ingot',
	weight = 't',
	modify_attack = 'f + t',
})

-- serrated blade --
recipe({
	class = 'serrated blade',
	made_by = 'forge',
	tile = 26,
	modify_attack = 1,

})

component({
	mat = '1',
	class = 'blade',
	match_type = true,
	weight = 't',
	modify_attack = 'f + t',
})

-- hilt --
recipe({
	class = 'hilt',
	made_by = 'forge',
	made_with = 5,
	tile = 18,

})

component({
	mat = '1',
	class = 'ingot',
	weight = 't * 0.75',
})


---------------------------------
--        Here be weapons      --
---------------------------------

-- cross guard --
recipe({
	class = 'cross guard',
	made_by = 'forge',
	tile = 18,
	modify_ac = 1,
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'shaft',
	modify_ac = 'f + t',
	weight = 'f + t',
})

-- sinclair hilt --
recipe({
	class = 'sinclair hilt',
	made_by = 'forge',
	tile = 18,
	modify_ac = 2,
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'plate',
	modify_ac = 'f + t',
	weight = 'f + t',
})

-- short sword --
recipe({
	class = 'xiphos',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

-- long sword --
recipe({
	class = 'spatha',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

-- paramerion --
recipe({
	class = 'paramerion',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

-- estoc --
recipe({
	class = 'estoc',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

-- flambard --
recipe({
	class = 'flambard',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'serrated blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'serrated blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

-- schiavona --
recipe({
	class = 'schiavona',
	made_by = 'forge',
	tile = 23,
	modify_attack = 2,
	equipable = true,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'hilt',
	modify_attack = 'f + t',
	modify_ac     = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'blade',
	modify_attack = 'f + t',
	weight = 'f + t',
})

component({
	class = 'ingot',
	modify_attack = 'f + t',
	weight = 'f + t',
})


-- mace --
recipe({
	class = 'mace',
	made_by = 'forge',
	equipable = true,
	tile = 23,
	weight = 3000,
	modify_attack = 7,
	slot = 'right-hand',
})

component({
	mat = '1',
	class = 'ingot',
	modify_attack = 't * 2 + f',
})

component({
	class = 'shaft',
	modify_attack = 'f + t',
})

---------------------------------
--         Here be armour      --
---------------------------------

-- coif --
recipe({
	class = 'coif',
	made_by = 'forge',
	tile = 29,
	equipable = true,
	weight = 1450,
	modify_ac = 2,
	slot = 'head',
})

component({
	mat = '1',
	class = 'links',
	modify_attack = 'f + t',
})

-- hauberk --
recipe({
	class = 'hauberk',
	made_by = 'forge',
	tile = 31,
	weight = 3000,
	modify_ac = 4,
	slot = 'torso',
})

component({
	mat = '1',
	class = 'links',
	modify_attack = 'f + t',
})

component({
	class = 'links',
	modify_attack = 'f + t',
})

