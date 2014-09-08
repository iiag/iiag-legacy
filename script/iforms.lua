-----------------------
-- script/iforms.lua --
-----------------------

-- Foods --
iform {
	name = 'muffin';
	class = 'food';
	material = 'muffin';
	consumable = true;
	char = '%';
	weight = 35;
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
	weight = 35;
	freq = 3;
}

iform {
	name = 'breadstick';
	class = 'food';
	consumable = true;
	equipable = true;
	char = '|';
	weight = 15;
	restore_health = 1;
	restore_stamina = 20;
	modify_attack = 4;
	slot = 'right-hand';
	freq = 15;
}

-- Fruit --
function fruit(n, w, s, f)
	iform {
		name = n;
		class = 'food/fruit';
		consumable = true;
		char = '*';
		weight = w;
		restore_stamina = s;
		freq = f;
	}
end

fruit('papaya', 152, 10, 1);

-- Potions --
iform {
	name = 'health-potion';
	class = 'potion';
	consumable = true;
	char = '&';
	weight = 300;
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
	weight = 2000;
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
	weight = 3000;
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
	weight = 3000;
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
	weight = 1500;
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
	weight = 600;
}

-- Throwing weapons --
iform {
	name = 'shuriken';
	class = 'weapon/throwing';
	equipable = true;
	char = '+';
	weight = 75;
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
	weight = 1250;
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
	weight = 1450;
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
	weight = 5000;
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
	weight = 3000;
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
	weight = 4000;
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
	weight = 4500;
	modify_ac = 14;
	slot = 'feet';
	freq = 6;
}

-- To be categorized --
iform {
	name = 'magic cape';
	equipable = true;
	char = '\\';
	weight = 750;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 1;
}
