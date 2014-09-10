-----------------------
-- script/iforms.lua --
-----------------------

-- Foods --
iform {
	tile = 18;
	name = 'muffin';
	class = 'food';
	material = 'muffin';
	consumable = true;
	weight = 35;
	restore_stamina = 40;
	freq = 15;
}

iform {
	tile = 19;
	name = 'semi egg';
	class = 'food';
	consumable = true;
	equipable = true;
	slot = 'head';
	restore_health = 50;
	restore_stamina = -10;
	weight = 35;
	freq = 3;
}

iform {
	tile = 20;
	name = 'breadstick';
	class = 'food';
	consumable = true;
	equipable = true;
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
		tile = 21;
		name = n;
		class = 'food/fruit';
		consumable = true;
		weight = w;
		restore_stamina = s;
		freq = f;
	}
end

fruit('papaya', 152, 10, 1);

-- Potions --
iform {
	tile = 22;
	name = 'health-potion';
	class = 'potion';
	consumable = true;
	weight = 300;
	restore_health = 10;
	freq = 15;
}

-- Melee weapons --
iform {
	tile = 23;
	name = 'sword';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	weight = 2000;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 10;
}

iform {
	tile = 24;
	name = 'axe';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 7;
}

iform {
	tile = 25;
	name = 'mace';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 5;
}

iform {
	tile = 26;
	name = 'sickle';
	class = 'weapon/melee';
	material = 'metal';
	equipable = true;
	weight = 1500;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 5;
}

iform {
	tile = 27;
	name = 'whip';
	class = 'weapon/melee';
	equipable = true;
	modify_attack = 9;
	slot = 'right-hand';
	freq = 7;
	weight = 600;
}

-- Throwing weapons --
iform {
	tile = 28;
	name = 'shuriken';
	class = 'weapon/throwing';
	equipable = true;
	weight = 75;
	modify_attack = 2;
	slot = 'left-hand';
	freq = 10;
	spikiness = 20;
}

-- Armor --
iform {
	tile = 29;
	name = 'barbute';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 1250;
	modify_ac = 4;
	slot = 'head';
	freq = 8;
}

iform {
	tile = 30;
	name = 'mail coif';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 1450;
	modify_ac = 3;
	slot = 'head';
	freq = 7;
}

iform {
	tile = 31;
	name = 'cuirass';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 5000;
	modify_ac = 9;
	slot = 'torso';
	freq = 4;
}

iform {
	tile = 32;
	name = 'hauberk';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 3000;
	modify_ac = 7;
	slot = 'torso';
	freq = 4;
}

iform {
	tile = 33;
	name = 'greaves';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 4000;
	modify_ac = 7;
	slot = 'legs';
	freq = 5;
}

iform {
	tile = 34;
	name = 'boots';
	class = 'armor';
	material = 'metal';
	equipable = true;
	weight = 4500;
	modify_ac = 14;
	slot = 'feet';
	freq = 6;
}

-- To be categorized --
iform {
	tile = 35;
	name = 'magic cape';
	equipable = true;
	weight = 750;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 1;
}
