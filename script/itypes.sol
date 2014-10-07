-----------------------
-- script/itypes.lua --
-----------------------

iclass({
	class = 'any',
})

iclass({
	class = 'ore',
	type_of = 'any',
})

iclass({
	class = 'slag',
	type_of = 'any',
})

iclass({
	class = 'ingot',
	type_of = 'slag',
})

iclass({
	class = 'links',
	type_of = 'slag',
})

iclass({
	class = 'ring',
	type_of = 'slag',
})

iclass({
	class = 'blade',
	type_of = 'slag',
})

iclass({
	class = 'serrated blade',
	type_of = 'blade',
})

iclass({
	class = 'hilt',
	type_of = 'slag',
})

iclass({
	class = 'cross guard',
	type_of = 'hilt',
})

iclass({
	class = 'sinclair hilt',
	type_of = 'hilt',
})

iclass({
	class = 'shaft',
	type_of = 'slag',
})

iclass({
	class = 'plate',
	type_of = 'slag',
})

-- weapons --

iclass({
	class = 'weapon',
	type_of = 'slag',
})

iclass({
	class = 'sword',
	type_of = 'weapon',
})

iclass({
	class = 'xiphos',
	type_of = 'sword',
})

iclass({
	class = 'spatha',
	type_of = 'sword',
})

iclass({
	class = 'paramerion',
	type_of = 'sword',
})

iclass({
	class = 'estoc',
	type_of = 'sword',
})

iclass({
	class = 'flambard',
	type_of = 'sword',
})

iclass({
	class = 'schiavona',
	type_of = 'sword',
})

iclass({
	class = 'axe',
	type_of = 'weapon',
})

iclass({
	class = 'mace',
	type_of = 'weapon',
})

iclass({
	class = 'sickle',
	type_of = 'weapon',
})

iclass({
	class = 'shuriken',
	type_of = 'weapon',
})

--armour--

iclass({
	class = 'armour',
	type_of = 'slag',
})

iclass({
	class = 'cape',
	type_of = 'any',
})

iclass({
	class = 'hauberk',
	type_of = 'armour',
})

iclass({
	class = 'cuirass',
	type_of = 'armour',
})

iclass({
	class = 'coif',
	type_of = 'armour',
})

iclass({
	class = 'barbute',
	type_of = 'armour',
})

iclass({
	class = 'greaves',
	type_of = 'armour',
})

iclass({
	class = 'boots',
	type_of = 'armour',
})

-- potions --

iclass({
	class = 'potion',
	type_of = 'any',
})

iclass({
	class = 'health-potion',
	type_of = 'potion',
})

-- foods --

iclass({
	class = 'food',
	type_of = 'any',
})

iclass({
	class = 'muffin',
	type_of = 'food',
})

iclass({
	class = 'semi egg',
	type_of = 'food',
})

iclass({
	class = 'breadstick',
	type_of = 'food',
})
