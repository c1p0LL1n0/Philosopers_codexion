/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_job(void *arg)
{
	t_coder	*c;
	t_all	*a;

	c = (t_coder *)arg;
	a = c->all;
	while (!is_stop(a))
	{
		if (atomic_load(&c->count) >= a->need)
			break ;
		if (one_round(c, a))
			break ;
	}
	return (NULL);
}

static int	is_burned(t_all *a, t_coder *c, uint64_t now)
{
	if (atomic_load(&c->state) == COMPILING)
		return (0);
	return (now >= atomic_load(&c->last) + a->burn);
}

void	*watch_job(void *arg)
{
	t_all		*a;
	int			i;
	uint64_t	now;
	int			id;

	a = (t_all *)arg;
	while (1)
	{
		if (is_stop(a))
			break ;
		now = now_ms(a);
		i = 0;
		while (i < a->n)
		{
			if (is_burned(a, &a->coders[i], now))
			{
				id = a->coders[i].id;
				atomic_store(&a->stop, 1);
				say(a, id, "burned out", now);
				return (NULL);
			}
			i++;
		}
		if (all_done(a))
		{
			atomic_store(&a->stop, 1);
			break ;
		}
		usleep(500);
	}
	return (NULL);
}
