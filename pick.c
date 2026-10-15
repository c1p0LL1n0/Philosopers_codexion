/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   give2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Собираем голодных в кучу. */
static int	fill(t_all *a)
{
	int	i;

	a->hsize = 0;
	i = 0;
	while (i < a->n)
	{
		if (a->coders[i].state == WAITING && a->coders[i].want > 0)
			hpush(a, i);
		i++;
	}
	return (a->hsize == 0);
}

/* Раздаём донглы по куче. */
int	pick_give(t_all *a, uint64_t now)
{
	int	best;
	int	fed;

	pthread_mutex_lock(&a->lock);
	if (a->stop || fill(a))
	{
		pthread_mutex_unlock(&a->lock);
		return (0);
	}
	fed = 0;
	while (a->hsize > 0 && !fed)
	{
		best = hpop(a);
		if (a->coders[best].state == WAITING)
			fed = give_one_pub(a, best, now);
	}
	if (fed && all_done(a))
		a->stop = 1;
	if (fed)
		pthread_cond_broadcast(&a->bell);
	pthread_mutex_unlock(&a->lock);
	return (fed);
}

/* Кладём донглы и мажем холодком. */
void	put_back(t_all *a, int idx, uint64_t now)
{
	t_dongle	*l;
	t_dongle	*r;

	l = &a->dongles[idx];
	r = &a->dongles[(idx + 1) % a->n];
	pthread_mutex_lock(&a->lock);
	pthread_mutex_lock(&l->m);
	if (r != l)
		pthread_mutex_lock(&r->m);
	l->owner = -1;
	l->ready = now + a->cool;
	if (r != l)
	{
		r->owner = -1;
		r->ready = now + a->cool;
		pthread_mutex_unlock(&r->m);
	}
	pthread_mutex_unlock(&l->m);
	pthread_cond_broadcast(&a->bell);
	pthread_mutex_unlock(&a->lock);
}
