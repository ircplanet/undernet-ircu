/*
 * IRC - Internet Relay Chat, ircd/m_map.c
 * Copyright (C) 1990 Jarkko Oikarinen and
 *                    University of Oulu, Computing Center
 *
 * See file AUTHORS in IRC package for additional names of
 * the programmers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 */

#include "config.h"

#include "client.h"
#include "ircd.h"
#include "ircd_defs.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_snprintf.h"
#include "ircd_string.h"
#include "list.h"
#include "match.h"
#include "msg.h"
#include "numeric.h"
#include "s_user.h"
#include "s_serv.h"
#include "send.h"
#include "querycmds.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <stdio.h>
#include <string.h>

/** Send a server map to a client.
 * @param[in] cptr Client to who to send the map.
 * @param[in] server Top-level server to display.
 * @param[in] mask Mask to filter which servers are shown.
 * @param[in] prompt_length Number of characters used in prompt.
 */
static void dump_map(struct Client *cptr, struct Client *server, char *mask, int prompt_length)
{
  const char *chr;
  static char prompt[64];
  struct DLink *lp;
  char *p = prompt + prompt_length;
  int cnt = 0;
  
  *p = '\0';
  if (prompt_length > 60)
    send_reply(cptr, RPL_MAPMORE, prompt, cli_name(server));
  else
  {
    char lag[512];
    if (cli_serv(server)->lag>10000)
      lag[0]=0;
    else if (cli_serv(server)->lag<0)
      strcpy(lag,"(0s)");
    else
      sprintf(lag,"(%is)",cli_serv(server)->lag);
    if (IsBurst(server))
      chr = "*";
    else if (IsBurstAck(server))
      chr = "!";
    else
      chr = "";
    send_reply(cptr, RPL_MAP, prompt, chr, cli_name(server),
               lag, (server == &me) ? UserStats.local_clients :
                                      cli_serv(server)->clients);
  }
  if (prompt_length > 0)
  {
    p[-1] = ' ';
    if (p[-2] == '`')
      p[-2] = ' ';
  }
  if (prompt_length > 60)
    return;
  strcpy(p, "|-");
  for (lp = cli_serv(server)->down; lp; lp = lp->next)
    if (match(mask, cli_name(lp->value.cptr)))
      ClrFlag(lp->value.cptr, FLAG_MAP);
    else
    {
      SetFlag(lp->value.cptr, FLAG_MAP);
      cnt++;
    }
  for (lp = cli_serv(server)->down; lp; lp = lp->next)
  {
    if (!HasFlag(lp->value.cptr, FLAG_MAP))
      continue;
    if (--cnt == 0)
      *p = '`';
    dump_map(cptr, lp->value.cptr, mask, prompt_length + 2);
  }
  if (prompt_length > 0)
    p[-1] = '-';
}


/** Handle a MAP request from a local connection.
 * -- by Run
 *
 * \a parv has the following elements:
 * \li \a parv[1] (optional) is a mask of servers to display in the map
 *
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int m_map(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (feature_bool(FEAT_HIS_MAP) && !IsAnOper(sptr))
  {
    sendcmdto_one(&me, CMD_NOTICE, sptr, "%C :%s %s", sptr,
                  "/MAP has been disabled, from CFV-165.  "
                  "Visit ", feature_str(FEAT_HIS_URLSERVERS));
    return 0;
  }
  if (parc < 2)
    parv[1] = "*";
  dump_map(sptr, &me, parv[1], 0);
  send_reply(sptr, RPL_MAPEND);

  return 0;
}

