-----------------------
-- script/iforms.lua --
-----------------------

iform {
	name = 'rock';
	char = '*';
	weight = 500;
	freq = 20;
}

iform {
	name = 'muffin';
	consumable = true;
	char = '%';
	weight = 7;
	restore_stamina = 40;
	freq = 15;
}

iform {
	name = 'semi egg';
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
	name = 'health-potion';
	consumable = true;
	char = '&';
	weight = 60;
	restore_health = 10;
	freq = 15;
}

iform {
	name = 'sword';
	equipable = true;
	char = '/';
	weight = 400;
	modify_attack = 12;
	slot = 'right-hand';
	freq = 10;
}

iform {
	name = 'helm';
	equipable = true;
	char = '^';
	weight = 250;
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
	weight = 150;
	modify_ac = 5;
	modify_attack = 10;
	slot = 'cape';
	freq = 5;
}

iform {
	name = 'cloak of darkness';
	equipable = true;
	char = '\\';
	weight = 800;
	modify_ac = 2700;
	modify_attack = -1000;
	slot = 'cape';
	freq = 1;
}

iform {
	name = 'quaz boots';
	equipable = true;
	char = '"';
	weight = 900;
	modify_ac = 14;
	slot = 'feet';
	freq = 3;
}

iform {
	name = 'hiking boots';
	equipable = true;
	char = '"';
	weight = 140;
	modify_ac = 1;
	slot = 'feet';
	freq = 15;
}

iform {
	name = 'death stick';
	equipable = true;
	char = '/';
	weight = 4800;
	modify_ac = -100;
	modify_attack = 9001;
	slot = 'right-hand';
	freq = 1;
}
