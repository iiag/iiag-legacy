-----------------------
-- script/iforms.lua --
-----------------------

iform {
	name = 'rock';
	char = '*';
	weight = 100;
}

iform {
	name = 'muffin';
	char = '%';
	weight = 1;
	restore_stamina = 40;
}

iform {
	name = 'health-potion';
	consumable = true;
	char = '&';
	weight = 1;
	restore_health = 10;
}

iform {
	name = 'sword';
	equipable = true;
	char = '/';
	weight = 8;
	modify_attack = 12;
	slot = 'right-hand';
}

iform {
	name = 'helm';
	equipable = true;
	char = '^';
	weight = 6;
	modify_ac = 4;
	slot = 'head';
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
}

iform {
	name = 'magic cape';
	equipable = true;
	char = '\\';
	weight = 4;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
}

iform {
	name = 'cloak of darkness';
	equipable = true;
	char = '\\';
	weight = 80;
	modify_ac = 2700;
	modify_attack = -1000;
	slot = 'cape';
}

iform {
	name = 'quaz boots';
	equipable = true;
	char = '"';
	weight = 120;
	modify_ac = 14;
	slot = 'feet';
}

iform {
	name = 'hiking boots';
	equipable = true;
	char = '"';
	weight = 70;
	modify_ac = 1;
	slot = 'feet';
}

iform {
	name = 'death stick';
	equipable = true;
	char = '/';
	weight = 499;
	modify_ac = -100;
	modify_attack = 9001;
	slot = 'right-hand';
}
