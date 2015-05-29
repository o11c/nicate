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


typedef struct HashKey HashKey;
typedef struct HashValue HashValue;
typedef struct HashEntry HashEntry;
typedef struct HashMap HashMap;
typedef struct HashIterator HashIterator;

typedef struct Pool Pool;

typedef struct Builder Builder;
typedef struct BuildTranslationUnit BuildTranslationUnit;
typedef struct BuildTopLevel BuildTopLevel;
typedef struct BuildStorageClass BuildStorageClass;
typedef struct BuildDeclaration BuildDeclaration;
typedef struct BuildParamDeclaration BuildParamDeclaration;
typedef struct BuildMemberDeclaration BuildMemberDeclaration;
typedef struct BuildEnumerator BuildEnumerator;
typedef struct BuildType BuildType;
typedef struct BuildStatement BuildStatement;
typedef struct BuildInitializer BuildInitializer;
typedef struct BuildExpression BuildExpression;

typedef struct BuildTypePairDeclarator BuildTypePairDeclarator;
typedef struct BuildTypePairAbstractDeclarator BuildTypePairAbstractDeclarator;

typedef struct Nfa Nfa;
typedef struct MultiNfa MultiNfa;
typedef struct MreRuntime MreRuntime;

typedef struct BitSet BitSet;
typedef struct CharBitSet CharBitSet;

typedef struct Symbol Symbol;
typedef struct Lexicon Lexicon;
typedef struct Tokenizer Tokenizer;

typedef struct Tree Tree;
/* typedef enum ActionType ActionType; */
typedef struct Action Action;
typedef struct Rule Rule;
typedef struct Grammar Grammar;
typedef struct State State;
typedef struct Automaton Automaton;
