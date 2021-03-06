/*
** Copyright (C) 2002 by Kevin L. Mitchell <klmitch@mit.edu>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA
**
** @(#)$Id$
*/
#include "dbprim.h"
#include "dbprim_int.h"

RCSTAG("@(#)$Id$");

struct _st_iter_s {
  smat_table_t *si_table;	/* pointer to the smat table */
  smat_iter_t	si_iter;	/* iter function */
  void	       *si_extra;	/* extra data */
};

static unsigned long
_st_iter_iter(hash_table_t *table, hash_entry_t *ent, void *extra)
{
  struct _st_iter_s *si;

  si = extra;

  /* call the user iteration function--with appropriate translation */
  return (*si->si_iter)(si->si_table, he_value(ent), si->si_extra);
}

/** \ingroup dbprim_smat
 * \brief Iterate over each entry in a sparse matrix.
 *
 * This function iterates over every entry in a sparse matrix (in an
 * unspecified order), executing the given \p iter_func on each entry.
 *
 * \param table	A pointer to a #smat_table_t.
 * \param iter_func
 *		A pointer to a callback function used to perform
 *		user-specified actions on an entry in a sparse
 *		matrix.  \c NULL is an invalid value.  See the
 *		documentation for #smat_iter_t for more information.
 * \param extra	A \c void pointer that will be passed to \p
 *		iter_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The sparse matrix is frozen.
 */
unsigned long
st_iter(smat_table_t *table, smat_iter_t iter_func, void *extra)
{
  struct _st_iter_s si;

  initialize_dbpr_error_table(); /* initialize error table */

  if (!st_verify(table) || !iter_func) /* verify arguments */
    return DB_ERR_BADARGS;

  /* initialize extra data... */
  si.si_table = table;
  si.si_iter = iter_func;
  si.si_extra = extra;

  /* call into linked list library to iterate over the list */
  return ht_iter(&table->st_table, _st_iter_iter, &si);
}
