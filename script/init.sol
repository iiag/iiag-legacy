-----------------------
--  script/init.lua  --
-----------------------

info('---Sol Init Script---')

scripts = ["script/cforms.sol", "script/itypes.sol", "script/iforms.sol", "script/rforms.sol", "script/recipe.sol"]

for script in scripts do
	debug('--- Init '+script+'---')
	res = try(execfile, script)
	if !res[0] then
		fatal('Failed to run init script '+script+': '+res[1])
	end
end

player({
	name = "you",
	max_health = 10,
	attack = 3,
	sight = 30,
})

info('---Sol Init Complete!---')