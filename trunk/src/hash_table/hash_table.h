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

#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PRIME 211
#define EOST   0

#define __FALSE  0
#define __TRUE   !(__FALSE)
#define ERRO   -1

struct dimensoes {
  int dim[128][2];
  int n_dim;
};

/**
 * Tipo abstrato das entradas na tabela de Hash.
 */
typedef struct {
	char* name;  /**< um string que representa o nome de uma variavel. */
	clock_t time;    /**< representacao do tempo do último hello. */
	int ip;    /**< string que representa o ip. */ 
} entry_t ;


/**
 * Tipo abstrato de uma lista encadeada de entradas na tabela Hash (para tratamento de colisões).
 */
typedef struct LINKEDLIST {
	entry_t* head;		/**< referencia a uma entrada na tabela. */
	struct LINKEDLIST* next;	/**< referencia pro próximo elemento da lista. */
} linkedlist;


/**
 * Tipo abstrato de uma tabela hash
 */
typedef struct SYMBOL_T {
	linkedlist *ll[PRIME];	/**< referencia a um vetor de listas encadeadas. */
} symbol_t ;


/** \brief Calcula um codigo Hash correspondente a uma string.
 *
 * Obs.: Funcao auxiliar.
 * Utiliza o algoritmo HPJW (do livro do Dragao).
 *
 * @param name um string.
 * @return código hash correspondente.
 */
int hashify(char * name);

/** \brief Compara duas strings.
 *
 * Obs.: Funcao auxiliar.
 * @param str1 um string.
 * @param str2 um string.
 * @return 0 se os strings são iguais; um valor >0 se str1 sucede str2 ou um valor <0 em caso oposto.
 */
int compare(char * str1, char * str2);

/** \brief Destroi uma lista encadeada.
 *
 * Obs.: Funcao auxiliar.
 * @param ll lista encadeada.
 */
void free_linkedlist(linkedlist* ll);

/** \brief Cria uma entrada de uma tabela hash.
 *
 * Obs.: Funcao auxiliar.
 * @param enter referencia a referencia da entrada que sera alocada.
 * @param insertion referencia a entrada que sera inserida.
 * @param next proximo elemento que sera ligado a entrada que sera inserida, se houver.
 * @return FALSE (0) se criação nao foi efetuada; TRUE caso contrario.
 */
int create_new(linkedlist ** enter, entry_t * insertion, linkedlist * next);

/** \brief Inicializa a tabela Hash.
 *
 * @param table uma referencia sobre uma tabela de simbolos.
 * @return o valor 0 se deu certo.
 */
int init_table(symbol_t* table) ;

/** \brief Destroi a tabela Hash.
 *
 * 'free_table' eh o destrutor da estrutura de dados. Deve ser chamado pelo 
 * usuario no fim de seu uso de uma tabela de simbolos.
 * @param table uma referencia sobre uma tabela de simbolos.
 */
void free_table(symbol_t* table) ;

/** \brief Retorna um ponteiro sobre a entrada associada a 'name'.
 *
 * @param table uma tabela de simbolos.
 * @param name um char* (string).
 * @return uma lista encadeada com todas as entradas que contém 'name' na tabela hash, ou NULL se 'name'
 *         nao se encontrou na tabela.
 */
linkedlist* lookup(symbol_t table, char* name) ;

/** \brief Insere uma entrada em uma tabela.
 *
 * @param table uma tabela de simbolos.
 * @param entry uma entrada.
 * @return um numero negativo se nao conseguiu efetuar a insercao, zero se
 *   deu certo.
 */
int insert(symbol_t* table, entry_t* entry) ;

/** \brief Imprime o conteudo de uma tabela.
 *
 * @param table uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_table(symbol_t table);

/** \brief Imprime o conteudo de uma tabela em um arquivo.
 *
 * @param out um descrito de arquivo (FILE*).
 * @param table uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_file_table(FILE * out, symbol_t table);

#endif
