#
# **************************************************************
# *                         Nanocalc                           *
# *                                                            *
# * Author: Arash Partow (2001)                                *
# * URL: http://www.partow.net/programming/nanocalc/index.html *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Nanocalc is permitted under the guidelines *
# * and in accordance with the most current version of the     *
# * Common Public License.                                     *
# * http://www.opensource.org/licenses/cpl1.0.php              *
# *                                                            *
# **************************************************************
#


COMPILER         = -c++
#COMPILER        = -clang
OPTIMIZATION_OPT = -O1
BASE_OPTIONS     = -ansi -pedantic-errors -Wall -Wextra -Werror -Wno-long-long
OPTIONS          = $(BASE_OPTIONS) $(OPTIMIZATION_OPT) -o
LINKER_OPT       = -L/usr/lib -lstdc++

BUILD_LIST+=nanocalc

all: $(BUILD_LIST)

nanocalc: nanocalc.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) nanocalc nanocalc.cpp $(LINKER_OPT)

strip_bin:
	strip -s nanocalc

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./nanocalc

clean:
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch
