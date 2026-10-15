/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Часы. Назад не идут. */
uint64_t	wall_ms(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000);
}

/* Сколько прошло с начала. */
uint64_t	now_ms(t_all *a)
{
	return (wall_ms() - a->start);
}

/* Игра кончилась? */
int	is_stop(t_all *a)
{
	int	s;

	pthread_mutex_lock(&a->lock);
	s = a->stop;
	pthread_mutex_unlock(&a->lock);
	return (s);
}

/* Спим кусочками, чтобы проснуться при конце. */
void	naps(t_all *a, uint64_t ms)
{
	uint64_t	i;

	i = 0;
	while (i < ms)
	{
		if (is_stop(a))
			break ;
		usleep(1000);
		i++;
	}
}
