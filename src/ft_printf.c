#include "../includes/ft_printf.h"

#include <assert.h>

/*

Note: Any flags past some point are just treated as characters to print:
	printf ("%is", 5); prints "5s"
	-This functionality is already present in my code.
	Same goes for length modifiers:
	printf ("%ih", 5); prints "5h"
	-This functionality is also already present in my code

Errors to handle:
	-input isn't type specified (isn't this just compiler?)
	-unequal number of arguments and conversing specifiers (isn't this just compiler?)
	-nonsensical combinations of input? %hhll? (This is definitely just compiler...)
	-How do you throw compliler errors? Are these automatic or do you need to code for these?

*/

int	ishash(t_flags flags)
{
	return (flags.flags & 1);
}

int	iscaps(t_flags flags)
{
	return (flags.x);
}

int	prefix(int base, int depth, int cap)
{
	if (base == 16)
	{
		if (depth == 5)
			(cap) ? write(1, "0X100", 5) : write(1, "0x100", 5);
		else
			(cap) ? write(1, "0X10", 4) : write(1, "0x10", 4);
	}
	else
		write(1, "0", 1);
	return ((base == 16) ? depth : 1);
}

int    putnbr_base(long i, int base, int depth, t_flags flags)
{
	char 		out;
	static int 	d;
	int		o;

	o = 0;
	d = depth;
	if (i >= base)
		o += putnbr_base(i / base, base, depth + 1, flags);
	if (d == depth && ishash(flags))
		o += prefix(base, depth, iscaps(flags));
	if (iscaps(flags))
		out = (i % base < 10) ? '0' + i % base : 'a' + i % base % 10;
	else
		out = (i % base < 10) ? '0' + i % base : 'A' + i % base % 10;
	write(1, &out, 1);
	return (o + 1);
}

int	test_ls(char fmt_substr)
{
	if (fmt_substr == 'l')
		return (2);
	return ( fmt_substr == 'S' || fmt_substr == 'C' || \
		 fmt_substr == 'D' || fmt_substr == 'O' ||	\
		 fmt_substr == 'U'
	       );
}

/*

just stubbing...

int	dispatch_l(va_list ap, char **fmt_substr)
{
}

int     dispatch_ll(va_list ap, char **fmt_substr)
{
}

	Is it legal to combine the different length modifiers together?
	Will %hlhllc throw an error, for example?
	if no error thrown, the length modifiers are unlikely to be combined in any way,
	there are just rules for precendence that you will likewise implement.
	What does %lC do?
	Answer:

	-No, you cannot combine any of the length modifiers
	-No, you cannot do %lC or %lS
	-%lD is just a long
	-%lO is unsigned long
	-%lU is unsigned long
*/

int	get_base(char c)
{
	if (c == 'i' || c == 'd' || c == 'u')
		return (10);
	if (c == 'c')
		return (1);
	if (c == 'x' || c == 'X' || c == 'p')
		return (16);
        if (c == 'o')
		return (8);
	return (0);
}

int	standard_dispatch(va_list ap, char **fmt_substr, t_flags flags)
{
	char		*s;
	int		i;
	unsigned int	u;
	int		base;

	if (**fmt_substr == 's')
	{
		s = va_arg(ap, char *);
		(*fmt_substr)++;
		return (ft_putstrl(s));
	}
	else
		i = (**fmt_substr != 'u') ? va_arg(ap, int) : va_arg(ap, unsigned int);
	if ((base = get_base(**fmt_substr)) == 1)
		write(1, &i, 1);
	else
		base = putnbr_base(i, base, 0, flags);
	(*fmt_substr)++;
	return (base);
}

int	is_flag(char c)
{
	return (c == '#' || c == '0' || c == '-' || c == ' ' || c == '+');
}

int	test_X(char *str)
{
	while (*str && *str != ' ')
		if (*str++ == 'X')
			return (1);
	return (0);
}

void	finalize_flags(char c, t_flags *out)
{
	if (c == 'p' && (out->flags ^ 1))
		out->flags |= 1;
        if ((out->flags & 2) && (out->flags & 4)) 
                out->flags ^= 2;
        if ((out->flags & 20) && (out->flags & 10))
                out->flags ^= 10;
}

t_flags	get_flags(char **fmt_substr)
{
	t_flags out;

	out.flags = 0;
	out.x = test_X(*fmt_substr);
	out.field_width = ft_atoi(*fmt_substr);
	while(ft_isdigit(**fmt_substr))
		(*fmt_substr)++;
	while (is_flag(**fmt_substr))
	{
		if (**fmt_substr == '#' && (out.flags ^ 1))
			out.flags |= 1;
		if (**fmt_substr == '0' && (out.flags ^ 2))
                	out.flags |= 2;
		if (**fmt_substr == '-' && (out.flags ^ 4))
                	out.flags |= 4;
		if (**fmt_substr == ' ' && (out.flags ^ 10))
                	out.flags |= 10;
		if (**fmt_substr == '+' && (out.flags ^ 20))
			out.flags |= 20;
		(*fmt_substr)++;
	}
	finalize_flags(**fmt_substr, &out);
	return (out);
}

int	get_function(char **fmt_substr)
{
	int	i;

	if ((i = test_ls(**fmt_substr)))
	{
		if (*(*fmt_substr + 1) == 'l' && i == 2)
			 return (1);
		else
			return (0);
	}
	if (**fmt_substr == 'h')
	{
		if (*(*fmt_substr + 1) == 'h')
			return (3);
		else
			return (2);
	}
	if (**fmt_substr == 'j')
		return (4);
	if (**fmt_substr == 'z')
		return (5);
	else
		return (0);
}

int	dispatcher(va_list ap, char **fmt_substr)
{
	t_dispatcher 	functs[1] = {
		//&dispatch_l,
		//&dispatch_ll,
		//&dispatch_h,
		//&dispatch_hh,
		//&dispatch_j,
		//&dispatch_z,
		&standard_dispatch
	};
	return (functs[get_function(fmt_substr)](ap, fmt_substr, get_flags(fmt_substr)));
}

int	write_length(char *fmt_str)
{
	int out_len;

	out_len = 0;
	if( *(fmt_str + out_len) == '%')
		out_len++;
	while (*(fmt_str + out_len) && *(fmt_str + out_len) != '%')
		out_len++;
	return (out_len);
}

int	ft_printf(char *fmt_str, ...)
{
	va_list	ap;
	char *s;
	int bytes_printed;
	int len;

	bytes_printed = 0;
	va_start(ap, fmt_str);
	while (*fmt_str)
	{
		if (*fmt_str == '%' && *(++fmt_str) != '%')
			bytes_printed += dispatcher(ap, &fmt_str);
		else
		{
			len = write_length(fmt_str);
			bytes_printed += write(1, fmt_str, len);
			fmt_str += len;
		}
	}
	va_end(ap);
	return (bytes_printed);
}

int	main()
{
	char *string = "this is a test run to make sure that it works";
	int	i = 5;
	char	c = 'A';
	unsigned int	u = 42;
	int		bytes_printed;

	//ft_printf("Printf %% %is %ih %s and this is a number %i and a number also %d and this is a character: %c and this is an address %p\n", 5, 5, string, i, i, c, string);
	//printf ("This is the address with native function: %p\n", string);
	bytes_printed = ft_printf ("Here are all the standard conversions: %s %p %d %i %o %u %x %X %c\n", string, string, i, i, i, u, i, i, *string);
	printf ("Bytes printed were %i\n", bytes_printed);
	bytes_printed = printf ("Here are all the standard conversions: %s %p %d %i %o %u %x %X %c\n", string, string, i, i, i, u, i, i, *string);
	printf ("Bytes printed were %i\n", bytes_printed);

	assert(ft_printf("Here are all the standard conversions: %s", string) == printf("Here are all the standard conversions: %s", string));
	assert(ft_printf("%p", string) == printf ("%p", string));
	assert(ft_printf("%d", i) == printf ("%d", i));
	assert(ft_printf("%i", i) == printf ("%i", i));
	assert(ft_printf("%o", i) == printf ("%o", i));
	assert(ft_printf("%u", u) == printf ("%u", u));
	assert(ft_printf("%x", i) == printf ("%x", i));
	assert(ft_printf("%X", i) == printf ("%X", i));
	assert(ft_printf("%c", *string) == printf ("%c", *string));
	return (0);
}
