/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: digglega <ihavauch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:00:00 by digglega          #+#    #+#             */
/*   Updated: 2026/10/14 13:00:00 by digglega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Только циферки? */
static int	is_digits(char *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/* Буквы в число. */
static int	to_u64(char *s, uint64_t *out)
{
	uint64_t	v;
	int			i;

	v = 0;
	i = 0;
	while (s[i])
	{
		if (v > 1844674407370955161ULL)
			return (1);
		v = v * 10 + (uint64_t)(s[i] - '0');
		i++;
	}
	*out = v;
	return (0);
}

/* Читаем число. big = ноль запрещен. */
static int	get_num(char *s, uint64_t *out, int big)
{
	if (!is_digits(s))
		return (1);
	if (to_u64(s, out))
		return (1);
	if (big && *out == 0)
		return (1);
	if (!big && *out > 2147483647ULL)
		return (1);
	return (0);
}

/* Хвост аргументов. */
static int	parse_tail(t_all *a, char **av)
{
	uint64_t	need;

	if (get_num(av[5], &a->ref, 0))
		return (1);
	if (get_num(av[6], &need, 0))
		return (1);
	if (get_num(av[7], &a->cool, 0))
		return (1);
	a->need = (int)need;
	if (strcmp(av[8], "fifo") == 0)
		a->fifo = 1;
	else if (strcmp(av[8], "edf") == 0)
		a->fifo = 0;
	else
		return (1);
	return (0);
}

/* Читаем все билеты. */
int	parse_args(t_all *a, char **av)
{
	uint64_t	n;

	memset(a, 0, sizeof(t_all));
	if (get_num(av[1], &n, 1) || n == 0 || n > 200)
		return (1);
	a->n = (int)n;
	if (get_num(av[2], &a->burn, 1))
		return (1);
	if (get_num(av[3], &a->comp, 1))
		return (1);
	if (get_num(av[4], &a->deb, 0))
		return (1);
	if (parse_tail(a, av))
		return (1);
	if (a->need == 0)
		return (1);
	return (0);
}
