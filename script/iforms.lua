-----------------------
-- script/iforms.lua --
-----------------------

iform {
	name = 'rock';
	char = '*';
	weight = 500;
	freq = 20;
}

-- Foods --
iform {
	name = 'muffin';
	class = 'food';
	consumable = true;
	char = '%';
	weight = 7;
	restore_stamina = 40;
	freq = 15;
}

iform {
	name = 'semi egg';
	class = 'food';
	consumable = true;
	equipable = true;
	slot = 'head';
	char = '0';
	restore_health = 50;
	restore_stamina = -10;
	weight = 5;
	freq = 3;
}

iform {
	name = 'breadstick';
	class = 'food';
	consumable = true;
	equipable = true;
	char = '|';
	weight = 3;
	restore_health = 1;
	restore_stamina = 20;
	modify_attack = 4;
	slot = 'right-hand';
	freq = 15;
}

-- Potions --
iform {
	name = 'health-potion';
	class = 'potion';
	consumable = true;
	char = '&';
	weight = 60;
	restore_health = 10;
	freq = 15;
}

-- Melee weapons --
iform {
	name = 'sword';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	char = '/';
	weight = 400;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 10;
}

iform {
	name = 'axe';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	char = '/';
	weight = 600;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 7;
}

iform {
	name = 'mace';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	char = '/';
	weight = 600;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 5;
}

iform {
	name = 'sickle';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	char = '?';
	weight = 300;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 5;
}

iform {
	name = 'whip';
	class = 'weapon/melee';
	equipable = true;
	char = '$';
	modify_attack = 9;
	slot = 'right-hand';
	freq = 7;
}

-- Throwing weapons --
iform {
	name = 'shuriken';
	class = 'weapon/throwing';
	equipable = true;
	char = '+';
	weight = 15;
	modify_attack = 2;
	slot = 'left-hand';
	freq = 10;
	spikiness = 20;
}

-- Armor --
iform {
	name = 'barbute';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = '^';
	weight = 250;
	modify_ac = 4;
	slot = 'head';
	freq = 8;
}

iform {
	name = 'mail coif';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = '^';
	weight = 290;
	modify_ac = 3;
	slot = 'head';
	freq = 7;
}

iform {
	name = 'cuirass';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = ']';
	weight = 1000;
	modify_ac = 9;
	slot = 'torso';
	freq = 4;
}

iform {
	name = 'hauberk';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = ']';
	weight = 600;
	modify_ac = 7;
	slot = 'torso';
	freq = 4;
}

iform {
	name = 'greaves';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = '}';
	weight = 800;
	modify_ac = 7;
	slot = 'legs';
	freq = 5;
}

iform {
	name = 'boots';
	class = 'armor';
	material = 'metal';
	equipable = true;
	char = '"';
	weight = 900;
	modify_ac = 14;
	slot = 'feet';
	freq = 6;
}

-- To be categorized --
iform {
	name = 'magic cape';
	equipable = true;
	char = '\\';
	weight = 150;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 1;
}
