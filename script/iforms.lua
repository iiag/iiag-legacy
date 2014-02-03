-----------------------
-- script/iforms.lua --
-----------------------

iform {
	name = 'rock';
	char = '*';
	weight = 100;
	freq = 20;
}

iform {
	name = 'muffin';
	char = '%';
	weight = 1;
	restore_stamina = 40;
	freq = 15;
}

iform {
	name = 'health-potion';
	consumable = true;
	char = '&';
	weight = 1;
	restore_health = 10;
	freq = 15;
}

iform {
	name = 'sword';
	equipable = true;
	char = '/';
	weight = 8;
	modify_attack = 12;
	slot = 'right-hand';
	freq = 10;
}

iform {
	name = 'helm';
	equipable = true;
	char = '^';
	weight = 6;
	modify_ac = 4;
	slot = 'head';
	freq = 15;
}

iform {
	name = 'breadstick';
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

iform {
	name = 'magic cape';
	equipable = true;
	char = '\\';
	weight = 4;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 5;
}

iform {
	name = 'cloak of darkness';
	equipable = true;
	char = '\\';
	weight = 80;
	modify_ac = 2700;
	modify_attack = -1000;
	slot = 'cape';
	freq = 1;
}

iform {
	name = 'quaz boots';
	equipable = true;
	char = '"';
	weight = 120;
	modify_ac = 14;
	slot = 'feet';
	freq = 3;
}

iform {
	name = 'hiking boots';
	equipable = true;
	char = '"';
	weight = 70;
	modify_ac = 1;
	slot = 'feet';
	freq = 15;
}

iform {
	name = 'death stick';
	equipable = true;
	char = '/';
	weight = 499;
	modify_ac = -100;
	modify_attack = 9001;
	slot = 'right-hand';
	freq = 1;
}
