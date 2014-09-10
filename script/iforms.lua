-----------------------
-- script/iforms.lua --
-----------------------

iform {
	iclass = 'ore';
	material = 'iron';
	char = '*';
	weight = 2500;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'slag';
	char = '*';
	weight = 2500;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'quaz';
	char = '*';
	weight = 2500;
	modify_ac = 3;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'tin';
	char = '*';
	weight = 1500;
	modify_ac = 1;
	modify_attack = 2;
	freq = 20;
}

iform {
	iclass = 'ore';
	material = 'lead';
	char = '*';
	weight = 5000;
	modify_ac = 1;
	modify_attack = 1;
	freq = 20;
}

iform {
	iclass = 'ingot';
	material = 'bronze';
	char = '-';
	weight = 2500;
	modify_ac = 2;
	modify_attack = 2;
	freq = 20;
}

iform {
	iclass = 'ingot';
	material = 'brass';
	char = '-';
	weight = 2500;
	modify_ac = 1;
	modify_attack = 2;
	freq = 20;
}

-- Foods --
iform {
	iclass = 'muffin';
	material = 'stale';
	consumable = true;
	char = '%';
	weight = 35;
	restore_stamina = 40;
	freq = 15;
}

iform {
	iclass = 'semi egg';
	material = 'rotting';
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
	iclass = 'breadstick';
	material = 'hard';
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

-- Potions --
iform {
	iclass = 'health-potion';
	material = 'cloudy';
	class = 'potion';
	consumable = true;
	char = '&';
	weight = 300;
	restore_health = 10;
	freq = 15;
}

-- Melee weapons --
iform {
	iclass = 'sword';
	material = 'rusty';
	equipable = true;
	char = '/';
	weight = 2000;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 10;
}

iform {
	iclass = 'axe';
	material = 'rusty';
	equipable = true;
	char = '/';
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 7;
}

iform {
	iclass = 'mace';
	material = 'rusty';
	equipable = true;
	char = '/';
	weight = 3000;
	modify_attack = 7;
	slot = 'right-hand';
	freq = 5;
}

iform {
	iclass = 'sickle';
	material = 'rusty';
	equipable = true;
	char = '?';
	weight = 1500;
	modify_attack = 6;
	slot = 'right-hand';
	freq = 5;
}

iform {
	iclass = 'wepon';
	material = 'rusty';
	char = '$';
	modify_attack = 9;
	slot = 'right-hand';
	freq = 7;
	weight = 600;
}

-- Throwing weapons --
iform {
	iclass = 'shuriken';
	material = 'rusty';
	char = '+';
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
	equipable = true;
	char = '^';
	weight = 1250;
	modify_ac = 4;
	slot = 'head';
	freq = 8;
}

iform {
	iclass = 'mail coif';
	material = 'rusty';
	equipable = true;
	char = '^';
	weight = 1450;
	modify_ac = 3;
	slot = 'head';
	freq = 7;
}

iform {
	iclass = 'cuirass';
	material = 'rusty';
	equipable = true;
	char = ']';
	weight = 5000;
	modify_ac = 9;
	slot = 'torso';
	freq = 4;
}

iform {
	iclass = 'hauberk';
	material = 'rusty';
	equipable = true;
	char = ']';
	weight = 3000;
	modify_ac = 7;
	slot = 'torso';
	freq = 4;
}

iform {
	iclass = 'greaves';
	material = 'rusty';
	equipable = true;
	char = '}';
	weight = 4000;
	modify_ac = 7;
	slot = 'legs';
	freq = 5;
}

iform {
	iclass = 'boots';
	material = 'rusty';
	equipable = true;
	char = '"';
	weight = 4500;
	modify_ac = 14;
	slot = 'feet';
	freq = 6;
}

-- To be categorized --
iform {
	iclass = 'cape';
	material = 'magic';
	equipable = true;
	char = '\\';
	weight = 750;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 1;
}
