///
/// inventory.c
///
/// Handles inventories, which are simple, arbitrarily large arrays of
/// item pointers.
///
/// Items have been implemented to keep track of the inventory they are
/// in, such that an item may be in at most one inventory (they contain
/// only one inventory/index member). The current inventory containing
/// an item is contained within \ref item.of , which will be `NULL` if
/// no inventory presently contains it.

#include <assert.h>
#include <stdlib.h>
#include "inventory.h"
#include "io/display.h"

///
/// Allocates a new inventory.
///
/// \newref{inv_free}
///
/// \param max Maximum weight of the inventory, or `INFINITE` for no limit.
///
inventory * inv_new(int max)
{
	inventory * inv = malloc(sizeof(inventory));
	inv->max_weight = max;
	inv->size = 0;
	inv->weight = 0;
	inv->itms = NULL;
	return inv;
}

///
/// Frees an inventory
///
/// \param inv The inventory to free.
///
void inv_free(inventory * inv)
{
	int i;

	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL) {
			item_free(inv->itms[i]);
		}
	}

	free(inv->itms);
	free(inv);
}

///
/// Adds an item to an inventory
/// Fails if \ref inv_try would fail
///
/// \note
/// If the item was previously in another inventory, and adding the item
/// to this inventory succeeded, it is guaranteed that it will have been
/// removed from the previous inventory.
///
/// \param inv The operant inventory.
/// \param it The item to add.
/// \return The index allocated to the item, or `INVALID` if addition failed.
///
int inv_add(inventory * inv, item * it)
{
#define REALLOC_SIZE 8

	int i;
	// See if item fits
	if (!inv_try(inv, it)) return INVALID;
	inv->weight += it->weight;

	// Look for free slot
	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] == NULL) {
			inv->itms[i] = it;
			if(it->of) inv_rm(it);
			it->of = inv;
			it->i = i;
			return i;
		}
	}

	// No free slots, must add slots
	if (inv->size % REALLOC_SIZE == 0) {
		inv->itms = realloc(inv->itms,
				(inv->size + REALLOC_SIZE) * sizeof(item *)
		);
	}
	inv->itms[inv->size] = it;
	if(it->of) inv_rm(it);
	it->of = inv;
	it->i = inv->size;

	for (i = 1; i < REALLOC_SIZE; i++) {
		inv->itms[i + inv->size] = NULL;
	}

	inv->size += REALLOC_SIZE;
	return inv->size - REALLOC_SIZE;
}

///
/// Verifies whether the item can be added to the inventory.
/// Presently, this implements the following checks:
/// * Fails if new weight exceeds max weight
///
/// \param inv The operant inventory.
/// \param it The item to test.
/// \return Whether or not the addition should succeed.
///
int inv_try(inventory * inv, item * it)
{
	if (inv->max_weight == INFINITE) return 1;
	return it->weight + inv->weight <= inv->max_weight;
}

///
/// Removes an item from its containing inventory.
/// This operation never fails.
///
/// \param it The item to remove from any containing inventory.
/// \return it, for backward compatibility.
///
item * inv_rm(item *it)
{
	if (!it || !it->of) return it;

	it->of->weight -= it->weight;
	it->of->itms[it->i] = NULL;
	it->of = NULL;
	it->i = -1;

	return it;
}

///
/// Counts the number of items in an inventory.
///
/// \note
/// The array structure may be sparse (that is, contain NULL pointers
/// within the boundary of \ref inv.size , so this function should be
/// used wherever an accurate count of items in an inventory is required.
///
/// \param inv The operant inventory.
/// \return The number of items therein.
///
int inv_count(inventory* inv){
	int i;
	int count=0;
	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL)
			count++;
	}

return count;
}

///
/// Clears an inventory, emptying it of all items.
///
/// \warning
/// This frees all the contained items via \ref item_free .
///
/// \param inv The inventory to clear.
///
void inv_clear(inventory * inv)
{
	int i;
	for (i = 0; i < inv->size; i++) {
	if (inv->itms[i] == NULL) continue;

	item * ret = inv->itms[i];
	inv->itms[i] = NULL;
	item_free(ret);
	}
	inv->weight = 0;
}

///
/// Converts an index to a character
///
/// \todo
/// Fix for indexs >= 62
///
/// \param i The index.
/// \return A character corresponding to that index.
///
char ind2ch(int i)
{
	if (i < 26) return 'a' + i;
	if (i < 52) return 'A' + i - 26;
	if (i < 62) return '0' + i - 52;
	assert(0); // TODO: Handle inventories with more than 62 items
}

///
/// Converts a character to an index
///
/// \todo
/// fix for characters not [a-zA-Z0-9]
///
/// \param c The character corresponding to an index.
/// \return The corresponding index.
///
int  ch2ind(char c)
{
	if (c <= '9') return (int)c + 52 - '0';
	if (c <= 'Z') return (int)c + 26 - 'A';
	if (c <= 'z') return (int)c - 'a';
	assert(0); // TODO: Handle inventories with more than 62 items
}
