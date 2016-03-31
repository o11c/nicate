#pragma once
/*
    Copyright © 2015 Ben Longbons

    This file is part of Nicate.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <stddef.h>

#include "fwd.h"


struct CharBitSet
{
    size_t _data[256 / (sizeof(size_t) * 8)];
};

BitSet *bitset_create(size_t bits);
void bitset_destroy(BitSet *b);
size_t bitset_bits(BitSet *b);
size_t bitset_refcount(BitSet *b);

BitSet *bitset_incref(BitSet *b);
BitSet *bitset_copy(BitSet *b);
BitSet *bitset_unique(BitSet *b);

void bitset_set(BitSet *b, size_t i);
void bitset_unset(BitSet *b, size_t i);
bool bitset_test(BitSet *b, size_t i);

void bitset_invert(BitSet *b);
void bitset_erase(BitSet *b);
bool bitset_any(BitSet *b);
bool bitset_all(BitSet *b);

bool bitset_equals(BitSet *l, BitSet *r);
void bitset_and_eq(BitSet *l, BitSet *r);
void bitset_or_eq(BitSet *l, BitSet *r);
void bitset_xor_eq(BitSet *l, BitSet *r);

HashKey bitset_as_key(BitSet *b);


void char_bitset_set(CharBitSet *b, unsigned char i);
void char_bitset_unset(CharBitSet *b, unsigned char i);
bool char_bitset_test(CharBitSet *b, unsigned char i);

void char_bitset_invert(CharBitSet *b);
void char_bitset_erase(CharBitSet *b);
bool char_bitset_any(CharBitSet *b);
