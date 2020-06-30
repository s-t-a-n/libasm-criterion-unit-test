/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   criterion_libasm.c                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: sverschu <sverschu@student.codam.n>          +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/06/29 21:03:48 by sverschu      #+#    #+#                 */
/*   Updated: 2020/06/30 18:56:25 by sverschu      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "libasm.h"

// define STRNCMP_SIGN as 1 to do only sign comparison, nut signed number comparison
#ifndef STRNCMP_SIGN
# define STRNCMP_SIGN					1
#endif

// disable RANDOMIZED_TESTS if your output is totally flooded by errors containing <random strings>
#ifndef RANDOMIZED_TESTS
# define RANDOMIZED_TESTS				1
#endif

// set FSANITIZE_ADDRESS to 1 if you are planning on using -fsanitize=address (which you should); it disables segv tests
// because these will throw false positives
#ifndef FSANITIZE_ADDRESS
# define FSANITIZE_ADDRESS				0
#endif

// MEMSIZE is the amount of memmory called to life by malloc. pay attention: bigger is not necessarily more rigorous!
#ifndef MEMSIZE
# define MEMSIZE						1000
#endif

// ITERATIONS is the ammount of times our beloved for loops will be called to life; more is theoretically more rigorous
#ifndef ITERATIONS
# define ITERATIONS						5000
#endif

// i hope this is a proper enough rand init; a custom main throws all kinds of weird errors on my computer
ReportHook(PRE_ALL)() {
	srand(time(NULL));
}

char *CRIT_randstring(size_t max_length)
{
	int length = rand() % --max_length;
	char *random_string = malloc(sizeof(char) * (length + 1));
    if (random_string)
	{
        for (int i = 0; i < length; i++)
		{
            random_string[i] = rand() % 256;
			if (random_string[i] == '\0' && rand() % 2)
				random_string[i] = 42;
		}
        random_string[length] = '\0';
        return random_string;
    }
	return (NULL);
}

// randstring_nullbyte garantees a nullbyte ONLY at the end of string
char *CRIT_randstring_nullbyte(size_t max_length)
{
	int length = rand() % --max_length;
	char *random_string = malloc(sizeof(char) * (length + 1));
    if (random_string)
	{
        for (int i = 0; i < length; i++)
		{
            random_string[i] = rand() % 256;
			if (random_string[i] == '\0')
				random_string[i] = 42;
		}
        random_string[length] = '\0';
        return random_string;
    }
	return (NULL);
}

unsigned char *CRIT_randmem(size_t length)
{
	unsigned char *random_string = malloc(sizeof(unsigned char) * length);
    if (random_string)
	{
        for (size_t i = 0; i < length; i++)
            random_string[i] = rand() % 256;
        return random_string;
    }
	return (NULL);
}

#if !FSANITIZE_ADDRESS
Test(strings, ft_strlen_segv, .signal = SIGSEGV)
{
   ft_strlen(NULL);
}
#endif

Test(strings, ft_strlen)
{
	char *str;

	str = "Hello";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));
	str = "He\0llo";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));
	str = "Heøllo";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));
	str = "He\255llo";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));
	str = "";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));
	str = "\255";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s| -> strlen: %zu, ft_strlen: %zu", str, strlen(str), ft_strlen(str));

	srand(time(NULL));
#if RANDOMIZED_TESTS
	for (long i = -ITERATIONS; i < ITERATIONS; i++)
	{
		str = CRIT_randstring(ITERATIONS);
    	cr_assert_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for <RANDOM STRING> -> strlen: %zu, ft_strlen: %zu", strlen(str), ft_strlen(str));
		free(str);
	}
#endif
}

#if !FSANITIZE_ADDRESS
Test(strings, ft_strcmp_segv1, .signal = SIGSEGV)
{
	ft_strcmp(NULL, "Yallaaaaa");
}

Test(strings, ft_strcmp_segv2, .signal = SIGSEGV)
{
	ft_strcmp("Yallaaaaa", NULL);
}
#endif

bool  test_ft_strcmp(char *str1, char *str2)
{
#if STRNCMP_SIGN
	return (((ft_strcmp(str1, str2) > 0) && (strcmp(str1, str2) > 0)) || ((ft_strcmp(str1, str2) < 0) && (strcmp(str1, str2) < 0)) || ((ft_strcmp(str1, str2) == 0) && (strcmp(str1, str2) == 0)));
#else
	return(ft_strcmp(str1, str2) == strcmp(str1, str2));
#endif
}

Test(strings, ft_strcmp)
{
	char *str1;
	char *str2;

	str1 = "a";
	str2 = "b";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "a\0";
	str2 = "b\200";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "b\255";
	str2 = "a\0";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "a\0";
	str2 = "b\255";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "aa";
	str2 = "b";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));
	
	str1 = "a";
	str2 = "bb";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "aYallaaaaa";
	str2 = "bYallaaaaa";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "Yallbaaaa";
	str2 = "Yallaaaaa";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "";
	str2 = "Yallaaaaa";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "Yallaaaaa";
	str2 = "";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "";
	str2 = "";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	str1 = "a";
	str2 = "b";
	cr_expect(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1{%s} s2{%s} -> ft_strcmp : %i, strcmp : %i", str1, str2, ft_strcmp(str1, str2), strcmp(str1, str2));

	srand(time(NULL));
#if RANDOMIZED_TESTS
	for (long i = -ITERATIONS; i < ITERATIONS; i++)
	{
		str1 = CRIT_randstring(ITERATIONS);
		str2 = CRIT_randstring(ITERATIONS);
		cr_assert(test_ft_strcmp(str1, str2), "Your ft_strcmp doesnt work for s1<RANDOM STRING> s2<RANDOM STRING> -> ft_strcmp : %i, strcmp : %i", ft_strcmp(str1, str2), strcmp(str1, str2));
		free(str1);
		free(str2);
	}
#endif
}

#if !FSANITIZE_ADDRESS
Test(strings, ft_strcpy_segv1, .signal = SIGSEGV)
{
	ft_strcpy(NULL, malloc(100));
}

Test(strings, ft_strcpy_segv2, .signal = SIGSEGV)
{
	ft_strcpy(malloc(100), NULL);
}
#endif

bool	test_ft_strcpy(char *src, char *dst1, char *dst2)
{
	memset(dst1, 'A', MEMSIZE);
	memset(dst2, 'A', MEMSIZE);
	strcpy(dst2, src);
	return(ft_strcpy(dst1, src) == dst1 && memcmp(dst1, dst2, MEMSIZE) == 0);
}

Test(strings, ft_strcpy)
{
	char *dst1 = malloc(MEMSIZE);
	char *dst2 = malloc(MEMSIZE);
	char *src;

	src = "\200";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "\255";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "\255\0";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "a";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "ab\0ab";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	src = "Yallaaaaa";
	cr_expect(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy{%s}, ft_strcpy{%s}", dst1, dst2);

	srand(time(NULL));
#if RANDOMIZED_TESTS
	for (int i = -ITERATIONS; i < ITERATIONS; i++)
	{
		src = CRIT_randstring(MEMSIZE);
		cr_assert(test_ft_strcpy(src, dst1, dst2) == true, "Your ft_strcpy doesnt work -> strcpy<RANDOM STRING>, ft_strcpy<RANDOM STRING>");
		free(src);
	}
#endif
	free (dst1);
	free (dst2);
}

#if !FSANITIZE_ADDRESS
Test(strings, ft_strdup_segv1, .signal = SIGSEGV)
{
	ft_strdup(NULL);
}
#endif

Test(strings, ft_strdup)
{
	char *dst1;
	char *dst2;
	char *src;

	src = "\255";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	// mutability test
	dst1[0] = 'a';
	free (dst1);
	free (dst2);

	src = "Yallaaaaaaaaaa";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	// mutability test
	dst1[0] = 'a';
	free (dst1);
	free (dst2);

	src = "Yallaaa\0aaaaaaa";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	srand(time(NULL));
#if RANDOMIZED_TESTS
	for (int i = -ITERATIONS; i < ITERATIONS; i++)
	{
		src = CRIT_randstring(200);
		dst1 = ft_strdup(src);
		dst2 = strdup(src);
		cr_assert(memcmp(dst1, dst2, strlen(src) + 1) == 0,"Your ft_strdup doesnt work -> strdup <RANDOM STRING>");
		free (src);
		free (dst1);
		free (dst2);
	}
#endif
}

bool	test_ft_read(int *fd, char *buf, size_t *n, ssize_t *rt, int *_errno)
{
	const char	*filename = "/tmp/ft_read_test.file";
	const char	*str = "Fakkaa wereld!";
	char		op[128];
	char		shad_buf[128];
	ssize_t		shad_rt;

	memset(buf, 'A', 128);
	memset(shad_buf, 'A', 128);
	
	snprintf(op, 128, "echo %s > %s", str, filename);
	system(op);

	errno = 0;
	*fd = open(filename, O_RDONLY);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	*rt = ft_read(*fd, buf, *n);
	if (*rt < 0)	*_errno = errno;
	else			cr_expect(errno == 0, "Your ft_read returns 0 or higher but you do set errno\n");
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	*fd = open(filename, O_RDONLY);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	shad_rt = read(*fd, shad_buf, *n);
	cr_expect(shad_rt == *rt, "Read bytes mismatch : notre: %zi, votre: %zi\n", shad_rt, *rt);
	
	if (shad_rt < 0)	cr_expect(*_errno == errno, "Errno mismatch : notre: %i, votre: %i\n", errno, *_errno);
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	snprintf(op, 128, "rm -f %s", filename);
	system(op);
	return (memcmp(buf, shad_buf, 128) == 0);
}

bool	test_ft_read_bad_fd(int *fd, char *buf, size_t *n, ssize_t *rt, int *_errno)
{ 
	const char	*filename = "/tmp/ft_read_test.file";
	const char	*str = "Fakkaa wereld!";
	char		op[128];
	char		shad_buf[128];
	ssize_t		shad_rt;

	memset(buf, 'A', 128);
	memset(shad_buf, 'A', 128);
	
	snprintf(op, 128, "echo %s > %s", str, filename);
	system(op);

	*fd = -1;
	errno = 0;
	*rt = ft_read(*fd, buf, *n);
	if (*rt < 0)	*_errno = errno;
	else			cr_expect(errno == 0, "Your ft_read returns 0 or higher but you do set errno\n");

	shad_rt = read(*fd, shad_buf, *n);
	cr_expect(shad_rt == *rt, "Read bytes mismatch : notre: %zi, votre: %zi\n", shad_rt, *rt);
	if (*rt < 0)	cr_expect(*_errno == errno, "Errno mismatch : notre: %i, votre: %i\n", errno, *_errno);

	snprintf(op, 128, "rm -f %s", filename);
	system(op);
	return (memcmp(buf, shad_buf, 128) == 0);
}

Test(syscalls, ft_read)
{
	char	buf[128];
	int		fd;
	int		_errno;
	size_t	n;
	ssize_t	rt;
	
	n = 5;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	n = 1;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	n = 0;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);

	n = -1;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);

	// oh yes why not
	n = INT_MIN;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	n = INT_MAX;
	cr_expect(test_ft_read(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_read_bad_fd(&fd, buf, &n, &rt, &_errno), "Your ft_read doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
}

bool	test_ft_write(int *fd, char *buf, size_t *n, ssize_t *rt, int *_errno)
{
	const char	*filename = "/tmp/ft_write_test.file";
	const char	*str = "Fakkaa wereld!";
	char		op[128];
	char		shad_buf[128];
	ssize_t		shad_rt;

	memset(buf, 'A', 128);
	memset(shad_buf, 'A', 128);

	errno = 0;

	*fd = open(filename, O_RDWR|O_TRUNC|O_CREAT);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	*rt = ft_write(*fd, str, *n);
	if (*rt < 0)	*_errno = errno;
	else			cr_expect(errno == 0, "Your ft_write returns 0 or higher but you do set errno\n");
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	*fd = open(filename, O_RDONLY);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	cr_expect(read(*fd, buf, *n) == (ssize_t)*n, "Your ft_write has not written (enough)!");
	cr_expect(read(*fd, buf, *n) <= 0, "Your ft_write has written too much");
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	*fd = open(filename, O_RDWR|O_TRUNC|O_CREAT);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	shad_rt = write(*fd, str, *n);
	if (shad_rt < 0)	cr_expect(*_errno == errno, "Errno mismatch : notre: %i, votre: %i\n", errno, *_errno);
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	*fd = open(filename, O_RDONLY);
	cr_assert(*fd > -1, "Couldn't open test file |%s|", filename);
	cr_assert(read(*fd, shad_buf, *n) == (ssize_t)*n, "Couldnt read from file!");
	cr_assert(read(*fd, shad_buf, *n) <= 0, "Couldnt read from file!");
	cr_assert(close(*fd) > -1, "Couldn't close test file |%s|", filename);

	snprintf(op, 128, "rm -f %s", filename);
	system(op);
	return (memcmp(buf, shad_buf, 128) == 0);
}

bool	test_ft_write_badfd(int *fd, char *buf, size_t *n, ssize_t *rt, int *_errno)
{
	const char	*filename = "/tmp/ft_write_test.file";
	const char	*str = "Fakkaa wereld!";
	char		op[128];
	char		shad_buf[128];
	ssize_t		shad_rt;

	memset(buf, 'A', 128);
	memset(shad_buf, 'A', 128);

	errno = 0;
	*fd = -1;

	*rt = ft_write(*fd, str, *n);
	if (*rt < 0)	*_errno = errno;
	else			cr_expect(errno == 0, "Your ft_write returns 0 or higher but you do set errno\n");

	shad_rt = write(*fd, str, *n);
	if (shad_rt < 0)	cr_expect(*_errno == errno, "Errno mismatch : notre: %i, votre: %i\n", errno, *_errno);

	snprintf(op, 128, "rm -f %s", filename);
	system(op);
	return (memcmp(buf, shad_buf, 128) == 0);
}

#if !FSANITIZE_ADDRESS
Test(syscalls, ft_write)
{
	char	buf[128];
	int		fd;
	int		_errno;
	size_t	n;
	ssize_t	rt;
	
	n = 5;
	cr_expect(test_ft_write(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_write_badfd(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);

	n = 1;
	cr_expect(test_ft_write(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_write_badfd(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);

	n = 0;
	cr_expect(test_ft_write(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_write_badfd(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);

	n = -1;
	cr_expect(test_ft_write(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
	cr_expect(test_ft_write_badfd(&fd, buf, &n, &rt, &_errno), "Your ft_write doesn't work -> ft_read: returns: %zi, errno : %i, buf : |%s|, n : %zu, fd : %i", rt, _errno, buf, n, fd);
}
#endif
