// { dg-do compile }
// { dg-options "-std=gnu++0x" }

// Copyright (C) 2011 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.


// NOTE: This makes use of the fact that we know how moveable
// is implemented on pair, and also vector. If the implementation
// changes this test may begin to fail.

#include <tuple>

bool test __attribute__((unused)) = true;


// make_tuple
#if 0
void
test_make_tuple()
{
  {
    typedef std::tuple<int, float> tuple_type;
    constexpr tuple_type p1 = std::make_tuple(22, 22.222);
  }

  {
    typedef std::tuple<int, float, int> tuple_type;
    constexpr tuple_type p1 = std::make_tuple(22, 22.222, 77799);
  }
}
#endif

// get
void
test_get()
{
  {
    typedef std::tuple<int, float> tuple_type;
    constexpr tuple_type t1 { 55, 77.77 };
    constexpr auto var = std::get<1>(t1);
  }

  {
    typedef std::tuple<int, float, int> tuple_type;
    constexpr tuple_type t1 { 55, 77.77, 99 };
    constexpr auto var = std::get<2>(t1);
  }
}

// tuple_cat
#if 0
void
test_tuple_cat()
{
  typedef std::tuple<int, float> 	tuple_type1;
  typedef std::tuple<int, int, float> tuple_type2;

  constexpr tuple_type1 t1 { 55, 77.77 };
  constexpr tuple_type2 t2 { 55, 99, 77.77 };
  constexpr auto cat1 = std::tuple_cat(t1, t2);
}
#endif

int
main()
{
#if 0
  test_make_tuple();
#endif

  test_get();

#if 0
  test_tuple_cat();
#endif

  return 0;
}
