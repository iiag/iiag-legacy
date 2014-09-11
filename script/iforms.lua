-----------------------
-- script/iforms.lua --
-----------------------

iform {
	iclass = 'ore';
	material = 'iron';
	tile = 21;
	weight = 2500;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'slag';
	tile = 21;
	weight = 2500;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'quaz';
	tile = 21;
	weight = 2500;
	modify_ac = 3;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'tin';
	tile = 21;
	weight = 1500;
	modify_ac = 1;
	modify_attack = 2;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'lead';
	tile = 21;
	weight = 5000;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ingot';
	material = 'bronze';
	char = 53;
	weight = 2500;
	modify_ac = 2;
	modify_attack = 2;
	freq = 20;
}

iform {
	iclass = 'ingot';
	material = 'brass';
	tile = 53;
	weight = 2500;
	modify_ac = 1;
	modify_attack = 2;
	freq = 20;
}

-- Foods --
iform {
	iclass = 'muffin';
	material = 'stale';
	tile = 18;
	consumable = true;
	weight = 35;
	restore_stamina = 40;
	freq = 15;
}

iform {
	iclass = 'semi egg';
	material = 'rotting';
	tile = 19;
	consumable = true;
	equipable = true;
	slot = 'head';
	restore_health = 50;
	restore_stamina = -10;
	weight = 35;
	freq = 3;
}

iform {
	iclass = 'breadstick';
	material = 'hard';
	tile = 20;
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
--function fruit(n, w, s, f)
--	iform {
--		tile = 21;
--		name = n;
--		class = 'food/fruit';
--		consumable = true;
--		weight = w;
--		restore_stamina = s;
--		freq = f;
--	}
--end

--fruit('papaya', 152, 10, 1);

-- Potions --
iform {
	iclass = 'health-potion';
	material = 'cloudy';
	tile = 22;
	class = 'potion';
	consumable = true;
	weight = 300;
	restore_health = 10;
	freq = 15;
}

-- Melee weapons --
iform {
	iclass = 'sword';
	material = 'rusty';
	tile = 23;
	equipable = true;
	weight = 2000;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 10;
}

iform {
	iclass = 'axe';
	material = 'rusty';
	tile = 24;
	name = 'axe';
	equipable = true;
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 7;
}

iform {
	iclass = 'mace';
	material = 'rusty';
	tile = 25;
	equipable = true;
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 5;
}

iform {
	iclass = 'sickle';
	material = 'rusty';
	tile = 26;
	equipable = true;
	weight = 1500;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 5;
}

iform {
	iclass = 'wepon';
	material = 'rusty';
	tile = 27;
	equipable = true;
	modify_attack = 9;
	slot = 'right-hand';
	freq = 7;
	weight = 600;
}

-- Throwing weapons --
iform {
	iclass = 'shuriken';
	material = 'rusty';
	tile = 28;
	equipable = true;
	weight = 75;
	modify_attack = 2;
	slot = 'left-hand';
	freq = 10;
	spikiness = 20;
}

-- Armor --
iform {
	iclass = 'barbute';
	material = 'rusty';
	tile = 29;
	equipable = true;
	weight = 1250;
	modify_ac = 4;
	slot = 'head';
	freq = 8;
}

iform {
	iclass = 'mail coif';
	material = 'rusty';
	tile = 30;
	equipable = true;
	weight = 1450;
	modify_ac = 3;
	slot = 'head';
	freq = 7;
}

iform {
	iclass = 'cuirass';
	material = 'rusty';
	tile = 31;
	equipable = true;
	weight = 5000;
	modify_ac = 9;
	slot = 'torso';
	freq = 4;
}

iform {
	iclass = 'hauberk';
	material = 'rusty';
	tile = 32;
	equipable = true;
	weight = 3000;
	modify_ac = 7;
	slot = 'torso';
	freq = 4;
}

iform {
	iclass = 'greaves';
	material = 'rusty';
	tile = 33;
	equipable = true;
	weight = 4000;
	modify_ac = 7;
	slot = 'legs';
	freq = 5;
}

iform {
	iclass = 'boots';
	material = 'rusty';
	tile = 34;
	equipable = true;
	weight = 4500;
	modify_ac = 14;
	slot = 'feet';
	freq = 6;
}

-- To be categorized --
iform {
	iclass = 'cape';
	material = 'magic';
	tile = 35;
	equipable = true;
	weight = 750;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 1;
}
