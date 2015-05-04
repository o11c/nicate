#include "bitset.h"
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

#include <stdlib.h>
#include <string.h>

#include "hashmap.h"


struct BitSet
{
    size_t word;
};


static size_t bitset_words(BitSet *b)
{
    return b[-1].word;
}
static size_t bitset_index(size_t bi)
{
    return bi / (sizeof(size_t) * 8);
}
static size_t bitset_mask(size_t bi)
{
    size_t shift = bi % (sizeof(size_t) * 8);
    size_t one = 1;
    return one << shift;
}

BitSet *bitset_create(size_t bits)
{
    size_t words = bitset_index(bits - 1) + 1;
    BitSet *rv = (BitSet *)calloc(words + 1, sizeof(*rv));
    rv->word = words;
    return rv + 1;
}

void bitset_destroy(BitSet *b)
{
    free(b - 1);
}

void bitset_set(BitSet *b, size_t i)
{
    size_t index = bitset_index(i);
    size_t mask = bitset_mask(i);
    size_t *w = &b[index].word;
    *w |= mask;
}

void bitset_unset(BitSet *b, size_t i)
{
    size_t index = bitset_index(i);
    size_t mask = bitset_mask(i);
    size_t *w = &b[index].word;
    *w &= ~mask;
}

bool bitset_test(BitSet *b, size_t i)
{
    size_t index = bitset_index(i);
    size_t mask = bitset_mask(i);
    size_t *w = &b[index].word;
    return (*w & mask) != 0;
}

void bitset_invert(BitSet *b)
{
    size_t w = bitset_words(b);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        b[i].word = ~b[i].word;
    }
}

void bitset_erase(BitSet *b)
{
    size_t w = bitset_words(b);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        b[i].word = 0;
    }
}

bool bitset_any(BitSet *b)
{
    size_t w = bitset_words(b);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        if (b[i].word)
        {
            return true;
        }
    }
    return false;
}


HashKey bitset_as_key(BitSet *b)
{
    size_t w = bitset_words(b);
    HashKey rv = {(unsigned char *)b, w * sizeof(size_t)};
    return rv;
}



void char_bitset_set(CharBitSet *b, unsigned char i)
{
    bitset_set((BitSet *)b, i);
}

void char_bitset_unset(CharBitSet *b, unsigned char i)
{
    bitset_unset((BitSet *)b, i);
}

bool char_bitset_test(CharBitSet *b, unsigned char i)
{
    return bitset_test((BitSet *)b, i);
}


void char_bitset_invert(CharBitSet *b)
{
    size_t w = sizeof(b->_data) / sizeof(b->_data[0]);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        b->_data[i] = ~b->_data[i];
    }
}

void char_bitset_erase(CharBitSet *b)
{
    size_t w = sizeof(b->_data) / sizeof(b->_data[0]);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        b->_data[i] = 0;
    }
}

bool char_bitset_any(CharBitSet *b)
{
    size_t w = sizeof(b->_data) / sizeof(b->_data[0]);
    size_t i;
    for (i = 0; i < w; ++i)
    {
        if (b->_data[i])
        {
            return true;
        }
    }
    return false;
}
