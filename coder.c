/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	one_round(t_coder *c, t_all *a)
{
	uint64_t		now;
	uint64_t		t0;
	uint64_t		ns;
	struct timespec	ts;

	/* Билет = штамп голода. fifo: кто раньше встал — тот старше. */
	atomic_store(&c->ticket, now_ms(a));
	t0 = now_ms(a);
	while (!is_stop(a))
	{
		now = now_ms(a);
		if (now - t0 > a->burn)
			return (0);
		if (try_take(a, c, now))
			break ;
		/* Знак: срочному 0 (крутись), остальным пауза. */
		ns = yield_ns(a, c, now);
		if (ns)
		{
			ts.tv_sec = 0;
			ts.tv_nsec = (long)ns;
			nanosleep(&ts, NULL);
		}
	}
	if (is_stop(a))
		return (1);
	now = now_ms(a);
	say_took(a, c->id - 1, now);
	naps(a, a->comp);
	if (is_stop(a))
		return (1);
	put_back(a, c->id - 1, now_ms(a));
	say(a, c->id, "is debugging", now_ms(a));
	naps(a, a->deb);
	say(a, c->id, "is refactoring", now_ms(a));
	naps(a, a->ref);
	atomic_store(&c->state, WAITING);
	return (0);
}
