


#include <unistd.h>




int	main()
{
	int i;

	i = 35;

	write(1, &i, 1);

	return (0);
}
