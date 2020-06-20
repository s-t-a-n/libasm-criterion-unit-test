/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   criterion_libasm.c                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: sverschu </var/mail/sverschu>                +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/24 13:21:37 by sverschu      #+#    #+#                 */
/*   Updated: 2020/06/20 16:04:44 by sverschu      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

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
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s|",str);
	str = "He\0llo";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s|",str);
	str = "Heøllo";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s|",str);
	str = "";
    cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for |%s|",str);

#if RANDOMIZED_TESTS
	for (long i = -ITERATIONS; i < ITERATIONS; i++)
	{
		str = CRIT_randstring(ITERATIONS);
    	cr_expect_eq(ft_strlen(str), strlen(str), "Your ft_strlen doesnt work for <RANDOM STRING>");
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

void test_ft_strlen(char *str1, char *str2)
{
#if STRNCMP_SIGN
	cr_expect(((ft_strcmp(str1, str2) > 0) && (strcmp(str1, str2) > 0)) || ((ft_strcmp(str1, str2) < 0) && (strcmp(str1, str2) < 0)) || ((ft_strcmp(str1, str2) == 0) && (strcmp(str1, str2) == 0)) == 1, "Your ft_strcmp doesnt work for s1{%s} s2{%s}", str1, str2);
#else
	cr_expect_eq(ft_strcmp(str1, str2, n), strcmp(str1, str2),"Your ft_strcmp doesnt work for s1{%s} s2{%s}", str1, str2);
#endif
}

Test(strings, ft_strcmp)
{
	char *str1;
	char *str2;

	str1 = "a";
	str2 = "b";
	test_ft_strlen(a, b);

	str1 = "a\0";
	str2 = "b\200";
	test_ft_strlen(a, b);

	str1 = "aa";
	str2 = "b";
	test_ft_strlen(a, b);
	
	str1 = "a";
	str2 = "bb";
	test_ft_strlen(a, b);

	str1 = "aYallaaaaa";
	str2 = "bYallaaaaa";
	test_ft_strlen(a, b);

	str1 = "Yallbaaaa";
	str2 = "Yallaaaaa";
	test_ft_strlen(a, b);

	str1 = "";
	str2 = "Yallaaaaa";
	test_ft_strlen(a, b);

	str1 = "Yallaaaaa";
	str2 = "";
	test_ft_strlen(a, b);

	str1 = "";
	str2 = "";
	test_ft_strlen(a, b);

	str1 = "a";
	str2 = "b";
	test_ft_strlen(a, b);

#if RANDOMIZED_TESTS
	for (long i = -ITERATIONS; i < ITERATIONS; i++)
	{
		str1 = CRIT_randstring(ITERATIONS);
		str2 = CRIT_randstring(ITERATIONS);
		test_ft_strlen(a, b);
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

void test_ft_strcpy(char *src, char *dst1, char *dst2)
{
	memset(dst1, 'A', MEMSIZE);
	memset(dst2, 'A', MEMSIZE);
	cr_expect_eq(ft_strcpy(dst1, src), strcpy(dst2, src),"Your ft_strcpy doesnt work for src{%s} n{%i}", src, (int)n);
	cr_expect(memcmp(dst1, dst2, MEMSIZE) == 0,"Your ft_strcpy doesnt work -> strlcpy{%s}, ft_strcpy{%s}", dst1, dst2);
}

Test(strings, ft_strcpy)
{
	char *dst1 = malloc(MEMSIZE);
	char *dst2 = malloc(MEMSIZE);
	char *src;

	src = "\200";
	test_ft_strcpy(src, dst1, dst2);

	src = "\255";
	test_ft_strcpy(src, dst1, dst2);

	src = "\255\0";
	test_ft_strcpy(src, dst1, dst2);

	src = "";
	test_ft_strcpy(src, dst1, dst2);

	src = "a";
	test_ft_strcpy(src, dst1, dst2);

	src = "ab\0ab";
	test_ft_strcpy(src, dst1, dst2);

	src = "Yallaaaaa";
	test_ft_strcpy(src, dst1, dst2);

#if RANDOMIZED_TESTS
	for (int i = -ITERATIONS; i < ITERATIONS; i++)
	{
		src = CRIT_randstring(MEMSIZE);
		test_ft_strcpy(src, dst1, dst2);
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
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"1: Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"1: Your ft_strdup doesnt work -> strdup{%s}", src);
	// mutability test
	dst1[0] = 'a';
	free (dst1);
	free (dst2);

	src = "Yallaaaaaaaaaa";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"2: Your ft_strdup doesnt work -> strdup{%s}", src);
	// mutability test
	dst1[0] = 'a';
	free (dst1);
	free (dst2);

	src = "Yallaaa\0aaaaaaa";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"3: Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"4: Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

	src = "";
	dst1 = ft_strdup(src);
	dst2 = strdup(src);
	cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"5: Your ft_strdup doesnt work -> strdup{%s}", src);
	free (dst1);
	free (dst2);

#if RANDOMIZED_TESTS
	for (int i = -ITERATIONS; i < ITERATIONS; i++)
	{
		src = CRIT_randstring(200);
		dst1 = ft_strdup(src);
		dst2 = strdup(src);
		cr_expect(memcmp(dst1, dst2, strlen(src) + 1) == 0,"5: Your ft_strdup doesnt work -> strdup <RANDOM STRING>");
		free (src);
		free (dst1);
		free (dst2);
	}
#endif
} 
