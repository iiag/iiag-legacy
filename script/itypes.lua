-----------------------
-- script/itypes.lua --
-----------------------

iclass {
	class = 'any';
}

iclass {
	class = 'ore';
	type_of = 'any';
}

iclass {
	class = 'ingot';
	type_of = 'ore';
}

iclass {
	class = 'links';
	type_of = 'ore';
}

iclass {
	class = 'ring';
	type_of = 'ore';
}

iclass {
	class = 'blade';
	type_of = 'ingot';
}

iclass {
	class = 'hilt';
	type_of = 'ingot';
}

iclass {
	class = 'shaft';
	type_of = 'ingot';
}

iclass {
	class = 'plate';
	type_of = 'ingot';
}

-- weps --

iclass {
	class = 'wepon';
	type_of = 'ore';
}

iclass {
	class = 'sword';
	type_of = 'wepon';
}

iclass {
	class = 'axe';
	type_of = 'wepon';
}

iclass {
	class = 'mace';
	type_of = 'wepon';
}

iclass {
	class = 'sickle';
	type_of = 'wepon';
}

iclass {
	class = 'shuriken';
	type_of = 'wepon';
}

--armour--

iclass {
	class = 'armour';
	type_of = 'ore';
}

iclass {
	class = 'cape';
	type_of = 'any';
}

iclass {
	class = 'hauberk';
	type_of = 'armour';
}

iclass {
	class = 'cuirass';
	type_of = 'armour';
}

iclass {
	class = 'coif';
	type_of = 'armour';
}

iclass {
	class = 'barbute';
	type_of = 'armour';
}

iclass {
	class = 'greaves';
	type_of = 'armour';
}

iclass {
	class = 'boots';
	type_of = 'armour';
}

-- potions --

iclass {
	class = 'potion';
	type_of = 'any';
}

iclass {
	class = 'health-potion';
	type_of = 'potion';
}

-- foods --

iclass {
	class = 'food';
	type_of = 'any';
}

iclass {
	class = 'muffin';
	type_of = 'food';
}

iclass {
	class = 'semi egg';
	type_of = 'food';
}

iclass {
	class = 'breadstick';
	type_of = 'food';
}
