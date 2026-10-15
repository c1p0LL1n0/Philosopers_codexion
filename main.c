/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Зовём всех за стол. */
int	start_all(t_all *a, pthread_t *pl, pthread_t *mo)
{
	int	i;

	i = 0;
	while (i < a->n)
	{
		if (pthread_create(&a->coders[i].th, NULL, coder_job, &a->coders[i]))
			return (1);
		i++;
	}
	if (pthread_create(pl, NULL, plan_job, a))
		return (1);
	if (pthread_create(mo, NULL, watch_job, a))
		return (1);
	return (0);
}

/* Всех ждём. */
void	join_all(t_all *a, pthread_t pl, pthread_t mo)
{
	int	i;

	pthread_join(mo, NULL);
	pthread_join(pl, NULL);
	i = 0;
	while (i < a->n)
	{
		pthread_join(a->coders[i].th, NULL);
		i++;
	}
}

/* Грустный конец при ошибке старта. */
static void	fail_start(t_all *a)
{
	pthread_mutex_lock(&a->lock);
	a->stop = 1;
	pthread_cond_broadcast(&a->bell);
	pthread_mutex_unlock(&a->lock);
}

/* Главная дверь. */
int	main(int argc, char **argv)
{
	t_all		a;
	pthread_t	pl;
	pthread_t	mo;

	if (argc != 9)
	{
		fprintf(stderr, "Error: need 8 args\n");
		return (1);
	}
	if (parse_args(&a, argv) || table_init(&a))
	{
		fprintf(stderr, "Error: bad args\n");
		return (1);
	}
	a.start = wall_ms();
	if (start_all(&a, &pl, &mo))
	{
		fprintf(stderr, "Error: thread fail\n");
		fail_start(&a);
		table_free(&a);
		return (1);
	}
	join_all(&a, pl, mo);
	table_free(&a);
	return (0);
}
