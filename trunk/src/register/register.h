/***************************************************************************
 *   Copyright (C) 2009 by Leonardo de Miranda Borba, Adriano Zanette      *
 *   lmborba@gmail.com, adrzanette@gmail.com                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __REGISTER_H
#define __REGISTER_H

#include <hash_table.h>
#include <pthread.h>

#define COMMAND_JOIN     1
#define COMMAND_PUBLISH  2
#define COMMAND_SEARCH   3

#define INCORRECT_ANSWER 24
#define CORRECT_ANSWER   16

#define SEARCH_END       0
#define SEARCH_NOT_END   1

#define HELLO_PORT       5679

struct all_information {

  symbol_t stable;
  
  linkedlist * servents;
  
  pthread_mutex_t servents_mutex;
  pthread_mutex_t stable_mutex;
  
};

#endif
