/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

uint64_t	wall_ms(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000);
}

uint64_t	now_ms(t_all *a)
{
	return (wall_ms() - a->start);
}

int	is_stop(t_all *a)
{
	return (a->stop);
}

/* Спим до дедлайна точным сном. Кусочки по 2 мс чтобы выйти при stop. */
void	naps(t_all *a, uint64_t ms)
{
	struct timespec	ts;
	uint64_t		end;
	uint64_t		now;
	uint64_t		left;
	uint64_t		step;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	end = (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000 + ms;
	while (1)
	{
		if (is_stop(a))
			break ;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		now = (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
		if (now >= end)
			break ;
		left = end - now;
		step = 2;
		if (left < step)
			step = left;
		ts.tv_sec = (time_t)(step / 1000);
		ts.tv_nsec = (long)((step % 1000) * 1000000L);
		nanosleep(&ts, NULL);
	}
}