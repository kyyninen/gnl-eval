/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl-test.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpolonen <tpolonen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/12 15:51:58 by tpolonen          #+#    #+#             */
/*   Updated: 2021/12/21 17:17:07 by tpolonen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FL_CAT 		0b000001
#define FL_WC 		0b000010
#define FL_STDOUT	0b000100
#define FL_HELP		0b001000
#define FL_MULTI	0b010000
#define FL_COMPARE	0b100000

static void run_gnl(int flags, int fd, char *path)
{
	char *line;
	int result = 1;
	int lines = 0;

	while (result > 0) {
		result = get_next_line(fd, &line);
		if (result > 0) {
			lines += result;
			if (flags & FL_CAT || flags & FL_STDOUT) {
				printf("%s\n", line);
			}
			free(line);
		} else if (result == -1) {
			printf("got -1\n");
		} 
	} if (flags & FL_WC) {
		printf("%d %s\n", lines, path);
	} else if (flags & FL_STDOUT) {
		printf("%d (stdout)\n", lines);
	}
//	system("leaks test_gnl");
}

/*
static void run_gl_from_file(int fd, char *path)
{
	ssize_t result = 1;
	size_t cap = 0;
	int lines = 0;
	char *line;
	FILE *file;

	file = fopen(path, "r");
	while (result > 0) {
		result = getline(&line, &cap, file);
		if (result > 0) {
			lines++;
			printf("%s\n",line);
			free(line);
		} else if (result == -1)
			printf("got -1\n");
	}
}
*/	

static void print_help(void)
{
	printf("usage:\n./gnl-test (-l/-c/-f/-m/-h) [FILENAME]\n\n");
	printf("\t-l\n\t\tbehaves as wc -l [FILENAME]\n");
	printf("\t-c\n\t\tbehaves as cat [FILENAME]\n\n");
	printf("\t-f\n\t\truns several tests with fake values for fd (invalid numbers or not opened fd's)\n\n");
	printf("\t-m\n\t\topens multiple files at the same time. enter all the filenames you want to use in this test as arguments. fails if only one filename is entered.\n\n");
	printf("\t-h\n\t\tdisplays this help message\n\n");
	printf("\t-x\n\t\tsame as running with [FILENAME] but uses getline(2) instead\n\n");
	printf("if run with just the FILENAME, contents are printed first and line count last.\n");
	printf("if run without any arguments, stdout is read and it's content and line count are printed.\n");
}

static int	validate_error(int fd, char **line)
{
	int result = get_next_line(fd, line);
	printf("result for fd=%d was: %d\n", fd, result);
	if (line != NULL && *line != NULL)
	{
		printf("something was put to line???\n");
		free(*line);
	}
	return (0);
}

static int	error_check(void)
{
	printf("running false fd tests!\n");
	char *line = NULL;

	validate_error(-42, NULL);
	validate_error(-42, &line);	
	validate_error(42, NULL);
	validate_error(42, &line);	
//	system("leaks test_gnl");
	return (0);
}

static void	bonus_check(int count, char** files)
{
	//open all the provided files in a loop and verify results
	while (count > 0) {
		printf("%s\n", *files++);
		count--;
	}
}

int	main(int argc, char** argv)
{
	int flags = 0;
	int fd;
	char *path;

	//process args
	if (argc == 1) {
		fd = 0;
		flags = FL_STDOUT;
		} 
	else if (argc == 2) {
		if (argv[1][0] == '-') {
			if (argv[1][1] == 'f')
				return (error_check());
			if (argv[1][1] == 'h') {
				flags = FL_HELP;
				goto run;
			}
		} flags = FL_CAT | FL_WC;
		path = argv[1];
		fd = open(path, O_RDONLY);
	} else if (argc == 3) {
		path = argv[2];
		fd = open(path, O_RDONLY);
		if (argv[1][1] == 'l')
			flags = FL_WC;
		else if (argv[1][1] == 'c')
			flags = FL_CAT;
	} else if (argc > 3 && argv[1][1] == 'm') {
		flags = FL_MULTI;
	}	
	run:
	if (flags & FL_WC || flags & FL_CAT || flags & FL_STDOUT) {
		run_gnl(flags, fd, path);
		close(fd);
	} else if (flags & FL_MULTI) {
		bonus_check(argc - 2, argv + 2);
	} else {
		if (!(flags & FL_HELP))
			printf("incorrect usage\n\n");
		print_help();
	} return (0);
}
