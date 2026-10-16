/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <digglega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/16 00:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_all(t_all *a, pthread_t *mo)
{
	pthread_attr_t	attr;
	int				i;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 64 * 1024);
	i = 0;
	while (i < a->n)
	{
		if (pthread_create(&a->coders[i].th, &attr, coder_job, &a->coders[i]))
		{
			pthread_attr_destroy(&attr);
			return (1);
		}
		i++;
	}
	pthread_attr_destroy(&attr);
	if (pthread_create(mo, NULL, watch_job, a))
		return (1);
	return (0);
}

void	join_all(t_all *a, pthread_t mo)
{
	int	i;

	pthread_join(mo, NULL);
	i = 0;
	while (i < a->n)
	{
		pthread_join(a->coders[i].th, NULL);
		i++;
	}
}

static void	fail_start(t_all *a)
{
	atomic_store(&a->stop, 1);
}

int	main(int argc, char **argv)
{
	t_all		a;
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
	if (start_all(&a, &mo))
	{
		fprintf(stderr, "Error: thread fail\n");
		fail_start(&a);
		table_free(&a);
		return (1);
	}
	join_all(&a, mo);
	table_free(&a);
	return (0);
}
